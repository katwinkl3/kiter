/*
 * BufferlessNoCScheduling.cpp
 *
 *  Created on: Apr 21, 2020
 *      Author: toky
 */


#include <models/NoC.h>
#include <vector>
#include <iostream>
#include <printers/stdout.h>
#include <commons/verbose.h>
#include <commons/commons.h>
#include <models/Dataflow.h>
#include <models/EventGraph.h>
#include <algorithms/normalization.h>
#include <algorithms/software_noc.h>
#include <algorithms/repetition_vector.h>
#include <algorithms/kperiodic.h>
#include <algorithms/mappings.h>
#include <cstdlib>
#include <stack>
#include <climits>
#include <algorithms/schedulings.h>
#include <set>
#include <queue>
#include <tuple>
#include <commons/SDF3Wrapper.h>
#include <unordered_map>

static TIME_UNIT NULL_DURATION = 0;

typedef std::pair<ARRAY_INDEX, ARRAY_INDEX>  mypair;
typedef std::tuple<ARRAY_INDEX, ARRAY_INDEX, ARRAY_INDEX> mytuple;

static void print_graph (models::Dataflow * to, std::string suffix = "none") {

	static int counter = 0;
	counter ++ ;
	VERBOSE_INFO("=========== Write file " << counter << "\n");

	std::string sfilename = "bufferless_noc_schedule_"+ commons::toString(counter) + "_" + suffix + "";



	{

		std::string filename = sfilename + "_noc";
		std::ofstream myfile;
		myfile.open (filename  + ".dot");
		myfile << printers::GenerateNoCDOT(to);
		myfile.close();

		std::string cmd = "dot " + filename + ".dot -Gdpi=300  -Kneato -T png -o " + filename + ".png";
		auto out_err = system(cmd.c_str());
		if(out_err) {
			VERBOSE_INFO ("System call returns error\n");
		}

	}

	commons::writeSDF3File(sfilename+ ".xml", to);
	to->reset_computation();

}

static LARGE_INT gcdExtended(LARGE_INT x, LARGE_INT y, LARGE_INT *a, LARGE_INT *b)
{
	if (x == 0) //Base Case
	{
		*a = 0;
		*b = 1;
		return y;
	}

	LARGE_INT a1, b1; // To store results of recursive call
	LARGE_INT gcd = gcdExtended(y%x, x, &a1, &b1);
	// Update x and y using results of recursive  call
	*a = b1 - (y/x) * a1;
	*b = a1;

	return gcd;
}

//Function to remove all the edges that connect a particular vertex
static void removeAllEdgesVertex(models::Dataflow* d, Vertex vtx)
{
	{ForInputEdges(d, vtx, E)	{
		d->removeEdge(E);
	}}
	{ForOutputEdges(d, vtx, E)	{
		d->removeEdge(E);
	}}
	// d->removeVertex(vtx);
}

