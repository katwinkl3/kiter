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

bool algorithms::transformation::mergeCSDFFromSchedule(models::Dataflow* to, std::string name , const std::vector< ARRAY_INDEX >& mergeNodes, const models::Scheduling* scheduling_res) {

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
		EXEC_COUNT ni = to->getNi(vi);
		VERBOSE_INFO("For task " << vid << " period is " << std::fixed  << std::setprecision(2) << persched[vid].first << " and starts " << commons::toString(persched[vid].second));
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
		to->setEdgeType(new_edge,EDGE_TYPE::BUFFERLESS_EDGE);
		to->setPreload(new_edge, 0);

		to->setEdgeInPhases(new_edge, myin);
		to->setEdgeOutPhases(new_edge, periodic_token_vec[vid]);
		to->setEdgeOutInitPhases(new_edge, init_token_vec[vid]);

		new_edge = to->addEdge(middle, v2);
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


	  VERBOSE_ASSERT(mergeCSDFFromKperiodicSchedule(dataflow,new_name, task_ids), "mergeCSDFFromKperiodicSchedule return error.");

}
