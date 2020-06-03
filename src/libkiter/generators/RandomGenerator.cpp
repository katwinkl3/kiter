/*
 * RandomGenerator.cpp
 *
 *  Created on: Apr 30, 2020
 *      Author: toky
 */


#include "RandomGenerator.h"
#include <algorithms/schedulings.h>
#include <algorithms/normalization.h>
#include <algorithms/repetition_vector.h>
#include <random>

models::Dataflow*  generators::generate_random_sdf(parameters_list_t   param_list) {
		RandomGeneratorConfiguration config;
		config.min_vertices_count       = 10;
		config.max_vertices_count       = 20;
		config.min_edges_count          = 20;
		config.max_edges_count          = 30;
		config.max_weight               = 10;
		config.max_duration             = 1;
		config.max_phase_quantity       = 1;
		config.max_init_phase_quantity  = 0;

		if (param_list.count("min_vertices_count")) { config.min_vertices_count = (commons::fromString<ARRAY_INDEX>(param_list["min_vertices_count"])); }
		if (param_list.count("min_edges_count"))    { config.min_edges_count   = (commons::fromString<ARRAY_INDEX>(param_list["min_edges_count"])); }

		if (param_list.count("max_vertices_count")) { config.max_vertices_count = (commons::fromString<ARRAY_INDEX>(param_list["max_vertices_count"])); }
		if (param_list.count("max_edges_count")) { config.max_edges_count = (commons::fromString<ARRAY_INDEX>(param_list["max_edges_count"])); }
		if (param_list.count("min_vertices_count")) { config.min_vertices_count = (commons::fromString<ARRAY_INDEX>(param_list["min_vertices_count"])); }
		if (param_list.count("min_edges_count")) { config.min_edges_count = (commons::fromString<ARRAY_INDEX>(param_list["min_edges_count"])); }
		if (param_list.count("max_weight")) { config.max_weight = (commons::fromString<ARRAY_INDEX>(param_list["max_weight"])); }
		if (param_list.count("max_duration")) { config.max_duration = (commons::fromString<TIME_UNIT>(param_list["max_duration"])); }
		if (param_list.count("max_phase_quantity")) { config.max_phase_quantity = (commons::fromString<ARRAY_INDEX>(param_list["max_phase_quantity"])); }
		if (param_list.count("max_init_phase_quantity")) { config.max_init_phase_quantity = (commons::fromString<ARRAY_INDEX>(param_list["max_init_phase_quantity"])); }

		RandomGenerator generator (config);
		int seed = 124;
		if (param_list.count("seed")) {
			seed = (commons::fromString<int>(param_list["seed"]));
		}
		return generator.generate(seed);
}


static bool set_initial_marking (models::Dataflow * dataflow) {


	for (bool stopping_condition = false ; !stopping_condition ; ) {
		VERBOSE_INFO("increase marking continue");
	    VERBOSE_ASSERT(computeRepetitionVector(dataflow),"inconsistent graph");
		models::Scheduling scheduling_result = algorithms::scheduling::CSDF_KPeriodicScheduling(dataflow);
		VERBOSE_INFO("Period = " << scheduling_result.getGraphPeriod());

		if (scheduling_result.getGraphThroughput() <= 0) {
			critical_circuit_t critical_edges = scheduling_result.getCriticalEdges();
			VERBOSE_ASSERT(critical_edges.size(), "Scheduling does not provide a valid result");
			Edge e = *critical_edges.begin();
			TOKEN_UNIT current_size = dataflow->getPreload(e);
			dataflow->reset_computation();
			dataflow->setPreload(e, current_size + dataflow->getFineGCD(e)) ;

		} else {
			VERBOSE_INFO("increase marking finished.");
			stopping_condition = true;
		}

	}

	return true;

}