static bool resolveSrcConflicts(models::Dataflow* d, Vertex src, std::vector<ARRAY_INDEX> & origV)
{

	VERBOSE_INFO ( "resolveSrcConflicts for "  << d->getVertexName(src) << " with original number of vertices " << commons::toString(origV.size()));

	std::vector<Edge> srcedges, rtredges;
	TOKEN_UNIT in_flow = 0;
	TOKEN_UNIT preload = 0;
	LARGE_INT mygcd = 0, a, b;


	//1. Find all the edges from the vertex to router
	// and router to next edge in the NoC
	{ForOutputEdges(d,src,outE)	{
		auto next_node = d->getEdgeTarget(outE);

		if( !commons::findInVector (origV, d->getVertexId(next_node)) && next_node!= src)
		{
			srcedges.push_back(outE);
			auto edgein = d->getEdgeIn(outE);
			in_flow += edgein;
			preload += d->getPreload(outE);
			VERBOSE_INFO ( "states=" << in_flow << ",preload=" << preload );
			if(mygcd == 0)
				mygcd = edgein;
			else
				mygcd = gcdExtended(edgein, mygcd, &a, &b);

			{ForOutputEdges(d,next_node,inE)     {
				if(next_node != d->getEdgeTarget(inE))
					rtredges.push_back(inE);
			}}
		}
		else if( commons::findInVector (origV, d->getVertexId(next_node)))
		{
			std::cout << "src=" << d->getVertexName(src) << " still has outgoing edge to " << d->getVertexName(next_node) << "\n";
			exit(0);
		}
	}}


	//1B. Do some sanity check here
	VERBOSE_ASSERT(srcedges.size() == rtredges.size(),  "BIG ERROR in resolveSrcConflicts\n");

	if(srcedges.size() <= 1) {
		//std::cout << "no resolveSrcConflicts for vertex " << d->getVertexId(src) << "\n";
		return false;
	}

	VERBOSE_ASSERT(mygcd > 0 , "Division by zero");
	in_flow = in_flow / mygcd;

	//1C. Initialize token vector
	std::vector<TOKEN_UNIT> temp;
	std::vector< std::vector<TOKEN_UNIT> > token_vec (srcedges.size(), temp);

	//2. calculate the preload and flow values of all outgoing edges from source vertices
	//Remove the edges between source and router.
	//Remove the router nodes
	for(int i = 0; i < (int)srcedges.size(); i++)
	{
		for(int j = 0; j < (int)srcedges.size(); j++)
		{
			for(int vi = 0; vi < (int)d->getEdgeIn(srcedges[i])/mygcd; vi++)
			{
				if (i == j)
					token_vec[j].push_back(1);
				else
					token_vec[j].push_back(0);
			}
		}
	}
	VERBOSE_INFO ( "done with removing edges");
	VERBOSE_ASSERT(in_flow > 0, "This case is not supported: flow between task has to be strictly positive");

	//2B. Create the phase duration and token per phase for the new router node
	std::vector<TOKEN_UNIT> srctoken(in_flow, 1);
	std::vector<TIME_UNIT> phaseDurVec(in_flow, 1.0);

	//3. Create the BIG router
	auto middle = d->addVertex();
	std::stringstream ss;
	ss << d->getVertexId(src) << "-S";
	d->setVertexName(middle, ss.str());
	d->setPhasesQuantity(middle, in_flow); // number of state for the actor, only one in SDF
	d->setVertexDuration(middle, phaseDurVec); // is specify for every state , only one for SDF.
	d->setReentrancyFactor(middle, 1); // This is the reentrancy, it avoid a task to be executed more than once at the same time.
	VERBOSE_INFO ( "SRC_CONF(ID="<< d->getVertexId(src) << "):Done creating big router node  " << d->getVertexId(middle) );

	//3B. Add edge between source vertex and big router node
	auto srcEdge = d->addEdge(src, middle);
	d->setPreload(srcEdge, preload);
	d->setEdgeInPhases(srcEdge, {in_flow*mygcd});
	d->setEdgeOutPhases(srcEdge, srctoken);


	//4. Setup the new edges. First remove the edge between router and the next NoC link
	//Setup the flow appropriately
	//Remove the router vertex and the edge between routet to the next NoC link
	for(int i = 0; i < (int)rtredges.size(); i++)
	{
		//(v1)  rtredges[i]  v2
		auto v1 = d->getEdgeSource(rtredges[i]);
		auto v2 = d->getEdgeTarget(rtredges[i]);
		auto loc_preload = d->getPreload(rtredges[i]);

		//std::cout << "src=" << d->getVertexName(src) << ",rtr_new=" << d->getVertexName(middle) << ",v2=" << d->getVertexName(v2) << ",deleting=" << d->getVertexName(v1) << "\n";
		removeAllEdgesVertex(d, v1);

		auto new_edge = d->addEdge(middle, v2);
		d->setEdgeType(new_edge,EDGE_TYPE::BUFFERLESS_EDGE);
		d->setEdgeInPhases(new_edge, token_vec[i]);
		d->setEdgeOutPhases(new_edge, {1});
		d->setPreload(new_edge,loc_preload);  // preload is M0

	}
	return true;
}

static void removeOrphanNodes(models::Dataflow* to)
{
	for (bool removeme = true; removeme; ) {
		{ForEachVertex(to,v) {
			if (to->getVertexDegree(v) == 0) {
				ARRAY_INDEX vid = to->getVertexId(v);
				std::string vname = to->getVertexName(v);
				VERBOSE_INFO ( " I remove one task (" << vid << ",name=" << vname << ") lah!");
				to->removeVertex(v);
				removeme=true;
				break;
			}
			removeme=false;
		}}
	}
}

