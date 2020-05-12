/*
 * merging.cpp
 *
 *  Created on: Nov 13, 2019
 *      Author: toky
 */


#include <models/Dataflow.h>
#include <models/Scheduling.h>
#include <algorithms/repetition_vector.h>
#include <algorithms/transformation/merging.h>
#include <algorithms/schedulings.h>
#include <string>
#include <algorithm>

//Function to remove the edges and obtain phase vector values
//v1 -> cfg -> v2
static void getEdgesPhaseVector(models::Dataflow* d, Vertex& v1, Vertex cfg, Vertex& v2, std::vector<TOKEN_UNIT>& in, std::vector<TOKEN_UNIT>& out)
{
	//std::cout << "inside fn: v1=" << d->getVertexName(v1) << ",cfg=" << d->getVertexName(cfg) << ",v2=" << d->getVertexName(v2) << "\n";
	{ForInputEdges(d, cfg, E)	{
		in = d->getEdgeInVector(E);
		v1 = d->getEdgeSource(E);
		d->removeEdge(E);
		break;
	}}
	{ForOutputEdges(d, cfg, E)	{
		out = d->getEdgeOutVector(E);
		v2 = d->getEdgeTarget(E);
		d->removeEdge(E);
		break;
	}}
}


bool algorithms::transformation::mergeCSDFFromKperiodicSchedule(models::Dataflow* to, std::string name , std::vector< ARRAY_INDEX >& mergeNodes) {
	to->reset_computation();
	VERBOSE_ASSERT(computeRepetitionVector(to),"inconsistent graph");
	models::Scheduling scheduling_res = algorithms::scheduling::CSDF_KPeriodicScheduling(to);
	return algorithms::transformation::mergeCSDFFromSchedule(to,name,mergeNodes,&scheduling_res);
}

