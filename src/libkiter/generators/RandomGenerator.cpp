/*
 * RandomGenerator.cpp
 *
 *  Created on: Apr 30, 2020
 *      Author: toky
 */


#include <generators/RandomGenerator.h>
#include <algorithms/buffersizing/periodic.h>
#include <random>

	models::Dataflow * RandomGenerator::generate (int seed) {

		std::default_random_engine generator (seed);
		std::uniform_int_distribution<ARRAY_INDEX> weight_distribution(1,_configuration.max_weight);
		std::uniform_int_distribution<ARRAY_INDEX> vertex_count_distribution(1,_configuration.max_vertices_count);
		std::uniform_int_distribution<ARRAY_INDEX> edges_count_distribution(1,_configuration.max_edges_count);
		std::uniform_int_distribution<ARRAY_INDEX> duration_distribution(1, (ARRAY_INDEX) _configuration.max_duration);
		std::uniform_int_distribution<PHASE_INDEX> phase_distribution(1, (PHASE_INDEX) _configuration.max_phase_quantity);
		std::uniform_int_distribution<PHASE_INDEX> init_phase_distribution(0, (PHASE_INDEX) _configuration.max_init_phase_quantity);

		// STEP 1 - global shape, edges count and vertices count

		models::Dataflow * dataflow = new models::Dataflow ();

		const ARRAY_INDEX vertices_count = vertex_count_distribution(generator);
		const ARRAY_INDEX edges_count = edges_count_distribution(generator);

		std::uniform_int_distribution<ARRAY_INDEX> vertex_distribution(1,vertices_count);
		std::uniform_int_distribution<ARRAY_INDEX> edge_distribution(1,edges_count);


		// STEP 2 - create tasks, assign init and periodic phases with durations, and compute normalized weights

		std::map<ARRAY_INDEX,TOKEN_UNIT> normalized_weights;

		for (ARRAY_INDEX i = 1 ; i <= vertices_count ; i++) {

			Vertex nv = dataflow->addVertex(i);

			PHASE_INDEX init_phase_count = init_phase_distribution(generator) ;
			PHASE_INDEX periodic_phase_count = phase_distribution(generator) ;

			dataflow->setInitPhasesQuantity(nv, init_phase_count);
			dataflow->setPhasesQuantity(nv, periodic_phase_count);

			std::vector <TIME_UNIT> init_phase_duration (init_phase_count);
			std::vector <TIME_UNIT> periodic_phase_duration (periodic_phase_count);

			for (PHASE_INDEX ip = 0 ; ip < init_phase_count ; ip++) {
				init_phase_duration[ip] = (TIME_UNIT)duration_distribution(generator);
			}
			for (PHASE_INDEX pp = 0 ; pp < init_phase_count ; pp++) {
				periodic_phase_duration[pp] = (TIME_UNIT)duration_distribution(generator);
			}


			dataflow->setVertexDuration(nv,periodic_phase_duration);
			dataflow->setVertexInitDuration(nv,init_phase_duration);

			normalized_weights[i] = weight_distribution(generator);
		}


		for (ARRAY_INDEX i = 1 ; i <= edges_count ; i++) {

			ARRAY_INDEX src_id = vertex_distribution(generator);
			ARRAY_INDEX dst_id = vertex_distribution(generator);

			Vertex source = dataflow->getVertexById(src_id);
			Vertex target = dataflow->getVertexById(dst_id);

			TOKEN_UNIT nweight_in = normalized_weights[src_id];
			TOKEN_UNIT nweight_out = normalized_weights[dst_id];

			TOKEN_UNIT gcdz   =  boost::integer::gcd(nweight_in,nweight_out);
			TOKEN_UNIT factor =  std::round ( std::min ( _configuration.max_weight / (nweight_in / gcdz) , _configuration.max_weight / (nweight_out / gcdz) ) ) ;

			TOKEN_UNIT weight_in =  (nweight_in / gcdz) * factor;
			TOKEN_UNIT weight_out=  (nweight_out / gcdz) * factor;


			std::uniform_int_distribution<TOKEN_UNIT> in_sections_distribution(0, weight_in);
			std::uniform_int_distribution<TOKEN_UNIT> out_sections_distribution(0, weight_out);

			std::vector<TOKEN_UNIT> in_sections;
			std::vector<TOKEN_UNIT> out_sections;

			PHASE_INDEX init_in_phase_count = dataflow->getInitPhasesQuantity(source) ;
			PHASE_INDEX init_out_phase_count = dataflow->getInitPhasesQuantity(target);

			PHASE_INDEX total_in_phase_count = dataflow->getInitPhasesQuantity(source) + dataflow->getPhasesQuantity(source);
			PHASE_INDEX total_out_phase_count = dataflow->getInitPhasesQuantity(target) + dataflow->getPhasesQuantity(target);

			for (PHASE_INDEX p = 0 ; p < total_in_phase_count ; p++) {
				in_sections.push_back(in_sections_distribution(generator));
			}

			for (PHASE_INDEX p = 0 ; p < total_out_phase_count ; p++) {
				out_sections.push_back(in_sections_distribution(generator));
			}

			std::sort(in_sections.begin(), in_sections.end());
			std::sort(out_sections.begin(), out_sections.end());


			std::vector<TOKEN_UNIT> in_init_phases;
			std::vector<TOKEN_UNIT> in_periodic_phases;

			std::vector<TOKEN_UNIT> out_init_phases;
			std::vector<TOKEN_UNIT> out_periodic_phases;

			for (PHASE_INDEX p = 0 , cumul = 0 ; p < total_in_phase_count ; p++) {
				PHASE_INDEX current = in_sections[p] - cumul;
				cumul += current;
				if (p < init_in_phase_count) {
					in_init_phases.push_back(current);
				} else {
					in_periodic_phases.push_back(current);
				}
			}

			for (PHASE_INDEX p = 0 , cumul = 0 ; p < total_out_phase_count ; p++) {
				PHASE_INDEX current = out_sections[p] - cumul;
				cumul += current;
				if (p < init_out_phase_count) {
					out_init_phases.push_back(current);
				} else {
					out_periodic_phases.push_back(current);
				}
			}


			Edge c = dataflow->addEdge(source,target,i);
			dataflow->setEdgeInInitPhases(c, in_init_phases);
			dataflow->setEdgeOutInitPhases(c, out_init_phases);
			dataflow->setEdgeInPhases(c, in_periodic_phases);
			dataflow->setEdgeOutPhases(c,out_periodic_phases);

		}

		// Last step,
		VERBOSE_ASSERT(false, "Minimal initial marking not computed");

		return dataflow;
	}