static void getPrevAndNextVertex(ARRAY_INDEX vtx_id, models::Dataflow* to, ARRAY_INDEX& prev, ARRAY_INDEX& next)
{
	auto vtx = to->getVertexById(vtx_id);
	int count = 0;
	std::cout << "vname=" << to->getVertexName(vtx) << "\n===========================\n";
	{ForInputEdges(to,vtx,inE)	{
		prev = to->getVertexId( to->getEdgeSource(inE) );
		std::cout << "source=" << to->getVertexName( to->getEdgeSource(inE) ) << "\n";
		std::cout << "target=" << to->getVertexName( to->getEdgeTarget(inE) ) << "\n";
		count ++;
	}}
	if(count > 1)
	{
		std::cout << "incoming edges more than 1\n"; exit(0);
	}
	std::cout << "===========================\n";
	count = 0;
	{ForOutputEdges(to,vtx,outE)	{
		next = to->getVertexId( to->getEdgeTarget(outE) );
		std::cout << "source=" << to->getVertexName( to->getEdgeSource(outE) ) << "\n";
		std::cout << "target=" << to->getVertexName( to->getEdgeTarget(outE) ) << "\n";
		count++;
	}}
	if(count > 1)
	{
		std::cout << "incoming edges more than 1\n"; exit(0);
	}
	std::cout << "===========================\n\n\n";
}

static std::vector<ARRAY_INDEX> checkForConflicts(models::Dataflow* to, TIME_UNIT HP, scheduling_t& persched, models::Dataflow* original, std::string& conflictname) {
	//std::vector< mytuple > mergeNodes;
	std::vector< ARRAY_INDEX > mergeNodes;
	mytuple tuple_temp;
	ARRAY_INDEX prev, next;

	VERBOSE_ASSERT(computeRepetitionVector(to),"inconsistent graph");
	VERBOSE_ASSERT(computeRepetitionVector(original),"inconsistent graph");
	int total_conflict = 0;



	std::unordered_map< edge_id_t, std::vector<ARRAY_INDEX> > links_usage;

	for (auto v : to->vertices()) {
			ARRAY_INDEX tid = to->getVertexId(v);
			auto current_mapping =  (to->getMapping(v));
			VERBOSE_ASSERT(current_mapping >= 0, "UNSUPPORTED CASE, EVERY TASK NEED TO BE MAPPED, Task " << to->getVertexName(v) << " mapped to " << current_mapping);
			if (to->getNoC().hasEdge(current_mapping)) {
				links_usage[current_mapping].push_back(tid);
			}
		}




	for (auto  key : links_usage) {
		auto edge_id = key.first;
		auto mysize = links_usage[edge_id].size();
		if(mysize <= 1)
			continue;
		for(unsigned int i = 0; i < mysize; i++)
		{
			for(unsigned int j = 0; j < mysize; j++)
			{
				if (i <= j) //ignore the upper triangle
					continue;
				auto taski = links_usage[edge_id][i];
				auto taskj = links_usage[edge_id][j];

				std::cout << "problem in " << edge_id << ",vid=" << links_usage[edge_id][i] << ",vid2=" << links_usage[edge_id][j] << "\n";

				auto taski_vtx = to->getVertexById(taski);
				auto taskj_vtx = to->getVertexById(taskj);

				auto ni = to->getNi(taski_vtx);
				auto nj = to->getNi(taskj_vtx);

				//check if any of the starting times match
				std::vector<TIME_UNIT> si_vec, sj_vec;
				for (auto  skey : persched[taski].second) {
					si_vec.push_back(skey);
				}
				for (auto  skey : persched[taskj].second) {
					sj_vec.push_back(skey);
				}

				for(unsigned int si_i = 0; si_i < si_vec.size(); si_i++)
				{
					for(unsigned int sj_i = 0; sj_i < sj_vec.size(); sj_i++)
					{
						auto si =  persched[taski].second[si_i];
						auto sj =  persched[taskj].second[sj_i];

						if( algorithms::isConflictPresent((LARGE_INT) HP, si, (LARGE_INT) ni, sj, (LARGE_INT) nj) )
						{
							VERBOSE_INFO ("confl: " << to->getVertexName(taski_vtx) << " and " << to->getVertexName(taskj_vtx) << "\n");
							conflictname = to->getVertexName(taski_vtx) + "_" + to->getVertexName(taskj_vtx);
							get<0>(tuple_temp) = taski;
							getPrevAndNextVertex(taski, to, prev, next);
							get<1>(tuple_temp) = prev;
							get<2>(tuple_temp) = next;
							std::cout << prev << "-> " << taski << "->" << next << "\n";
							//mergeNodes.push_back(tuple_temp);
							mergeNodes.push_back(taski);


							get<0>(tuple_temp) = taskj;
							getPrevAndNextVertex(taskj, to, prev, next);

							if(prev == get<1>(tuple_temp))
							{
								std::cout << "avoiding conflicts\n";
								mergeNodes.clear();
								continue;
							}

							else
							{
								total_conflict += 1;
								get<1>(tuple_temp) = prev;
								get<2>(tuple_temp) = next;
								//mergeNodes.push_back(tuple_temp);
								mergeNodes.push_back(taskj);
								std::cout << prev << "-> " << taskj << "->" << next << "\n";
								return mergeNodes;
							}
						}
					}
				}
			}
		}
	}

	models::Dataflow* temp = new models::Dataflow(*original);
	VERBOSE_ASSERT(computeRepetitionVector(temp),"inconsistent graph");
	EXEC_COUNT orig_ni = 0, mod_ni = 0;
	{ForEachVertex(to,v) {
		if(to->getVertexId(v) == 1)
		{
			mod_ni = to->getNi(v);
			break;
		}
	}}
	{ForEachVertex(temp,v) {
		if(temp->getVertexId(v) == 1)
		{
			orig_ni = temp->getNi(v);
			break;
		}
	}}

	VERBOSE_INFO ( "total_conflict=" << total_conflict );
	VERBOSE_INFO ( "NEW HP=" << HP*orig_ni/mod_ni );
	return mergeNodes;
}