models::Dataflow * RandomGenerator::generate (int seed) {
		VERBOSE_INFO("RandomGenerator::generate  seed =" << seed);
		std::default_random_engine generator (seed);

		VERBOSE_ASSERT(_configuration.min_vertices_count <=_configuration.max_vertices_count, "Unsupported configuration");
		VERBOSE_ASSERT(_configuration.min_edges_count <=_configuration.max_edges_count, "Unsupported configuration");
		VERBOSE_ASSERT(1 <= _configuration.max_duration, "Unsupported configuration");
		VERBOSE_ASSERT(1 <= _configuration.max_weight, "Unsupported configuration");
		VERBOSE_ASSERT(1 <= _configuration.max_phase_quantity, "Unsupported configuration");
		VERBOSE_ASSERT(0 <= _configuration.max_init_phase_quantity, "Unsupported configuration");

		std::uniform_int_distribution<ARRAY_INDEX> weight_distribution(1,_configuration.max_weight);
		std::uniform_int_distribution<ARRAY_INDEX> vertex_count_distribution(_configuration.min_vertices_count,_configuration.max_vertices_count);
		std::uniform_int_distribution<ARRAY_INDEX> edges_count_distribution(_configuration.min_edges_count,_configuration.max_edges_count);
		std::uniform_int_distribution<ARRAY_INDEX> duration_distribution(1, (ARRAY_INDEX) _configuration.max_duration);
		std::uniform_int_distribution<PHASE_INDEX> phase_distribution(1, (PHASE_INDEX) _configuration.max_phase_quantity);
		std::uniform_int_distribution<PHASE_INDEX> init_phase_distribution(0, (PHASE_INDEX) _configuration.max_init_phase_quantity);

		// STEP 1 - global shape, edges count and vertices count

		models::Dataflow * dataflow = new models::Dataflow ();
		std::string name = "autogen_" + commons::toString(seed);
		dataflow->setAppName(name);
		dataflow->setGraphName(name);
		dataflow->setGraphType("AutoGenerated");

		const ARRAY_INDEX vertices_count = vertex_count_distribution(generator);
		const ARRAY_INDEX edges_count = edges_count_distribution(generator);

		std::uniform_int_distribution<ARRAY_INDEX> vertex_distribution(1,vertices_count);
		std::uniform_int_distribution<ARRAY_INDEX> edge_distribution(1,edges_count);


		// STEP 2 - create tasks, assign init and periodic phases with durations, and compute normalized weights

		std::map<ARRAY_INDEX,TOKEN_UNIT> normalized_weights;


		for (ARRAY_INDEX i = 1 ; i <= vertices_count ; i++) {
			VERBOSE_INFO("Autogen new task");
			Vertex nv = dataflow->addVertex(i);
			dataflow->setVertexName(nv,"Node_" + commons::toString(dataflow->getVertexId(nv)));

			if (not _configuration.no_reentrancy) {
				dataflow->setReentrancyFactor(nv,1);
			}

			PHASE_INDEX init_phase_count = init_phase_distribution(generator) ;
			PHASE_INDEX periodic_phase_count = phase_distribution(generator) ;

			dataflow->setInitPhasesQuantity(nv, init_phase_count);
			dataflow->setPhasesQuantity(nv, periodic_phase_count);

			std::vector <TIME_UNIT> init_phase_duration (init_phase_count);
			std::vector <TIME_UNIT> periodic_phase_duration (periodic_phase_count);

			for (PHASE_INDEX ip = 0 ; ip < init_phase_count ; ip++) {
				init_phase_duration[ip] = (TIME_UNIT)duration_distribution(generator);
			}
			for (PHASE_INDEX pp = 0 ; pp < periodic_phase_count ; pp++) {
				periodic_phase_duration[pp] = (TIME_UNIT)duration_distribution(generator);
			}

			VERBOSE_INFO("   init_phase_duration = " << commons::toString(init_phase_duration) << "   periodic_phase_duration = " << commons::toString(periodic_phase_duration));

			dataflow->setVertexDuration(nv,periodic_phase_duration);
			dataflow->setVertexInitDuration(nv,init_phase_duration);

			normalized_weights[i] = weight_distribution(generator);

			VERBOSE_INFO("Autogen task_id=" << i << "/"  << vertices_count << " Z=" << normalized_weights[i] << " init_phase_count=" << init_phase_count << " periodic_phase_count=" << periodic_phase_count);
		}


		for (ARRAY_INDEX i = 1 ; i <= edges_count ; i++) {

			ARRAY_INDEX src_id = vertex_distribution(generator);
			ARRAY_INDEX dst_id = vertex_distribution(generator);

			VERBOSE_ASSERT((!_configuration.no_self_loop) or (dataflow->getVerticesCount() > 1), "Impossible generation case");

			while (_configuration.no_self_loop and (src_id == dst_id)) {
				VERBOSE_INFO("Renew self-loop edge" << i);
				dst_id = vertex_distribution(generator);
			}

			Vertex source = dataflow->getVertexById(src_id);
			Vertex target = dataflow->getVertexById(dst_id);

			TOKEN_UNIT nweight_in = normalized_weights[src_id];
			TOKEN_UNIT nweight_out = normalized_weights[dst_id];

			TOKEN_UNIT gcdz   =  boost::integer::gcd(nweight_in,nweight_out);
			TOKEN_UNIT max_factor =  (TOKEN_UNIT) std::round ( std::min ( _configuration.max_weight / (nweight_in / gcdz) , _configuration.max_weight / (nweight_out / gcdz) ) ) ;

			std::uniform_int_distribution<TOKEN_UNIT> factor_distribution(1, max_factor);
			TOKEN_UNIT factor = factor_distribution(generator);

			TOKEN_UNIT weight_in_periodic =  (nweight_in / gcdz) * factor;
			TOKEN_UNIT weight_out_periodic=  (nweight_out / gcdz) * factor;
			TOKEN_UNIT weight_in_init =  weight_distribution(generator);
			TOKEN_UNIT weight_out_init=  weight_distribution(generator);


			std::uniform_int_distribution<TOKEN_UNIT> in_periodic_sections_distribution(0, weight_in_periodic);
			std::uniform_int_distribution<TOKEN_UNIT> out_periodic_sections_distribution(0, weight_out_periodic);
			std::uniform_int_distribution<TOKEN_UNIT> in_init_sections_distribution(0, weight_in_init);
			std::uniform_int_distribution<TOKEN_UNIT> out_init_sections_distribution(0, weight_out_init);

			std::vector<TOKEN_UNIT> in_periodic_sections;
			std::vector<TOKEN_UNIT> out_periodic_sections;
			std::vector<TOKEN_UNIT> in_init_sections;
			std::vector<TOKEN_UNIT> out_init_sections;

			PHASE_INDEX periodic_in_phase_count = dataflow->getPhasesQuantity(source) ;
			PHASE_INDEX periodic_out_phase_count = dataflow->getPhasesQuantity(target);
			PHASE_INDEX init_in_phase_count = dataflow->getInitPhasesQuantity(source) ;
			PHASE_INDEX init_out_phase_count = dataflow->getInitPhasesQuantity(target);

			for (PHASE_INDEX p = 1 ; p < periodic_in_phase_count ; p++) {
				in_periodic_sections.push_back(in_periodic_sections_distribution(generator));
			}
			in_periodic_sections.push_back(weight_in_periodic);

			for (PHASE_INDEX p = 1 ; p < periodic_out_phase_count ; p++) {
				out_periodic_sections.push_back(out_periodic_sections_distribution(generator));
			}
			out_periodic_sections.push_back(weight_out_periodic);

			for (PHASE_INDEX p = 1 ; p < init_in_phase_count ; p++) {
				in_init_sections.push_back(in_init_sections_distribution(generator));
			}
			in_init_sections.push_back(weight_in_init);

			for (PHASE_INDEX p = 1 ; p < init_out_phase_count ; p++) {
				out_init_sections.push_back(out_init_sections_distribution(generator));
			}
			out_init_sections.push_back(weight_out_init);


			std::sort(in_init_sections.begin(), in_init_sections.end());
			std::sort(out_init_sections.begin(), out_init_sections.end());
			std::sort(in_periodic_sections.begin(), in_periodic_sections.end());
			std::sort(out_periodic_sections.begin(), out_periodic_sections.end());


			std::vector<TOKEN_UNIT> in_periodic_phases;
			std::vector<TOKEN_UNIT> out_periodic_phases;

			std::vector<TOKEN_UNIT> in_init_phases;
			std::vector<TOKEN_UNIT> out_init_phases;

			for (PHASE_INDEX p = 0 , cumul = 0 ; p < periodic_in_phase_count ; p++) {
				PHASE_INDEX current = in_periodic_sections[p] - cumul;
				cumul += current;
				in_periodic_phases.push_back(current);
			}

			for (PHASE_INDEX p = 0 , cumul = 0 ; p < periodic_out_phase_count ; p++) {
				PHASE_INDEX current = out_periodic_sections[p] - cumul;
				cumul += current;
				out_periodic_phases.push_back(current);
			}

			for (PHASE_INDEX p = 0 , cumul = 0 ; p < init_in_phase_count ; p++) {
				PHASE_INDEX current = in_init_sections[p] - cumul;
				cumul += current;
				in_init_phases.push_back(current);
			}

			for (PHASE_INDEX p = 0 , cumul = 0 ; p < init_out_phase_count ; p++) {
				PHASE_INDEX current = out_init_sections[p] - cumul;
				cumul += current;
				out_init_phases.push_back(current);
			}



			Edge c = dataflow->addEdge(source,target,i,"buffer_" + commons::toString(i));


			dataflow->setEdgeInPhases(c, in_periodic_phases);
			dataflow->setEdgeOutPhases(c,out_periodic_phases);


			dataflow->setEdgeInInitPhases(c, in_init_phases);
			dataflow->setEdgeOutInitPhases(c, out_init_phases);


			VERBOSE_INFO("Autogen edge_id=" << i << " nweight_in=" << nweight_in << " nweight_out=" << nweight_out);
			VERBOSE_INFO("         " << " gcdz=" <<  gcdz  << "  factor=" <<  factor  );
			VERBOSE_INFO("         "
			<< "  weight_in_periodic=" <<  weight_in_periodic << "  weight_out_periodic=" <<  weight_out_periodic
			 << " in_periodic_phases=" <<  commons::toString(in_periodic_phases) << " out_periodic_phases=" <<  commons::toString(out_periodic_phases)
			<< " in_periodic_sections=" <<  commons::toString(in_periodic_sections)  << " out_periodic_sections=" <<  commons::toString(out_periodic_sections)  );
			VERBOSE_INFO("         "
			<< "  weight_in_init=" <<  weight_in_init << "  weight_out_init=" <<  weight_out_init
			 << " in_init_phases=" <<  commons::toString(in_init_phases) << " out_init_phases=" <<  commons::toString(out_init_phases)
			<< " in_init_sections=" <<  commons::toString(in_init_sections)  << " out_init_sections=" <<  commons::toString(out_init_sections)  );


		}

		//VERBOSE_ASSERT(algorithms::normalize(dataflow), "Not consistent");
		for (ARRAY_INDEX i = 1 ; i <= vertices_count ; i++) {
			Vertex v = dataflow->getVertexById(i);
			// TODO : This cannot work for now because normalized_weights is not normalized_weights
			//VERBOSE_ASSERT(normalized_weights[i] == dataflow->getZi(v), "Normalized weight does not match expectation");
		}

		VERBOSE_ASSERT(set_initial_marking(dataflow), "Minimal initial computation failed.");


		return dataflow;
	}