bool algorithms::transformation::basic_mergeCSDFFromSchedule(models::Dataflow* to, std::string name , const std::vector< ARRAY_INDEX >& mergeNodes, const models::Scheduling* scheduling_res) {

	VERBOSE_INFO("Start basic_mergeCSDFFromSchedule");
	// Bad idea cannot check easily the sched is for dataflow
	TIME_UNIT omega = scheduling_res->getGraphPeriod();
	VERBOSE_ASSERT(omega != std::numeric_limits<TIME_UNIT>::infinity(), "Infinite period, this dataflow does not schedule, thus I cannot merge anymore.");


	auto persched = scheduling_res->getTaskSchedule();


	// step two, we find the maximum time to reach by init phases
	TIME_UNIT max_start = 0;
	for (auto vid : mergeNodes) {
		max_start = std::max(max_start, persched[vid].second[0]);
	}

	//Find GCD value
	EXEC_COUNT gcd_value = 0;
	for(ARRAY_INDEX vid :mergeNodes)
	{
		Vertex vi     = to->getVertexById(vid);
		EXEC_COUNT ni = to->getNi(vi);
		gcd_value = boost::integer::gcd (gcd_value , ni) ;
	}




	//step two , we produce the init phases for each task
	std::vector < std::pair<TIME_UNIT , ARRAY_INDEX> > init_start_times ;
	std::vector < std::pair<TIME_UNIT , ARRAY_INDEX> > periodic_start_times ;
	for (auto vid : mergeNodes) {
		Vertex vi     = to->getVertexById(vid);
		std::string vname = to->getVertexName(vi);
		EXEC_COUNT ni = to->getNi(vi);
		VERBOSE_INFO("For task " << vid << " (" <<  vname << ") period is " << std::fixed  << std::setprecision(2) << persched[vid].first << " and starts " << commons::toString(persched[vid].second));
		ARRAY_INDEX current_start_index = 0;
		TIME_UNIT current_start_time = persched[vid].second[0];

		while (current_start_time < max_start) {
			init_start_times.push_back(std::make_pair(current_start_time , vid));
			current_start_index++;
			current_start_time = persched[vid].second[current_start_index % persched[vid].second.size()]  + (current_start_index / persched[vid].second.size() ) * persched[vid].first;
		}

		for (EXEC_COUNT i = 0 ; i <  to->getPhasesQuantity(vi) * (ni/gcd_value) ; i++) {
			periodic_start_times.push_back(std::make_pair(current_start_time , vid));
			current_start_index++;
			current_start_time = persched[vid].second[current_start_index % persched[vid].second.size()]  + (current_start_index / persched[vid].second.size() ) * persched[vid].first;
		}

	}

	std::sort(init_start_times.begin(), init_start_times.end());
	std::sort(periodic_start_times.begin(), periodic_start_times.end());

	VERBOSE_INFO("init_start_times = " << commons::toString(init_start_times));
	VERBOSE_INFO("periodic_start_times = " << commons::toString(periodic_start_times));

	std::vector<TIME_UNIT> phaseDurVec(periodic_start_times.size(), 0); //Create the phase duration
	std::vector<TIME_UNIT> InitPhaseDurVec(init_start_times.size(), 0); //Create the phase duration




	// TODO : set durations!!!!

	to->reset_computation();

	//3. Create the BIG router
	auto middle = to->addVertex();
	to->setVertexName(middle, name);
	to->setInitPhasesQuantity(middle, init_start_times.size()); // number of state for the actor, only one in SDF
	to->setPhasesQuantity(middle, periodic_start_times.size()); // number of state for the actor, only one in SDF
	to->setVertexDuration(middle, phaseDurVec); // is specify for every state , only one for SDF.
	to->setVertexInitDuration(middle, InitPhaseDurVec); // is specify for every state , only one for SDF.
	to->setReentrancyFactor(middle, 1); // This is the reentrancy, it avoid a task to be executed more than once at the same time.


	std::map <ARRAY_INDEX , std::vector <TOKEN_UNIT> > periodic_token_vec ;
	std::map <ARRAY_INDEX , std::vector <TOKEN_UNIT> > init_token_vec ;

	for(ARRAY_INDEX vid :mergeNodes) {
		periodic_token_vec[vid]  = std::vector <TOKEN_UNIT> (periodic_start_times.size(), 0);
		init_token_vec[vid]  = std::vector <TOKEN_UNIT> (init_start_times.size(), 0);
	}

	for (unsigned int i = 0 ; i < periodic_start_times.size() ; i++) {
		periodic_token_vec [periodic_start_times[i].second][i] = 1; // TODO : Put real timing
	}

	for (unsigned int i = 0 ; i < init_start_times.size() ; i++) {
		init_token_vec [init_start_times[i].second][i] = 1;
	}

	//4. Remove the edges before/after the config node. Setup the flow. Remove the router nodes.

	for(ARRAY_INDEX vid :mergeNodes)
	{
		Vertex cfgVtx = to->getVertexById(vid);
		Vertex v1;
		Vertex v2;
		std::vector<TOKEN_UNIT> myin, myout;
		getEdgesPhaseVector(to, v1, cfgVtx, v2, myin, myout);

		auto new_edge = to->addEdge(v1, middle);
		to->setEdgeName(new_edge, "merged_" + commons::toString(to->getEdgeId(new_edge)));
		to->setEdgeType(new_edge,EDGE_TYPE::BUFFERLESS_EDGE);
		to->setPreload(new_edge, 0);

		to->setEdgeInPhases(new_edge, myin);
		to->setEdgeOutPhases(new_edge, periodic_token_vec[vid]);
		to->setEdgeOutInitPhases(new_edge, init_token_vec[vid]);

		new_edge = to->addEdge(middle, v2);
		to->setEdgeName(new_edge, "merged_" + commons::toString(to->getEdgeId(new_edge)));
		to->setEdgeType(new_edge,EDGE_TYPE::BUFFERLESS_EDGE);
		to->setPreload(new_edge, 0);
		to->setEdgeInPhases(new_edge, periodic_token_vec[vid]);
		to->setEdgeInInitPhases(new_edge, init_token_vec[vid]);
		to->setEdgeOutPhases(new_edge, myout);
	}


	for(ARRAY_INDEX vid :mergeNodes)
	{
		Vertex cfgVtx = to->getVertexById(vid);
		to->removeVertex(cfgVtx);
	}

	return true;

}