static void findHP(models::Dataflow* orig, models::Dataflow* to, scheduling_t& persched, TIME_UNIT* HP, EXEC_COUNT* LCM)
{
	//fins ratio between orig and new graph for the nodes
	float ratio = -1;
	std::map<ARRAY_INDEX, float> ratio_arr;
	std::map<ARRAY_INDEX, TIME_UNIT> delays;
	VERBOSE_ASSERT(computeRepetitionVector(orig),"inconsistent graph");
	VERBOSE_ASSERT(computeRepetitionVector(to),"inconsistent graph");
	{ForEachVertex(orig,orig_v) {
		auto orig_vid = orig->getVertexId(orig_v);
		auto to_v = to->getVertexById(orig_vid);
		float new_ratio = (float) to->getNi(to_v) / (float)orig->getNi(orig_v);
		if(ratio != -1 && ratio != new_ratio)
			std::cout << "NODE " << to->getVertexName(to_v) << ",ni=" <<  to->getNi(to_v) << " failed\n";
		ratio = new_ratio;
		ratio_arr[orig_vid] = new_ratio;


		// Here I need to compute the Ni-th execution of a task
		auto Ni = orig->getNi(orig_v);
		auto periodic_start_count = persched[orig_vid].second.size();
		VERBOSE_ASSERT(Ni % periodic_start_count == 0, "FIXME");
		VERBOSE_ASSERT(orig->getPhasesQuantity(orig_v) == 1, "FIXME");



		// I take Ni and I need to know which start correspond to this Ni-th
		// auto start_index = Ni % periodic_start_count
		auto start_index = periodic_start_count - 1;
		auto end_of_last_start  = persched[orig_vid].second[start_index] * (Ni / periodic_start_count) + orig->getVertexDuration(orig_v);

		delays[orig_vid] = end_of_last_start;


	}}

	*HP = 0.0;
	*LCM = 1;
	for (auto  key : persched) {
		auto task = key.first;
		auto task_vtx = to->getVertexById(key.first);

		float new_ratio = ratio_arr[key.first];

		std::stringstream ss;
		for(int i = 0; i < (int)persched[task].second.size(); i++)
			ss << std::setprecision(13) << persched[task].second[i] << " ";

		*HP =    ( persched[task].first * to->getNi(task_vtx) ) / (new_ratio * ((float)persched[task].second.size())) ;
		auto delay = delays[key.first];
		VERBOSE_INFO ( "Task " <<  to->getVertexName(task_vtx) <<  " : "
				<< " duration=[ " << commons::toString(to->getVertexPhaseDuration(task_vtx)) <<  "]"
				<< " period=" <<  persched[task].first << ""
				<< " HP=" << *HP << ""
				<< " Delay=" << delay << ""
				<< " Ni=" << to->getNi(task_vtx) << ""
				<< " starts=[ " << ss.str() << "]");
		*LCM = boost::integer::lcm(*LCM, to->getNi(task_vtx));

	}
	*LCM = boost::integer::lcm(*LCM, (EXEC_COUNT)*HP);

}

