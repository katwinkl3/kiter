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

//typedef std::map< unsigned int, std::vector< std::pair<Vertex, Vertex> > > conflictEtype;
//typedef std::map< unsigned int, std::vector< mypair > > conflictEtype;
typedef std::unordered_map< unsigned int, std::vector<ARRAY_INDEX> > conflictEtype;
typedef std::unordered_map< std::string, std::vector< ARRAY_INDEX > > conflictConfigs;
//typedef std::unordered_map< std::string, std::vector< mytuple > > conflictConfigs;
typedef std::unordered_map< ARRAY_INDEX, unsigned int> vid_to_nocEid;

//remove the current edge between nodes
//add intermediate nodes based on the path between them
static std::vector<Vertex> addPathNode(models::Dataflow* d, Edge c, route_t list, conflictEtype& returnValue, conflictConfigs& configs, vid_to_nocEid& vid_to_conflict_map, bool addConfigNode)
{
	d->reset_computation();
	std::vector<Vertex> new_vertices;
	// We store infos about edge to be deleted
	auto source_vtx = d->getEdgeSource(c);
	auto target_vtx = d->getEdgeTarget(c);

	//Find the core index
	auto source = d->getVertexId(source_vtx);
	auto target = d->getVertexId(target_vtx);

	//use the inrate and route of the edges ans use it when creating the edges
	auto inrate = d->getEdgeIn(c);
	auto outrate = d->getEdgeOut(c);
	auto preload = d->getPreload(c);  // preload is M0

	if (source == target) //ignore this case
		return new_vertices;

	bool flag = true;
	d->removeEdge(c); //we delete the edge


	std::string key_str = "";

	int list_idx = 0;
	//std::cout << "flow:" << source << "->" << target << ":";
	//for every link in the path, add a corresponding node

	VERBOSE_INFO("Model the NoC communication between " << source << " to " << target );
	VERBOSE_INFO("  Route : " << commons::toString(list));

	for (auto e : list) {
		// we create a new vertex "middle"
		auto middle = d->addVertex();
		new_vertices.push_back(middle);

		////PLLLEASE DONT CHANGE THE "mid-" value in the name"
		std::stringstream ss;
		ss << "mid-" << source << "-" << target << "_" << e;

		returnValue[(unsigned int)e].push_back(  d->getVertexId(middle)  );
		vid_to_conflict_map[ d->getVertexId(middle) ] = (unsigned int)e;

		d->setVertexName(middle,ss.str());

		d->setPhasesQuantity(middle,1); // number of state for the actor, only one in SDF
		d->setVertexDuration(middle,{1}); // is specify for every state , only one for SDF.
		d->setReentrancyFactor(middle,1); // This is the reentrancy, it avoid a task to be executed more than once at the same time.


		//keep track of the
		if(addConfigNode && key_str != "")
		{
			//get<2>(configs[key_str][configs[key_str].size()-1]) = d->getVertexId(middle);
			key_str = "";
		}

		// we create a new edge between source and middle,
		auto e1 = d->addEdge(source_vtx, middle);

		if(flag)
		{
			d->setEdgeInPhases(e1,{inrate});  // we specify the production rates for the buffer
			flag = false;
		}
		else
		{
			d->setEdgeInPhases(e1,{1});
			d->setEdgeType(e1,EDGE_TYPE::BUFFERLESS_EDGE);
		}

		d->setEdgeOutPhases(e1,{1}); // and the consumption rate (as many rates as states for the associated task)
		d->setPreload(e1,0);  // preload is M0

		source_vtx = middle;

		if(addConfigNode && list_idx <= (int)list.size() - 2)
		{
			std::stringstream config_key, config_name;
			config_key << list[list_idx] << "_" << list[list_idx+1];
			config_name << "cfg-" << source << "_" << target << "-" << list[list_idx] << "_" << list[list_idx+1];


			auto vtx = d->addVertex();
			mytuple tuple_temp;
			//get<0>(tuple_temp) = d->getVertexId(vtx);
			//get<1>(tuple_temp) = d->getVertexId(middle);
			//configs[config_key.str()].push_back(tuple_temp);
			configs[config_key.str()].push_back(d->getVertexId(vtx));


			key_str = config_key.str();


			d->setVertexName(vtx,config_name.str());
			d->setPhasesQuantity(vtx,1); // number of state for the actor, only one in SDF
			d->setVertexDuration(vtx,{NULL_DURATION}); // is specify for every state , only one for SDF.
			d->setReentrancyFactor(vtx,1); // This is the reentrancy, it avoid a task to be executed more than once at the same time.

			// we create a new edge between source and middle,
			auto e1 = d->addEdge(middle, vtx);

			d->setEdgeInPhases(e1,{1});
			d->setEdgeType(e1,EDGE_TYPE::BUFFERLESS_EDGE);
			d->setEdgeOutPhases(e1,{1}); // and the consumption rate (as many rates as states for the associated task)
			d->setPreload(e1,0);  // preload is 0

			source_vtx = vtx;

		}
		list_idx++;
	}
	//find the final edge
	auto e2 = d->addEdge(source_vtx, target_vtx);
	d->setEdgeOutPhases(e2,{outrate});
	d->setEdgeInPhases(e2,{1});
	d->setPreload(e2,preload);  // preload is M0
	return new_vertices;
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

static void removeOrphanNodes(models::Dataflow* to, vid_to_nocEid& vid_to_conflict_map, conflictEtype& conflictEdgeMap)
{
	for (bool removeme = true; removeme; ) {
		{ForEachVertex(to,v) {
			if (to->getVertexDegree(v) == 0) {
				ARRAY_INDEX vid = to->getVertexId(v);
				std::string vname = to->getVertexName(v);
				VERBOSE_INFO ( " I remove one task (" << vid << ",name=" << vname << ") lah!");
				to->removeVertex(v);

				if(vid_to_conflict_map.find(vid) != vid_to_conflict_map.end())
				{
					unsigned int eid = vid_to_conflict_map[vid];
					std::vector<ARRAY_INDEX>::iterator it = std::find(conflictEdgeMap[eid].begin(), conflictEdgeMap[eid].end(), vid);
					if(it != conflictEdgeMap[eid].end())
					{
						conflictEdgeMap[eid].erase(it);
						//std::cout << "erasing " << vname << "\n";
					}
				}
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

static std::vector<ARRAY_INDEX> checkForConflicts(conflictEtype& conflictEdges, models::Dataflow* to, TIME_UNIT HP, scheduling_t& persched, models::Dataflow* original, std::string& conflictname)
{
	//std::vector< mytuple > mergeNodes;
	std::vector< ARRAY_INDEX > mergeNodes;
	mytuple tuple_temp;
	ARRAY_INDEX prev, next;

	VERBOSE_ASSERT(computeRepetitionVector(to),"inconsistent graph");
	VERBOSE_ASSERT(computeRepetitionVector(original),"inconsistent graph");
	int total_conflict = 0;
	for (auto  key : conflictEdges) {
		auto edge_id = key.first;
		auto mysize = conflictEdges[edge_id].size();
		if(mysize <= 1)
			continue;
		for(unsigned int i = 0; i < mysize; i++)
		{
			for(unsigned int j = 0; j < mysize; j++)
			{
				if (i <= j) //ignore the upper triangle
					continue;
				auto taski = conflictEdges[edge_id][i];
				auto taskj = conflictEdges[edge_id][j];

				std::cout << "problem in " << edge_id << ",vid=" << conflictEdges[edge_id][i] << ",vid2=" << conflictEdges[edge_id][j] << "\n";

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

//print_graph(to, original_df);
void algorithms::BufferlessNoCScheduling(models::Dataflow* const  dataflow, parameters_list_t   ) {


	models::Dataflow* to = new models::Dataflow(*dataflow);


	/// Store a list of origina l actors id.
		std::vector <ARRAY_INDEX> original_vertex_ids;
		for (auto v : to->vertices()) {
			original_vertex_ids.push_back(to->getVertexId(v));
		}


	conflictEtype conflictEdges; //stores details of flows that share noc edges
	conflictConfigs configs; //stores the details of the router configs that are shared
	vid_to_nocEid vid_to_conflict_map; //used to index newly added nodes into the conflict table, so as to remove it easily

	{ForEachChannel(to,c) {
		auto route = to->getRoute(c);
		VERBOSE_INFO("replace edge " << to->getEdgeName(c) << "by a sequence");
		addPathNode(to, c, route, conflictEdges, configs, vid_to_conflict_map, true);
	}





	// ###### At this pint, we have a SDFG with artefacts for network.
	VERBOSE_INFO ("adding path nodes done");
	std::cout << "done addpath\n";


	//resolve cnflicts for all the  (a) sources that sent data to multiple nodes.
	//				(b) destinations that receive data from multiple nodes.
	//				(c) nodes that correspond to the same coniguration
	//Use [1, origV] as it denotes the list of nodes in the original SDF


	// ###### First source and destination merge
	// ############################################
	for (auto vid : original_vertex_ids) {
		auto src = to->getVertexById(vid);
		resolveSrcConflicts(to, src, original_vertex_ids);
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
	for(conflictConfigs::iterator it = configs.begin(); it != configs.end(); it++) {
		std::cout << "Working on merge " << idx++ << " over " << configs.size() << "\n";
		mergeConfigNodesInit(to, it->first, it->second);

	}
	VERBOSE_INFO ("mergeConfigNodes Done.");
	std::cout << "done merge conflict resolve\n";

	//Remove conflicts at source and destination router links as a big node has been created
	{ForEachChannel(dataflow,c) {
		auto route = dataflow->getRoute(c);
		auto start_id = route[0];
		auto cf_it = conflictEdges.find((unsigned int)start_id);
		if(cf_it != conflictEdges.end())
	 		conflictEdges.erase(cf_it);

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


	removeOrphanNodes(to, vid_to_conflict_map, conflictEdges);


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

		mergeNodes = checkForConflicts(conflictEdges, to, omega, persched, dataflow, name);


		if(mergeNodes.size() == 0)
			break;

		std::cout << "conflict in " << name << "\n";
		//mergeConfigNodes(to, name, mergeNodes);
		mergeConfigNodesInit(to, name, mergeNodes);

		std::cout << "before removeme\n";
		removeOrphanNodes(to, vid_to_conflict_map, conflictEdges);

		std::cout << "before compp\n";


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


}