struct edge_infos {

	ARRAY_INDEX src_id;
	ARRAY_INDEX dst_id;

	std::vector<TOKEN_UNIT> in_per_vector;
	std::vector<TOKEN_UNIT> in_ini_vector;

	std::vector<TOKEN_UNIT> out_per_vector;
	std::vector<TOKEN_UNIT> out_ini_vector;

	TOKEN_UNIT preload;

	EDGE_TYPE type;


	edge_infos (models::Dataflow* to, Edge e) :
		src_id (to->getVertexId(to->getEdgeSource(e))),
		dst_id (to->getVertexId(to->getEdgeTarget(e))),
		in_per_vector(to->getEdgeInVector(e)),
		in_ini_vector(to->getEdgeInitInVector(e)),
		out_per_vector(to->getEdgeOutVector(e)),
		out_ini_vector(to->getEdgeInitOutVector(e)),
		preload(to->getPreload(e)),
		type(to->getEdgeType(e)) {

	}

};



bool algorithms::transformation::mergeCSDFFromSchedule(models::Dataflow* to, std::string name , const std::vector< ARRAY_INDEX >& mergeNodes, const models::Scheduling* scheduling_res) {

	VERBOSE_INFO("Start mergeCSDFFromSchedule");

	// we assume there is no init phases to merge




	VERBOSE_INFO("Store what to merge");

	// For each edge we need to store , input vector, output vector, initial marking

	std::map<ARRAY_INDEX, std::vector<edge_infos>> input_edges;
	std::map<ARRAY_INDEX,  std::vector<edge_infos>> output_edges;


	for (auto vid : mergeNodes) {
		Vertex v = to->getVertexById(vid);


		for (auto it : to->in_edges(v)) {
			input_edges[vid].push_back(edge_infos(to, *it));
		}
		for (auto it : to->out_edges(v)) {
			output_edges[vid].push_back(edge_infos(to, *it));
		}

	}

	VERBOSE_INFO("Schedule original tasks");

	//Find GCD value
	EXEC_COUNT gcd_value = 0;
	for(ARRAY_INDEX vid :mergeNodes)
	{
		Vertex vi     = to->getVertexById(vid);
		EXEC_COUNT ni = to->getNi(vi);
		gcd_value = boost::integer::gcd (gcd_value , ni) ;
	}


	TIME_UNIT omega = scheduling_res->getGraphPeriod();
	VERBOSE_ASSERT(omega != std::numeric_limits<TIME_UNIT>::infinity(), "Infinite period, this dataflow does not schedule, thus I cannot merge anymore.");
	VERBOSE_INFO("omega = " << omega);
	auto persched = scheduling_res->getTaskSchedule();
	EXEC_COUNT init_executions_count = 0;
	EXEC_COUNT periodic_executions_count = 0;
	std::map<ARRAY_INDEX, EXEC_COUNT> symbolic_execution;
	std::map<ARRAY_INDEX, EXEC_COUNT> objective;

	// TODO : This is not a good idea, not generic enough
	for (auto vid : mergeNodes) {
		Vertex v = to->getVertexById(vid);
		EXEC_COUNT ni = to->getNi(v);
		init_executions_count += to->getInitPhasesQuantity(v);
		periodic_executions_count += to->getPhasesQuantity(v) * (ni/gcd_value);
		objective[vid] = to->getInitPhasesQuantity(v) + to->getPhasesQuantity(v);
		symbolic_execution[vid] = 0;
		VERBOSE_INFO(" - Original task " << vid << " has ni = " << ni <<  ", " << to->getInitPhasesQuantity(v) << " init phases and " << to->getPhasesQuantity(v) <<  " periodic phases.");

	}
	VERBOSE_INFO(" - Merged task will have at least " << init_executions_count << " init phases and " << periodic_executions_count <<  " periodic phases.");

	VERBOSE_ASSERT(init_executions_count == 0 , "Unsupported case");

	// we perform a symbolic execution to sequence task execution from scheduling

	std::vector<ARRAY_INDEX> task_order;
	TIME_UNIT last_time = 0;

	for (bool need_to_execute = true  ; need_to_execute ; ) {
		need_to_execute = false;
		VERBOSE_DEBUG(" - Symbolic execution iteration");
		VERBOSE_DEBUG("  - last_time = " << last_time);
		VERBOSE_INFO("  - Task order is " << commons::toString(task_order));

		ARRAY_INDEX next_id = -1;
		TIME_UNIT next_time = std::numeric_limits<TIME_UNIT>::infinity() ;
		for (auto vid : mergeNodes) {
			VERBOSE_DEBUG("  - Test task " << vid);
			Vertex v = to->getVertexById(vid);
			EXEC_COUNT current_index = symbolic_execution[vid];
			ARRAY_INDEX current_start_index = current_index ;
			TIME_UNIT current_start_time = persched[vid].second[current_start_index % persched[vid].second.size()]  + (current_start_index / persched[vid].second.size() ) * persched[vid].first;

			VERBOSE_DEBUG("    - next_time = " << next_time);
			VERBOSE_DEBUG("    - current_index = " << current_index);
			VERBOSE_DEBUG("    - current_start_time = " << current_start_time);
			VERBOSE_DEBUG("    - objective[vid] = " << objective[vid]);

			VERBOSE_ASSERT(last_time <= current_start_time , "The task " << vid << " should have been executed already.");

			if (current_start_time <= next_time) {
				next_time = current_start_time;
				next_id = vid;
			}

		}

		VERBOSE_DEBUG("  - next_id = " << next_id << " and next_time = " << next_time);
		task_order.push_back(next_id);

		objective[next_id] = objective[next_id] - 1; // ending not proved
		symbolic_execution[next_id] ++ ;
		last_time = next_time;


		for (auto vid : mergeNodes) {
			if (objective[vid] > 0) {
				VERBOSE_DEBUG("    - need_to_execute " << vid);
				need_to_execute = true; // make sure to continue executing if we did not reach objectives
			}
		}
	}


	VERBOSE_INFO("  - Task order is " << commons::toString(task_order));


	std::vector<ARRAY_INDEX> init_executions_vec;
	std::vector<ARRAY_INDEX> periodic_executions_vec;

	std::vector<TIME_UNIT> init_duration_vec;
	std::vector<TIME_UNIT> periodic_duration_vec;

	VERBOSE_INFO("  - Gather init timings and phases ");



	for (auto idx = 0 ; idx < (task_order.size() - periodic_executions_count) ; idx++) {
		ARRAY_INDEX tid = task_order[idx];
		VERBOSE_INFO("  - idx = " << idx << " tid = " << tid);
		Vertex t = to->getVertexById(tid);
		TIME_UNIT dur = to->getVertexDuration(t, idx + 1 - to->getInitPhasesQuantity(t));
		init_executions_vec.push_back(tid);
		init_duration_vec.push_back(dur);

		VERBOSE_INFO("  - init_duration_vec =  " << commons::toString(init_duration_vec));
	}

	VERBOSE_INFO("  - Gather periodic timings and phases ");

	for (auto idx = (task_order.size() - periodic_executions_count) ; idx < task_order.size() ; idx++) {
		ARRAY_INDEX tid = task_order[idx];
		VERBOSE_INFO("  - idx = " << idx << " tid = " << tid);
		Vertex t = to->getVertexById(tid);
		TIME_UNIT dur = to->getVertexDuration(t, idx + 1 - to->getInitPhasesQuantity(t));
		periodic_executions_vec.push_back(tid);
		periodic_duration_vec.push_back(dur);
		VERBOSE_INFO("  - periodic_duration_vec =  " << commons::toString(init_duration_vec));
	}


	VERBOSE_INFO(" - Merged task has " << init_executions_vec.size() << " init phases and " << periodic_executions_vec.size() <<  " periodic phases.");


	VERBOSE_INFO("Delete original tasks");

	to->reset_computation();
	for(ARRAY_INDEX vid :mergeNodes)
	{
		Vertex cfgVtx = to->getVertexById(vid);
		to->removeVertex(cfgVtx);
	}



	VERBOSE_INFO("Add the merge task");

	auto middle = to->addVertex();
	to->setVertexName(middle, name);
	to->setReentrancyFactor(middle, 1); // This is the reentrancy, it avoid a task to be executed more than once at the same time.
	to->setInitPhasesQuantity(middle, init_executions_vec.size());
	to->setPhasesQuantity(middle, periodic_executions_vec.size());
	to->setVertexDuration(middle,periodic_duration_vec);
	to->setVertexInitDuration(middle,  init_duration_vec );


	VERBOSE_INFO("Add the input edges");

	for (auto task_it : input_edges ) {

		ARRAY_INDEX old_tid = task_it.first;

		VERBOSE_DEBUG("  - old_tid " << old_tid);

		for (auto infos : task_it.second ) {

			VERBOSE_DEBUG("   - input edge from " << infos.src_id
					<< " with  infos.out_ini_vector = " << commons::toString(infos.out_ini_vector)
			<< " and  infos.out_per_vector = " << commons::toString(infos.out_per_vector) );

			auto new_edge = to->addEdge(to->getVertexById(infos.src_id), middle);
			to->setEdgeName(new_edge, "merged_" + commons::toString(to->getEdgeId(new_edge)));
			to->setEdgeType(new_edge,infos.type);
			to->setPreload(new_edge, infos.preload);
			to->setEdgeInPhases(new_edge, infos.in_per_vector);
			to->setEdgeInInitPhases(new_edge, infos.in_ini_vector);

			std::vector<TOKEN_UNIT> init_out (init_executions_vec.size(), 0) ;
			std::vector<TOKEN_UNIT> per_out (periodic_executions_vec.size(), 0) ;

			ARRAY_INDEX task_to_match = infos.dst_id ;
			for (size_t idx = 0, state = 0 ; idx < (init_executions_vec.size() +  periodic_executions_vec.size()); idx ++) {

				VERBOSE_DEBUG("    - check idx " << idx << " current state " << state );

				ARRAY_INDEX task_id = idx < init_executions_vec.size() ? init_executions_vec[idx] : periodic_executions_vec[idx - init_executions_vec.size() ];
				VERBOSE_DEBUG("     - task id is " << task_id << " looking for " << task_to_match );
				if (task_id == task_to_match) {

					VERBOSE_DEBUG("      - task " << task_id << " is good " );
					if (idx < init_executions_vec.size()) {
						VERBOSE_DEBUG("        Update init " );
						init_out[idx] =  (state < infos.out_ini_vector.size()) ? infos.out_ini_vector[state] : infos.out_per_vector[(state -  infos.out_ini_vector.size()) % infos.out_per_vector.size()] ;
					} else {
						VERBOSE_DEBUG("        Update periodic " );
						per_out[idx -  init_executions_vec.size()] =  (state < infos.out_ini_vector.size()) ? infos.out_ini_vector[state] : infos.out_per_vector[(state -  infos.out_ini_vector.size()) % infos.out_per_vector.size()] ;
					}
					state++;
				}
			}
			VERBOSE_DEBUG("    - init_out = " << commons::toString(init_out));
			VERBOSE_DEBUG("    - per_out = " << commons::toString(per_out));

			to->setEdgeOutPhases(new_edge, per_out);
			to->setEdgeOutInitPhases(new_edge, init_out);
		}

	}

	VERBOSE_INFO("Add the output edges");

	for (auto task_it : output_edges ) {

		ARRAY_INDEX old_tid = task_it.first;

		VERBOSE_DEBUG("  - old_tid " << old_tid);

		for (auto infos : task_it.second ) {

			VERBOSE_DEBUG("   - output edge to " << infos.dst_id
					<< " with  infos.in_ini_vector = " << commons::toString( infos.in_ini_vector)
			<< " and  infos.in_per_vector = " << commons::toString( infos.in_per_vector ));

			auto new_edge = to->addEdge( middle, to->getVertexById(infos.dst_id));
			to->setEdgeName(new_edge, "merged_" + commons::toString(to->getEdgeId(new_edge)));
			to->setEdgeType(new_edge,infos.type);
			to->setPreload(new_edge, infos.preload);

			to->setEdgeOutPhases(new_edge, infos.out_per_vector);
			to->setEdgeOutInitPhases(new_edge, infos.out_ini_vector);


			std::vector<TOKEN_UNIT> init_in (init_executions_vec.size(), 0) ;
			std::vector<TOKEN_UNIT> per_in (periodic_executions_vec.size(), 0) ;

			ARRAY_INDEX task_to_match = infos.src_id ;
			for (size_t idx = 0, state = 0 ; idx < (init_executions_vec.size() +  periodic_executions_vec.size()); idx ++) {

				VERBOSE_DEBUG("    - check idx " << idx << " current state " << state );

				ARRAY_INDEX task_id = idx < init_executions_vec.size() ? init_executions_vec[idx] : periodic_executions_vec[idx - init_executions_vec.size() ];
				VERBOSE_DEBUG("     - task id is " << task_id << " looking for " << task_to_match );
				if (task_id == task_to_match) {

					VERBOSE_DEBUG("      - task " << task_id << " is good " );
					if (idx < init_executions_vec.size()) {
						VERBOSE_DEBUG("        Update init " );
						init_in[idx] =  (state < infos.in_ini_vector.size()) ? infos.in_ini_vector[state] : infos.in_per_vector[(state -  infos.in_ini_vector.size()) % infos.in_per_vector.size()] ;
					} else {
						VERBOSE_DEBUG("        Update periodic " );
						per_in[idx -  init_executions_vec.size()] =  (state < infos.in_ini_vector.size()) ? infos.in_ini_vector[state] : infos.in_per_vector[(state -  infos.in_ini_vector.size()) % infos.in_per_vector.size()] ;
					}
					state++;
				}
			}
			VERBOSE_DEBUG("    - init_in = " << commons::toString(init_in));
			VERBOSE_DEBUG("    - per_in = " << commons::toString(per_in));





			to->setEdgeInPhases(new_edge, per_in);
			to->setEdgeInInitPhases(new_edge, init_in);
		}

	}


	return true;



}





void algorithms::transformation::merge_tasks    (models::Dataflow* const dataflow, parameters_list_t  parameters  ) {

	// Check arguments

	if (parameters.find("tasks") == parameters.end() ) {
		VERBOSE_ERROR("Please specify the 'tasks' argument.");
		return;
	}

	if (parameters.find("name") == parameters.end() ) {
		VERBOSE_ERROR("New vertex 'name' argument has not been specified.");
		return;
	}

	// Prepare parameters
	std::string new_name = parameters["name"];
	std::vector<ARRAY_INDEX> task_ids = commons::split<ARRAY_INDEX>(parameters["tasks"], ',');


	VERBOSE_ASSERT(algorithms::transformation::mergeCSDFFromKperiodicSchedule(dataflow,new_name, task_ids), "mergeCSDFFromKperiodicSchedule return error.");

}