bool mergeConfigNodesInit(models::Dataflow* to, std::string name, std::vector< ARRAY_INDEX >& mergeNodes) {

	VERBOSE_INFO("name = " << name << " nodes: " << commons::toString( mergeNodes ) );
	// This function is not generic yet, cannot support preload
	if (mergeNodes.size() <= 1)
		return false;

	// step one we schedule

	to->reset_computation();
	VERBOSE_ASSERT(computeRepetitionVector(to),"inconsistent graph");
	//std::cout << "calling csdf\n";
	models::Scheduling scheduling_res = algorithms::scheduling::CSDF_KPeriodicScheduling(to);
	TIME_UNIT omega = scheduling_res.getGraphPeriod();
	auto persched = scheduling_res.getTaskSchedule();
	VERBOSE_INFO("omega = " << omega );

	//std::cout << "done done\n";
	VERBOSE_ASSERT(omega != std::numeric_limits<TIME_UNIT>::infinity(), "Infinite period, this dataflow does not schedule, thus I cannot merge anymore.");

	TIME_UNIT mergeVtxDuration = NULL_DURATION;

	// step two, we find the maximum time to reach by init phases
	TIME_UNIT max_start = 0;
	for (auto vid : mergeNodes) {
		max_start = std::max(max_start, persched[vid].second[0]);
	}

	//Find GCD value
	EXEC_COUNT gcd_value = 0;
	node_id_t router_id = -1;
	for(ARRAY_INDEX vid :mergeNodes)
	{
		Vertex vi     = to->getVertexById(vid);
		mergeVtxDuration = std::max(mergeVtxDuration, to->getVertexDuration(vi));
		EXEC_COUNT ni = to->getNi(vi);
		gcd_value = boost::integer::gcd (gcd_value , ni) ;
		if (router_id == -1) router_id = to->getMapping(vi);
		VERBOSE_ASSERT(router_id == to->getMapping(vi), "Merging tasks mapped to different components");
	}




	//step two , we produce the init phases for each task
	std::vector < std::pair<TIME_UNIT , ARRAY_INDEX> > init_start_times ;
	std::vector < std::pair<TIME_UNIT , ARRAY_INDEX> > periodic_start_times ;
	for (auto vid : mergeNodes) {
		Vertex vi     = to->getVertexById(vid);
		EXEC_COUNT ni = to->getNi(vi);
		VERBOSE_INFO("For task " << vid << " period is " << persched[vid].first << " and starts " << commons::toString(persched[vid].second));
		ARRAY_INDEX current_start_index = 0;
		TIME_UNIT current_start_time = persched[vid].second[0];

		while (current_start_time < max_start) {
			init_start_times.push_back(std::make_pair(current_start_time , vid));
			current_start_index++;
			current_start_time = persched[vid].second[current_start_index % persched[vid].second.size()]  + (current_start_index / persched[vid].second.size() ) * persched[vid].first;
		}

		for (EXEC_COUNT i = 0 ; i < (ni/gcd_value) ; i++) {
			periodic_start_times.push_back(std::make_pair(current_start_time , vid));
			current_start_index++;
			current_start_time = persched[vid].second[current_start_index % persched[vid].second.size()]  + (current_start_index / persched[vid].second.size() ) * persched[vid].first;
		}

	}

	VERBOSE_INFO("Sorting" );
	std::sort(init_start_times.begin(), init_start_times.end());
	std::sort(periodic_start_times.begin(), periodic_start_times.end());


	std::vector<TIME_UNIT> phaseDurVec(periodic_start_times.size(), mergeVtxDuration); //Create the phase duration
	std::vector<TIME_UNIT> InitPhaseDurVec(init_start_times.size(), mergeVtxDuration); //Create the phase duration
	//std::vector<TIME_UNIT> phaseDurVec(periodic_start_times.size(), NULL_DURATION); //Create the phase duration
	//std::vector<TIME_UNIT> InitPhaseDurVec(init_start_times.size(), NULL_DURATION); //Create the phase duration



	to->reset_computation();


	VERBOSE_INFO("Create router" );
	//3. Create the BIG router
	auto middle = to->addVertex();
	to->setMapping(middle,router_id);
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
		periodic_token_vec [periodic_start_times[i].second][i] = 1;
	}

	for (unsigned int i = 0 ; i < init_start_times.size() ; i++) {
		init_token_vec [init_start_times[i].second][i] = 1;
	}


	VERBOSE_INFO("Remove the edges" );

	//4. Remove the edges before/after the config node. Setup the flow. Remove the router nodes.

	for(ARRAY_INDEX vid :mergeNodes)
	{

		VERBOSE_INFO("Edge " << vid );
		Vertex cfgVtx = to->getVertexById(vid);
		Vertex v1;
		Vertex v2;
		std::vector<TOKEN_UNIT> myin, myout;
		std::vector<TOKEN_UNIT> myiin, myiout;
		TOKEN_UNIT preload_v1, preload_v2;
		EDGE_TYPE e_v1, e_v2;


		//getEdgesPhaseVector(to, v1, cfgVtx, v2, myin, myout, preload_v1, preload_v2, e_v1, e_v2);
		// void getEdgesPhaseVector(models::Dataflow* d, Vertex& v1, Vertex cfg, Vertex& v2, std::vector<TOKEN_UNIT>& in, std::vector<TOKEN_UNIT>& out, TOKEN_UNIT& preload_v1, TOKEN_UNIT& preload_v2, EDGE_TYPE& e_v1, EDGE_TYPE& e_v2)



		{ForInputEdges(to, cfgVtx, E)	{
			myin = to->getEdgeInVector(E);
			myiin = to->getEdgeInitInVector(E);
			v1 = to->getEdgeSource(E);
			preload_v1 =  to->getPreload(E);
			e_v1 = to->getEdgeType(E);
			to->removeEdge(E);
			break;
		}}
		{ForOutputEdges(to, cfgVtx, E)	{
			myout = to->getEdgeOutVector(E);
			myiout = to->getEdgeInitOutVector(E);
			v2 = to->getEdgeTarget(E);
			preload_v2 =  to->getPreload(E);
			e_v2 = to->getEdgeType(E);
			to->removeEdge(E);
			break;
		}}

		auto new_edge = to->addEdge(v1, middle);
		to->setEdgeType(new_edge,EDGE_TYPE::BUFFERLESS_EDGE);
		to->setPreload(new_edge, preload_v1);
		to->setEdgeType(new_edge, e_v1);

		to->setEdgeInInitPhases(new_edge, myiin);
		to->setEdgeInPhases(new_edge, myin);
		to->setEdgeOutPhases(new_edge, periodic_token_vec[vid]);
		to->setEdgeOutInitPhases(new_edge, init_token_vec[vid]);
		VERBOSE_ASSERT((EXEC_COUNT)init_token_vec[vid].size() == to->getInitPhasesQuantity(middle), "init_token_vec[vid] has the wrong size: " << init_token_vec[vid].size() << " !=" << to->getInitPhasesQuantity(middle));

		new_edge = to->addEdge(middle, v2);
		to->setEdgeType(new_edge,EDGE_TYPE::BUFFERLESS_EDGE);
		to->setPreload(new_edge, preload_v2);
		to->setEdgeInPhases(new_edge, periodic_token_vec[vid]);
		to->setEdgeInInitPhases(new_edge, init_token_vec[vid]);
		VERBOSE_ASSERT((EXEC_COUNT)init_token_vec[vid].size() == to->getInitPhasesQuantity(middle), "init_token_vec[vid] has the wrong size: " << init_token_vec[vid].size() << " !=" << to->getInitPhasesQuantity(middle));
		//VERBOSE_ASSERT(to->getInitPhasesQuantity(v2) == 0, "getInitPhasesQuantity(v2) has the wrong size: " <<  to->getInitPhasesQuantity(v2));

		to->setEdgeOutPhases(new_edge, myout);
		to->setEdgeOutInitPhases(new_edge, myiout);
		to->setEdgeType(new_edge, e_v2);
	}


	VERBOSE_INFO("End of Remove the edges" );
	return true;



}

//print_graph(to, original_df);
void algorithms::BufferlessNoCScheduling(models::Dataflow* const  dataflow, parameters_list_t   ) {


	models::Dataflow* to = new models::Dataflow(*dataflow);

	print_graph(to, "start");

	/// Store a list of origina l actors id.
	std::vector <ARRAY_INDEX> original_vertex_ids;
	for (auto v : to->vertices()) {
		original_vertex_ids.push_back(to->getVertexId(v));
	}


	VERBOSE_INFO ("Task mapping = ");
	for (auto v : to->vertices()) {
		VERBOSE_INFO ("  - " << to->getVertexId(v) << " mapped to "  << to->getMapping(v));
	}


	algorithms::ModelNoCConflictFreeCommunication(to) ;


	print_graph(to, "modeledNoC");

	/// Store a list of origina l actors id.
	std::map <edge_id_t, std::vector <ARRAY_INDEX> > links_usage;
	std::map <node_id_t, std::vector <ARRAY_INDEX> > router_usage;
	std::map <std::pair<edge_id_t,edge_id_t>, std::vector <ARRAY_INDEX> > router_xbar_usage;

	for (auto v : to->vertices()) {
		ARRAY_INDEX tid = to->getVertexId(v);
		auto current_mapping =  (to->getMapping(v));
		VERBOSE_ASSERT(current_mapping >= 0, "UNSUPPORTED CASE, EVERY TASK NEED TO BE MAPPED");
		if (to->getNoC().hasEdge(current_mapping)) {
			links_usage[current_mapping].push_back(tid);
		} else if (to->getNoC().hasNode(current_mapping)) {
			if (to->getNoC().getNode(current_mapping).type == NetworkNodeType::Router) {
				router_usage[current_mapping].push_back(tid);

				VERBOSE_ASSERT(to->getVertexInDegree(v) == 1, "The NoC has bot been modelled has expected, every NoCRouter-task should have one input NoCEdge Task. The task " << to->getVertexName(v) << " has " << to->getVertexInDegree(v));
				VERBOSE_ASSERT(to->getVertexOutDegree(v) == 1, "The NoC has bot been modelled has expected, every NoCRouter-task should have one output NoCEdge Task. The task " << to->getVertexName(v) << " has " << to->getVertexOutDegree(v));

				Vertex inputEdgeTask = to->getEdgeSource(*(to->getInputEdges(v).first));
				Vertex outputEdgeTask = to->getEdgeTarget(*(to->getOutputEdges(v).first));

				edge_id_t input_edge = to->getMapping(inputEdgeTask);
				edge_id_t output_edge = to->getMapping(outputEdgeTask);

				VERBOSE_ASSERT(to->getNoC().hasEdge(to->getMapping(inputEdgeTask)), "The NoC has bot been modelled has expected, every  NoCEdge Task should be properly mapped");
				VERBOSE_ASSERT(to->getNoC().hasEdge(to->getMapping(outputEdgeTask)), "The NoC has bot been modelled has expected, every  NoCEdge Task should be properly mapped");

				router_xbar_usage[std::pair<edge_id_t,edge_id_t> (input_edge, output_edge) ].push_back(tid);

			}
		}
	}

	VERBOSE_INFO ("Router_usage = ");
	for (auto item : router_usage) {

		VERBOSE_INFO ("  - " << item.first << ":"  << item.second);
	}
	VERBOSE_INFO ("Router_Xbar usage = ");
	for (auto item : router_xbar_usage) {

		VERBOSE_INFO ("  - " << item.first.first << "-" << item.first.second << ":"  << item.second);
	}
	VERBOSE_INFO ("links_usage = ");
	for (auto item : links_usage) {

		VERBOSE_INFO ("  - " << item.first << ":"  << item.second);
	}


	// ###### At this pint, we have a SDFG with artefacts for network.
	VERBOSE_INFO ("adding path nodes done.");



	//resolve cnflicts for all the  (a) sources that sent data to multiple nodes.
	//				(b) destinations that receive data from multiple nodes.
	//				(c) nodes that correspond to the same coniguration
	//Use [1, origV] as it denotes the list of nodes in the original SDF


	// ###### First source and destination merge
	// ############################################
	for (ARRAY_INDEX vid : original_vertex_ids) {
		Vertex src = to->getVertexById(vid);
		VERBOSE_INFO ("Run resolveSrcConflicts for task " << to->getVertexName(src));
		resolveSrcConflicts(to, src, original_vertex_ids);

		print_graph(to, "resolveSrcConflicts");
	}
	std::cout << "done sourceconflict\n";

	/*
	{
		to->reset_computation();
		VERBOSE_ASSERT(computeRepetitionVector(to),"inconsistent graph");
		models::Scheduling scheduling_res = algorithms::scheduling::CSDF_KPeriodicScheduling(to);
		TIME_UNIT omega = scheduling_res.getGraphPeriod();
		auto persched = scheduling_res.getTaskSchedule();

		VERBOSE_ASSERT(omega != std::numeric_limits<TIME_UNIT>::infinity(), "Infinite period, this dataflow does not schedule anymoe, resolveSrcConflicts failed.");
		to->reset_computation();
	}
	 */

	VERBOSE_INFO ( "done source conflict resolve" );

	//print_graph(to);
#ifdef WITH_DEST_CONFLICT
	for (auto vid : original_vertex_ids)
	{
		auto dest = to->getVertexById(vid);
		//resolveDestConflicts(to, dest, original_vertex_ids);

		//Change start
		std::vector<ARRAY_INDEX> mergeNodes;
		//Find all the edges from the vertex to router (dstedges) and router to next edge in the NoC (rtredges)
		{ForInputEdges(to,dest,inE)	{
			auto prev_node = to->getEdgeSource(inE);
			if(!commons::findInVector (original_vertex_ids, to->getVertexId(prev_node)) && prev_node!= dest)
				mergeNodes.push_back( to->getVertexId(prev_node));
		}}
		std::stringstream ss;
		ss << vid << "-D";
		auto retval = mergeConfigNodesInit(to, ss.str(), mergeNodes);
		//Change end

		if(retval)
		{
			removeOrphanNodes(to, vid_to_conflict_map, conflictEdges);
			//print_graph(to);
		}
	}
	std::cout << "done destconflict\n";
	VERBOSE_INFO ( "done dest conflict resolve" ) ;
#endif

	// ###### Systematically merge configs because we can
	// ############################################

	//print_graph(to);

	VERBOSE_INFO ("Call mergeConfigNodes");
	int idx = 0;
	for(auto it : router_xbar_usage) {
		std::cout << "Working on merge " << idx++ << " over " << router_xbar_usage.size() << "\n";
		if (it.second.size() > 1)
			{mergeConfigNodesInit(to, "merged_router_" + commons::toString(it.first), it.second);}

		print_graph(to, "mergeConfigNodesInit");

	}
	VERBOSE_INFO ("mergeConfigNodes Done.");
	std::cout << "done merge conflict resolve\n";

	//Remove conflicts at source and destination router links as a big node has been created
	{ForEachChannel(dataflow,c) {
		auto route = dataflow->getRoute(c);
		auto start_id = route[0];


#ifdef WITH_DEST_CONFLICT
		auto mysize = routes[it.first].size() - 1;
		auto end_id = routes[it.first][mysize];
		auto cf_it2 = conflictEdges.find((unsigned int)end_id);
		if(cf_it2 != conflictEdges.end())
			conflictEdges.erase(cf_it2);
#endif
	}}

	VERBOSE_INFO("resolving conflicts done");
	//Original graph


	removeOrphanNodes(to);


	std::cout << "calling scheduling\n";


	while(true)
	{
		std::vector< ARRAY_INDEX > mergeNodes;
		//std::vector< mytuple > mergeNodes;
		std::string name;

		std::cout << "calling kperiodic\n";

		to->reset_computation();
		VERBOSE_ASSERT(computeRepetitionVector(to),"inconsistent graph");
		models::Scheduling scheduling_res = algorithms::scheduling::CSDF_KPeriodicScheduling(to);
		TIME_UNIT omega = scheduling_res.getGraphPeriod();
		auto persched = scheduling_res.getTaskSchedule();

		VERBOSE_ASSERT(omega != std::numeric_limits<TIME_UNIT>::infinity(), "Infinite period, this dataflow does not schedule.");

		mergeNodes = checkForConflicts(to, omega, persched, dataflow, name);


		if(mergeNodes.size() == 0)
			break;

		std::cout << "conflict in " << name << "\n";
		//mergeConfigNodes(to, name, mergeNodes);
		mergeConfigNodesInit(to, name, mergeNodes);

		std::cout << "before removeme\n";
		removeOrphanNodes(to);

		std::cout << "before compp\n";

		print_graph(to, "mergeConfigNodesInit");


	}

	std::cout << "out of loop\n";

	VERBOSE_ASSERT(computeRepetitionVector(to),"inconsistent graph");
	models::Scheduling scheduling_res = algorithms::scheduling::CSDF_KPeriodicScheduling(to);
	scheduling_t persched = scheduling_res.getTaskSchedule();
	VERBOSE_INFO("findHP");
	EXEC_COUNT LCM;
	TIME_UNIT HP;
	findHP(dataflow, to, persched, &HP, &LCM);


}




