/*
 * software_noc.cpp
 *
 *  Created on: 9 mai 2013
 *      Author: toky
 */

#include <models/NoC.h>
#include <vector>
#include <iostream>
#include <printers/stdout.h>
#include <commons/verbose.h>
#include <models/Dataflow.h>
#include <models/EventGraph.h>
#include <algorithms/normalization.h>
#include <algorithms/software_noc.h>
#include <algorithms/repetition_vector.h>
#include <algorithms/kperiodic.h>
#include <cstdlib>
#include <stack>
#include <boost/math/common_factor.hpp>
#include <climits>
#include <algorithms/schedulings.h>
#include <set>
#include <queue>
#include <tuple>
#include <commons/SDF3Wrapper.h>

static TIME_UNIT NULL_DURATION = 0;
//static bool STOP_AT_FIRST = false;
//static bool GET_PREVIOUS = false;

//vector stores the cycle length in each of the vertices
std::vector< std::vector<unsigned int> > cycles;
std::vector< std::set<unsigned int> > cycid_per_vtxid;
std::vector< std::vector<unsigned int> > cyclen_per_vtxid;
typedef std::pair<ARRAY_INDEX, ARRAY_INDEX> mypair;
typedef std::tuple<ARRAY_INDEX, ARRAY_INDEX, ARRAY_INDEX> mytuple;

struct node{
	ARRAY_INDEX index;
};

//typedef std::map< unsigned int, std::vector< std::pair<Vertex, Vertex> > > conflictEtype;
typedef std::map< unsigned int, std::vector< mypair > > conflictEtype;
typedef std::map< std::string, std::vector< mytuple > > conflictConfigs;



void print_graph (models::Dataflow * to, models::Dataflow* original) {
	return;
	static int counter = 0;
	counter ++ ;

	to->reset_computation();
	// Write the input as a dot file
	std::ofstream myfile2;
	myfile2.open ("software_noc_" + commons::toString(counter) + ".dot");
	myfile2 << printers::GenerateDOT(to);
	myfile2.close();
	VERBOSE_INFO("=========== Write file " << counter << "\n");
	commons::writeSDF3File("software_noc_" + commons::toString(counter) + ".xml", to);
	to->reset_computation();

/*
	std::string cmd = "dot software_noc_" + commons::toString(counter) + ".dot -T pdf -o software_noc_" + commons::toString(counter) + ".pdf";
	auto out_err = system(cmd.c_str());
	if(out_err)
		VERBOSE_INFO ("System call returns error\n");

	to->reset_computation();
	VERBOSE_ASSERT(computeRepetitionVector(to),"inconsistent graph");
	VERBOSE_ASSERT(computeRepetitionVector(original),"inconsistent graph");
	float ratio = -1;
	{ForEachVertex(original,orig_v) {
		auto orig_vid = original->getVertexId(orig_v);
                auto to_v = to->getVertexById(orig_vid);
                float new_ratio = (float) to->getNi(to_v) / (float)original->getNi(orig_v);
		if(ratio != -1 && ratio != new_ratio)
		{
			std::cout << "NODE " << to->getVertexName(to_v) << ",ni=" <<  to->getNi(to_v) << " failed\n";
			exit(0);
		}
		ratio = new_ratio;
	}}

	scheduling_t persched = algorithms::scheduling::CSDF_KPeriodicScheduling(to) ;

	TIME_UNIT throughput = 0;
	VERBOSE_ASSERT(throughput > 0, "fix me: period is P_i * N_I");

	//scheduling_t persched = algorithms::scheduling::bufferless_kperiodic_scheduling (to, DO_BUFFERLESS , STOP_AT_FIRST, GET_PREVIOUS);
	to->reset_computation();

	if(throughput == 0)
	{
		std::cout << "throughput == 0, exiting\n";
		exit(0);
	}
*/
}

route_t get_route_wrapper(models::Dataflow* to, Edge c, NoC* noc)
{
	// We store infos about edge to be deleted
	auto source_vtx = to->getEdgeSource(c);
	auto target_vtx = to->getEdgeTarget(c);

	//Find the core index
	auto source = to->getVertexId(source_vtx)-1; //minus 1 since the nodes start from 1 instead of zero
	auto target = to->getVertexId(target_vtx)-1;

	//Core mapping, modulo scheduling
	source = source % noc->getMeshSize();
	target = target % noc->getMeshSize();

	return noc->get_route(source, target);
}


bool operator<(const node& a, const node& b) {

	auto alen = cyclen_per_vtxid[a.index].size();
	auto blen = cyclen_per_vtxid[b.index].size();
	auto lenmin = std::min(alen, blen);

	for(unsigned int i = 0; i < lenmin; i++)
	{
		if(cyclen_per_vtxid[a.index][i] == cyclen_per_vtxid[b.index][i])
			continue;
		else
			return cyclen_per_vtxid[a.index][i] < cyclen_per_vtxid[b.index][i];
	} 

	if(alen == 0)
		return true;
	if(blen == 0)
		return false;
	return false;
}


std::vector<ARRAY_INDEX> symbolic_execution(models::Dataflow* const  from) {
	std::vector<ARRAY_INDEX> prog_order;
	// Need RV.
	VERBOSE_ASSERT(computeRepetitionVector(from),"inconsistent graph");
	// Store task to execute by number of execution.
	EXEC_COUNT total = 0 ;
	std::vector < EXEC_COUNT >  remained_execution (from->getMaxVertexId());
	{ForEachVertex(from,t) {
		EXEC_COUNT Ni =  from->getNi(t) ;
		remained_execution[from->getVertexId(t)] = Ni ;
		total += Ni ;
	}}

	// Store buffer content.
	std::vector < EXEC_COUNT >  buffer_content (from->getMaxEdgeId());
	{ForEachChannel(from,c) {
		buffer_content[from->getEdgeId(c)] = from->getPreload(c);
	}}

	// Loop over to execute everyone. Unoptimized, could be much faster if tasks were stacked, and sum of dependences stored.
	// This does not model timings, useless for throughput
	while (total > 0) {
		{ForEachVertex(from,t) {
			ARRAY_INDEX vId = from->getVertexId(t);
			bool can_execute = false;
			if ( remained_execution [vId] > 0) {
				can_execute = true;
				{ForInputEdges(from,t,inE)	{
					TOKEN_UNIT inCount  = buffer_content[from->getEdgeId(inE)];
					TOKEN_UNIT reqCount = from->getEdgeOut(inE);
					if (!(inCount >= reqCount)) {
						can_execute = false;
						break;
					}
				}}

				if (can_execute) {

					if ( std::find (prog_order.begin(), prog_order.end(), vId) == prog_order.end())
					{
						prog_order.push_back(vId);
					}

					remained_execution[from->getVertexId(t)] -= 1 ;
					total -= 1;

					{ForInputEdges(from,t,inE)	{
						buffer_content[from->getEdgeId(inE)] -= from->getEdgeOut(inE);
					}}
					{ForOutputEdges(from,t,outE)	{
						buffer_content[from->getEdgeId(outE)] += from->getEdgeIn(outE);
					}}
				}
			}
		}}
	}

	VERBOSE_INFO ("PROGRAMORDER=" << commons::toString(prog_order ) );

	return prog_order;
}


void generateEdgesMap(models::Dataflow* dataflow, std::map<int, Edge>& edge_list, NoC* noc)
{
	{ForEachEdge(dataflow,e) {
		auto v1 = dataflow->getEdgeSource(e);
		auto v2 = dataflow->getEdgeTarget(e);

		int v1_i = (int)dataflow->getVertexId(v1);
		int v2_i = (int)dataflow->getVertexId(v2);

		int index = noc->getMapIndex(v1_i, v2_i);
		edge_list[index] = e;
	}}
}


LARGE_INT gcdExtended(LARGE_INT x, LARGE_INT y, LARGE_INT *a, LARGE_INT *b)
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


void DFSUtil(Vertex v, std::vector<bool>& visited, models::Dataflow* to, std::vector<Vertex>& traversal)
{ 
	// Mark the current node as visited and print it
	auto myid = to->getVertexId(v);
	visited[myid] = true; 
	traversal.push_back(v);

	// Recur for all the vertices adjacent to this vertex 
	{ForOutputEdges(to,v,e){
		Vertex end = to->getEdgeTarget(e);
		ARRAY_INDEX endid = to->getVertexId(end);
		if(!visited[endid])
			DFSUtil(end, visited, to, traversal);
	}}
} 


void DFSUtil_PgmOrder(Vertex v, std::vector<bool>& visited, models::Dataflow* to, std::vector<ARRAY_INDEX>& removeEdgeId)
{ 
	// Mark the current node as visited and print it
	auto myid = to->getVertexId(v);
	visited[myid] = true; 
	// Recur for all the vertices adjacent to this vertex 
	{ForOutputEdges(to,v,e){
		Vertex end = to->getEdgeTarget(e);
		ARRAY_INDEX endid = to->getVertexId(end);
		if(!visited[endid])
			DFSUtil_PgmOrder(end, visited, to, removeEdgeId);
		else if (cyclen_per_vtxid[endid].size() > 0)
			removeEdgeId.push_back(to->getEdgeId(e));
		//else
		//	std::cout << "Error: wrong removal of edge " << myid << "->" << endid << "\n";
	}}
} 



std::string vertexVector2str(std::vector<Vertex>& traversal, models::Dataflow* to)
{
	 std::stringstream s;
	for(unsigned int trav_i = 0; trav_i < traversal.size(); trav_i++)
		s << to->getVertexId(traversal[trav_i]) << " ";
	return s.str();
}


//find all parents and set their value with size
void updateAncestorsHelper(Vertex vtx, models::Dataflow* to, std::set<unsigned int>& myset, std::vector<bool>& visited)
{
	auto index = to->getVertexId(vtx);
	for(auto it = myset.begin(); it != myset.end(); it++)
		cycid_per_vtxid[index].insert(*it);
	visited[index] = true;

	{ForInputEdges(to,vtx,e){
		Vertex source = to->getEdgeSource(e);
		auto sourceid = to->getVertexId(source);
		if(!visited[sourceid])
			updateAncestorsHelper(source, to, myset, visited);
	}}
}


void updateAncestorsCycle(Vertex vtx, std::set<unsigned int>& myset, models::Dataflow* to)
{
	int V = to->getVerticesCount();
	std::vector<bool> visited(V+1, false);
	updateAncestorsHelper(vtx, to, myset, visited);
}


// DFS traversal of the vertices reachable from v. It uses recursive DFSUtil() 
void DFS(models::Dataflow* to, Vertex v) 
{
	int V = to->getVerticesCount(); 
	// Mark all the vertices as not visited 
	std::vector<bool> visited(V+1, false); 

	// Call the recursive helper function to print DFS traversal 
	std::vector<Vertex> traversal;
	DFSUtil(v, visited, to, traversal);

	std::string traverseStr = vertexVector2str(traversal, to);
	VERBOSE_INFO ( "DFS Order: "  << traverseStr) ;
}


void my_dfs(models::Dataflow* adj, Vertex node, std::vector<bool> visited, Vertex start, std::vector<unsigned int>& path, std::set<unsigned int>& cycleids)
{
	auto node_id = adj->getVertexId(node);
	if (visited[node_id])
	{
		if (node == start)
		{
			std::vector<unsigned int> temp = path;
			sort(temp.begin(), temp.end());
			bool found = false;
			for(unsigned int i = 0; i < cycles.size(); i++)
			{
				if(cycles[i] == temp)
				{
					found = true;
					cycleids.insert((unsigned int)i);
					break;
				}
			}
			if(!found)
			{
				cycles.push_back(temp);
				cycleids.insert((unsigned int)cycles.size()-1);
				//std::cout << "adding a cycle\n";
			}
		}
		return;
	}

	visited[node_id]=true;
	path.push_back((unsigned int)node_id);

	{ForOutputEdges(adj,node,e) {
		auto child = adj->getEdgeTarget(e);
		my_dfs(adj, child, visited, start, path, cycleids);
	}}

	visited[node_id]=false;
	while(path.back() != node_id)
		path.pop_back();
	//popping the node_id element as well
	path.pop_back();
}


std::set<unsigned int> my_dfs_wrapper(models::Dataflow* adj, Vertex start)
{
	int V = adj->getVerticesCount();
	std::vector<bool> visited(V+1, false);
	std::vector<unsigned int> path;

	//Call the above function with the start node:
	std::set<unsigned int> mycycleids;
	my_dfs(adj, start, visited, start, path, mycycleids);

	return mycycleids;
}


models::Dataflow getTranspose(models::Dataflow* to) 
{
	models::Dataflow out;
	std::map<ARRAY_INDEX, Vertex> out_vtx_map;

	{ForEachVertex(to,source) {
		auto s_id = to->getVertexId(source);
		out_vtx_map[s_id] = out.addVertex();
	}}
	// Note: getEdgeOut and getEdgeIn are Output and input Rates of a buffer
	{ForEachVertex(to,source) {
		{ForOutputEdges(to,source,e) {
			auto target = to->getEdgeTarget(e);
			auto s_id = to->getVertexId(source);
			auto t_id = to->getVertexId(target);
			out.addEdge(out_vtx_map[t_id], out_vtx_map[s_id]);
		}}
	}}

	return out;
} 


void fillOrder(Vertex vtx, std::vector<bool>& visited, std::stack<Vertex> &Stack, models::Dataflow* to) 
{ 
	auto v = to->getVertexId(vtx);
	// Mark the current node as visited and print it 
	visited[v] = true; 
	// Recur for all the vertices adjacent to this vertex 
	{ForOutputEdges(to,vtx,e){
		auto end = to->getEdgeTarget(e);
		auto endid = to->getVertexId(end);
		if(!visited[endid])
			fillOrder(end, visited, Stack, to);
	}}
	// All vertices reachable from v are processed by now, push v  
	Stack.push(vtx); 
} 


// The main function that finds and prints all strongly connected components 
void printSCCs(models::Dataflow* to, Vertex start)
{
	int V = to->getVerticesCount(); 
	std::stack<Vertex> Stack; 

	// Mark all the vertices as not visited (For first DFS) 
	std::vector<bool> visited(V+1, false); 

	//initiate with start node first
	fillOrder(start, visited, Stack, to);
	// Fill vertices in stack according to their finishing times 
	{ForEachVertex(to,t) {
		auto v = to->getVertexId(t);
		if(!visited[v])
			fillOrder(t, visited, Stack, to);
	}}

	// Create a reversed graph 
	auto transp = getTranspose(to); 
	// Mark all the vertices as not visited (For second DFS) 
	for(int i = 0; i <= V; i++) 
		visited[i] = false; 

	// Now process all vertices in order defined by Stack 
	while (Stack.size() != 0)
	{
		// Pop a vertex from stack 
		Vertex vtx = Stack.top();
		auto v = to->getVertexId(vtx); 
		Stack.pop(); 

		// Print Strongly connected component of the popped vertex 
		if (visited[v] == false) 
		{
			std::vector<Vertex> traversal;
			DFSUtil(vtx, visited, &transp, traversal);
			//printVector(traversal, to);

			if(traversal.size() > 1)
			{
				for(unsigned int i = 0; i < traversal.size(); i++)
				{
					auto temp_vtx = traversal[i];
					std::set<unsigned int> mycycles = my_dfs_wrapper(to, temp_vtx);
					updateAncestorsCycle(temp_vtx, mycycles, to);
				}
			}
		}
	}

	for(int i = 0; i <= V; i++)
	{
		std::vector<unsigned int> temp;
		cyclen_per_vtxid.push_back(temp);

		for(auto it = cycid_per_vtxid[i].begin(); it != cycid_per_vtxid[i].end(); it++)
			cyclen_per_vtxid[i].push_back( (unsigned int)cycles[*it].size() );

		if(cycid_per_vtxid[i].size() > 0)
			sort(cyclen_per_vtxid[i].begin(), cyclen_per_vtxid[i].end());
	}
}


void setmap(std::vector<int> paths, std::vector<int>& curr_util, NoC* noc)
{
	for(unsigned int p_j = 1; p_j < paths.size()-2; p_j++)
	{
		int mapindex = noc->getMapIndex(paths[p_j], paths[p_j+1]);
		curr_util[mapindex] += 1;
	}
}


int new_findPaths(int src, NoC* noc, int core_considered, std::vector<int>& curr_util, std::map<int, route_t>& store_path, int storepath_id)
{
	if(src != -1 && core_considered != -1)
	{
		std::vector<int> paths = noc->getPath(src, core_considered);
		int max_contention = noc->findPathCost(paths);
		//std::cout << "s=" << src << ",d=" << core_considered << ",#paths=" << paths.size() << ",cont=" << max_contention << "\n";
		route_t path_str;
		for(unsigned int p_j = 0; p_j < paths.size()-1; p_j++)
			path_str.push_back(  (edge_id_t)noc->getMapIndex(paths[p_j], paths[p_j+1]) );
		store_path[storepath_id] = path_str;
		setmap(paths, curr_util, noc);
		//std::cout << "end\n";
		return max_contention;
	}
	return 0;
}


void new_mapping(Vertex vtx, std::vector<int>& core_mapping, NoC* noc, models::Dataflow* d, std::vector<int>& avail_cores, std::map<int, route_t>& routes)
{
	const int start_core = avail_cores[0];
	auto index = d->getVertexId(vtx);
	if((int)avail_cores.size() == noc->size())
	{
		core_mapping[index] = start_core;
		std::remove(avail_cores.begin(), avail_cores.end(), start_core); 
		avail_cores.resize( avail_cores.size() - 1);
		return;
	}

	float best_contention_l1 = -1;
	std::map<int, route_t> best_store_path;
	std::vector<int> best_util;
	int core_allocated = -1;

	for (auto core_considered : avail_cores)
	{
		std::cout << "core_considered=" << core_considered << "\n";
		int cont_l1 = -1;
		std::vector<int> curr_util = noc->getLinkUtil();
		std::map<int, route_t> store_path; //variable to store the route to be utilized by the (src,dest) core
		unsigned int counter = 0;
		unsigned int pathlen = 0;


		{ForInputEdges(d, vtx, e){	//Find the core index
			Vertex source_vtx = d->getEdgeSource(e);
			auto source = d->getVertexId(source_vtx);
			int src_core = core_mapping[source];
			int storepath_id = noc->getMapIndex((int)source, (int)index);
			if(src_core != -1)
				pathlen += noc->getPathLength(src_core, core_considered);
		}}

		{ForOutputEdges(d, vtx, e){
			Vertex target_vtx = d->getEdgeTarget(e);
			auto target = d->getVertexId(target_vtx);
			int tgt_core = core_mapping[target];
			int storepath_id = noc->getMapIndex((int)index, (int)target);
			if(tgt_core != -1)
				pathlen += noc->getPathLength(core_considered, tgt_core);
		}}


		if(best_contention_l1 != -1 && pathlen > best_contention_l1) //reducing search space
			continue;

		{ForInputEdges(d, vtx, e){	//Find the core index
			Vertex source_vtx = d->getEdgeSource(e);
			auto source = d->getVertexId(source_vtx);
			int src_core = core_mapping[source];
			int storepath_id = noc->getMapIndex((int)source, (int)index);
			if(src_core != -1)
			{
				counter++;
				auto temp_cont_l1 = new_findPaths(src_core, noc, core_considered, curr_util, store_path, storepath_id);
				cont_l1 = std::max( temp_cont_l1, cont_l1 );
			}
		}}

		{ForOutputEdges(d, vtx, e){
			Vertex target_vtx = d->getEdgeTarget(e);
			auto target = d->getVertexId(target_vtx);
			int tgt_core = core_mapping[target];
			int storepath_id = noc->getMapIndex((int)index, (int)target);
			if(tgt_core != -1)
			{
				counter++;
				auto temp_cont_l1 = new_findPaths(core_considered, noc, tgt_core, curr_util, store_path, storepath_id);
				cont_l1 = std::max( temp_cont_l1, cont_l1 );
			}
		}}

		float cost = (float)cont_l1;
		if(counter > 0 )
			cost += (float)pathlen/(float)counter;


		std::cout << "cost=" << cost << "\n";

		if(best_contention_l1 == -1 || cost < best_contention_l1)
		{
			best_contention_l1 = cost;
			best_store_path = store_path;
			core_allocated = core_considered;
			best_util = curr_util;
		}
	}

	core_mapping[index] = core_allocated;
	std::remove(avail_cores.begin(), avail_cores.end(), core_allocated);
	avail_cores.resize(avail_cores.size()-1);
	noc->setLinkUtil(best_util);

	for(auto it: best_store_path)
	{
		if(routes.find(it.first) != routes.end())
			VERBOSE_INFO ( "already one route is stored fro this");
		routes[it.first] = it.second;
	}
	std::cout << "allocating to " << core_allocated << "\n";
}


int findPaths(int src, NoC* noc, int core_considered, std::vector<int>& curr_util, std::map<int, route_t>& store_path, int storepath_id)
{
	if(src != -1 && core_considered != -1)
	{
		int max_contention_l2 = INT_MAX;
		int path_idx = -1;

		std::vector< std::vector<int> > paths = noc->getShortestPaths(src, core_considered); //estimate the cost
		//std::cout << "s=" << src << ",d=" << core_considered << ",#paths=" << paths.size() << "\n";
		for(unsigned int p_i = 0; p_i < paths.size(); p_i++)
		{
			int cont_l2 = 0;
			for(unsigned int p_j = 1; p_j < paths[p_i].size()-2; p_j++)
			{
				//std::cout << paths[p_i][p_j] << "->" << paths[p_i][p_j+1] << " ";
				int mapindex = noc->getMapIndex(paths[p_i][p_j], paths[p_i][p_j+1]);
				//if(curr_util.find(mapindex) != curr_util.end())
				cont_l2 = std::max(cont_l2, curr_util[mapindex]);
			}
			if(cont_l2 < max_contention_l2)
			{
				max_contention_l2 = cont_l2;
				path_idx = p_i;
				//std::cout << "\nabove path is chosen";
			}
			//std::cout << "\n";
		}
		route_t path_str;
		for(unsigned int p_j = 0; p_j < paths[path_idx].size()-1; p_j++)
			path_str.push_back(  (edge_id_t)noc->getMapIndex(paths[path_idx][p_j], paths[path_idx][p_j+1]) );
		store_path[storepath_id] = path_str;

		setmap(paths[path_idx], curr_util, noc);
		return max_contention_l2;
	}
	return 0;
}


void mapping(Vertex vtx, std::vector<int>& core_mapping, NoC* noc, models::Dataflow* d, std::vector<int>& avail_cores, std::map<int, route_t>& routes)
{
	const int start_core = avail_cores[0];
	auto index = d->getVertexId(vtx);
	if((int)avail_cores.size() == noc->size())
	{
		core_mapping[index] = start_core;
		std::remove(avail_cores.begin(), avail_cores.end(), start_core); 
		//std::cout << "mapping node " << index << " to core " << start_core << ",size=" << avail_cores.size() << "\n";
		avail_cores.resize( avail_cores.size() - 1);
		return;
	}

	float best_contention_l1 = -1;
	std::map<int, route_t> best_store_path;
	//std::map<int, int> best_util;
	std::vector<int> best_util;

	int core_allocated = -1;

	for (auto core_considered : avail_cores)
	{
		//std::cout << "considering core " << core_considered << "\n";
		int cont_l1 = -1;
		//std::map<int, int> curr_util = noc->getLinkUtil();
		std::vector<int> curr_util = noc->getLinkUtil();

		std::map<int, route_t> store_path; //variable to store the route to be utilized by the (src,dest) core
		unsigned int counter = 0;
		unsigned int pathlen = 0;

		{ForInputEdges(d, vtx, e){
			//Find the core index
			Vertex source_vtx = d->getEdgeSource(e);
			auto source = d->getVertexId(source_vtx);
			int src_core = core_mapping[source];
			int storepath_id = noc->getMapIndex((int)source, (int)index);
			auto temp_cont_l1 = findPaths(src_core, noc, core_considered, curr_util, store_path, storepath_id);
			cont_l1 = std::max( temp_cont_l1, cont_l1 );
			if(src_core != -1)
			{
				counter++;
				pathlen += noc->getPathLength(src_core, core_considered);
			}
			//std::cout << "cont_l1=" << cont_l1 << "\n";
		}}
		{ForOutputEdges(d, vtx, e){
			Vertex target_vtx = d->getEdgeTarget(e);
			auto target = d->getVertexId(target_vtx);
			int tgt_core = core_mapping[target];
			int storepath_id = noc->getMapIndex((int)index, (int)target);
			auto temp_cont_l1 = findPaths(core_considered, noc, tgt_core, curr_util, store_path, storepath_id);
			cont_l1 = std::max( temp_cont_l1, cont_l1 );
			if(tgt_core != -1)
			{
				counter++;
				pathlen += noc->getPathLength(core_considered, tgt_core);
			}
			//std::cout << "2-cont_l1=" << cont_l1 << "\n";
		}}
		float cost = (float)cont_l1;
		if(counter > 0 )
			cost += (float)pathlen/(float)counter;
		if(best_contention_l1 == -1 || cost < best_contention_l1)
		{
			best_contention_l1 = cost;
			best_store_path = store_path;
			core_allocated = core_considered;
			best_util = curr_util;
		}
		//std::cout << "trying core " << core_considered << ",cont=" << cont_l1 << ",lenfac=" << (float)pathlen/(float)counter << ",cost=" << cost << "\n";
	}
	//std::cout << "after edges\n";
	core_mapping[index] = core_allocated;
	//std::cout << "mapping node " << index << " to core " << core_allocated << ",cont=" << best_contention_l1 << "\n";
	std::remove(avail_cores.begin(), avail_cores.end(), core_allocated);
	avail_cores.resize(avail_cores.size()-1);
	noc->setLinkUtil(best_util);

	for(auto it: best_store_path)
	{
		if(routes.find(it.first) != routes.end()) {
			VERBOSE_INFO ( "already one route is stored fro this");
		}
		routes[it.first] = it.second;
	}
} 


//remove the current edge between nodes add intermediate nodes based on the path between them
void taskAndNoCMapping(models::Dataflow* input, models::Dataflow* to, Vertex start, NoC* noc, std::map<int, route_t>& routes/*, std::vector<ARRAY_INDEX>& prog_order*/)
{
	int V = to->getVerticesCount();
	std::priority_queue<node> pq;
	std::vector<bool> visited(V+1, false);
	std::vector<int> core_mapping(V+1, -1);

	//list of cores that are available
	std::vector<int> available_cores;//{5, 6, 10, 9, 8, 4, 0, 1, 2, 3, 7, 11, 15, 14, 13, 12};
	int origV = input->getVerticesCount();
	if(origV <= 16)
	{
		std::vector<int> temp_vec{5, 6, 10, 9, 8, 4, 0, 1, 2, 3, 7, 11, 15, 14, 13, 12};
		available_cores = temp_vec;
	}
	else
	{
		for(int i = 0; i < noc->getMeshSize(); i++)
			available_cores.push_back(i);
	}

	noc->clear();

	auto top = start;
	ARRAY_INDEX endid = to->getVertexId(top);

	node temp2;
	temp2.index = endid;
	visited[endid] = true;
	pq.push(temp2);

	while(!pq.empty())
	{
		top = to->getVertexById(pq.top().index);
		pq.pop();
		if(top != start)
		{
			std::cout << "mapping " << to->getVertexId(top) << "\n";
			if( noc->getMeshSize() <= 9)
				mapping(top, core_mapping, noc, input, available_cores, routes);
			else
				new_mapping(top, core_mapping, noc, input, available_cores, routes);
		}
		visited[to->getVertexId(top)] = true;

		{ForOutputEdges(to, top, e){
			Vertex end = to->getEdgeTarget(e);
			ARRAY_INDEX endid = to->getVertexId(end);

			//std::cout << "trying node " << endid << "\n";
			if(!visited[endid])
			{
				bool flag = true;
				{ForInputEdges(to, end, e2){
					Vertex e2end = to->getEdgeSource(e2);
					ARRAY_INDEX e2endid = to->getVertexId(e2end);
					if(!visited[e2endid])//if parent is not executed, cannot map this node
						flag = false;
				}}
				if(flag)
				{
					//std::cout << "adding " << endid << " to queue\n";
					node temp;
					temp.index = endid;
					pq.push(temp);
				}
			}
		}}
	}

	/*for(unsigned int s = 0; s < prog_order.size(); s++)
	{
		auto top = to->getVertexById(prog_order[s]);
		mapping(top, core_mapping, noc, input, available_cores, routes);
	}*/

/*
	for(ARRAY_INDEX s = 1; s <= (ARRAY_INDEX)(V-1); s++)
	{
		prog_order[s] = prog_order[s] + 1 - 1;
		auto top = to->getVertexById(s);
		mapping(top, core_mapping, noc, input, available_cores, routes);
	}
*/
	VERBOSE_INFO ( "srjkvr-mapping " << commons::toString(core_mapping) ) ;

}


//remove the current edge between nodes
//add intermediate nodes based on the path between them
std::vector<Vertex> addPathNode(models::Dataflow* d, Edge c, route_t list, conflictEtype& returnValue, conflictConfigs& configs, bool addConfigNode)
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
	for (auto e : list) {
		// we create a new vertex "middle"
		auto middle = d->addVertex();
		new_vertices.push_back(middle);

		////PLLLEASE DONT CHANGE THE "mid-" value in the name"
		std::stringstream ss;
		ss << "mid-" << source << "," << target << "-" << e;

		mypair pair_temp;
		pair_temp.first = d->getVertexId(middle);
		pair_temp.second = d->getVertexId(source_vtx);

		returnValue[(unsigned int)e].push_back(pair_temp);

		d->setVertexName(middle,ss.str());

		d->setPhasesQuantity(middle,1); // number of state for the actor, only one in SDF
		d->setVertexDuration(middle,{1}); // is specify for every state , only one for SDF.
		d->setReentrancyFactor(middle,1); // This is the reentrancy, it avoid a task to be executed more than once at the same time.


		//keep track of the 
		if(addConfigNode && key_str != "")
		{
			get<2>(configs[key_str][configs[key_str].size()-1]) = d->getVertexId(middle);
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
			config_name << "cfg-" << source << "," << target << "-" << list[list_idx] << "_" << list[list_idx+1];


			auto vtx = d->addVertex();
			mytuple tuple_temp;
			get<0>(tuple_temp) = d->getVertexId(vtx);
			get<1>(tuple_temp) = d->getVertexId(middle);
			configs[config_key.str()].push_back(tuple_temp);
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
			d->setPreload(e1,0);  // preload is M0

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


//Remove the cyclic edges
Vertex removeCycleEdges(models::Dataflow* to, std::vector<ARRAY_INDEX>& prog_order)
{
	int origV = to->getVerticesCount();
	std::vector<bool> visited(origV, false);
	std::vector<ARRAY_INDEX> removeEdgeId;
	std::vector<ARRAY_INDEX> vertexId;


	for(int i = 0; i < (int)prog_order.size(); i++)
	{
		auto vid = prog_order[i];
		Vertex v = to->getVertexById(vid);
		if(!visited[vid])
			DFSUtil_PgmOrder(v, visited, to, removeEdgeId);
	}
	for(int i = 0; i < (int)removeEdgeId.size(); i++)
	{
		auto e = to->getEdgeById( removeEdgeId[i] );
		auto src = to->getEdgeSource(e);
		auto dst = to->getEdgeTarget(e);

		VERBOSE_INFO ( "Removing edge " << to->getVertexId(src) << "->" << to->getVertexId(dst) );
		to->removeEdge(e);
	}

	//find isolated nodes
	{ForEachVertex(to,t) {
		if(to->getVertexInDegree(t) == 0)
		{
			vertexId.push_back( to->getVertexId(t) );
			//std::cout << "inside function::::: adding new edge between start " << to->getVertexId(t) << "\n";
		}
	}}


	//Now create the start node
	auto start = to->addVertex();
	to->setVertexName(start, "start");

	//add edges between start node and isolated edges
	for(int i = 0; i < (int)vertexId.size(); i++)
	{
		auto t = to->getVertexById( vertexId[i]);
		to->addEdge(start, t);
	}
	return start;
}


//Process the graph for DFS, etc. in this function
std::map<int, route_t> graphProcessing(models::Dataflow* dataflow, NoC* noc, std::vector<ARRAY_INDEX>& prog_order)
{
	models::Dataflow* to = new models::Dataflow(*dataflow);
	to->reset_computation();
	bool myflag = false;
	bool myflag2 = false;
	Vertex top;
	std::map<int, route_t> routes;

	auto start = to->addVertex();
	to->setVertexName(start, "start");
	{ForEachVertex(to,t) {
		if( (t != start) && (!myflag) && (!myflag2))
		{
			top = t;
			myflag2 = true;
		}

		if(t == start)
		{
			continue;
		}
		if(dataflow->getVertexInDegree( dataflow->getVertexById(to->getVertexId(t)) ) == 0)
		{
			myflag = true;
			to->addEdge(start, t);
			std::cout << "adding new edge between start " << to->getVertexId(t) << "\n";
		}
	}}

	if(!myflag)
		to->addEdge(start, top);

	int V = to->getVerticesCount();
	for(int i = 0; i < V+1; i++)
	{
		std::set<unsigned int> temp;
		cycid_per_vtxid.push_back(temp);
	}

	printSCCs(to, start);

	models::Dataflow* to2 = new models::Dataflow(*dataflow);
	to2->reset_computation();
	start = removeCycleEdges(to2, prog_order);

	taskAndNoCMapping(dataflow, to2, start, noc, routes/*, prog_order*/);

	return routes;
}


//This adds intermediate nodes by using fixed XY routing protocol
void addIntermediateNodes(models::Dataflow* dataflow, NoC* noc, conflictEtype& returnValue, std::map<int, route_t>& routes, conflictConfigs& configs)
{
	std::map<int, Edge> edge_list;
	generateEdgesMap(dataflow, edge_list, noc);

	for(auto it:routes)
		addPathNode(dataflow, edge_list[it.first], it.second, returnValue, configs, false);
}


void addDependency(models::Dataflow* d, const Vertex vi, const Vertex vj, EXEC_COUNT ni, EXEC_COUNT nj)
{
	LARGE_INT a, b;
	auto gcd_value = gcdExtended((LARGE_INT) ni, (LARGE_INT) nj, &a, &b);

	ni /= gcd_value;
	nj /= gcd_value;
	if (d->is_read_only()) {
		d->reset_computation();
	}
	auto e1 = d->addEdge(vi, vj);
	auto e2 = d->addEdge(vj, vi);

	gcd_value = ni + nj - gcdExtended((LARGE_INT) ni, (LARGE_INT) nj, &a, &b);

	d->setEdgeOutPhases(e1,{(TOKEN_UNIT)ni});
	d->setEdgeInPhases(e1,{(TOKEN_UNIT)nj});
	d->setPreload(e1,gcd_value);  // preload is M0

	d->setEdgeOutPhases(e2,{(TOKEN_UNIT)nj});
	d->setEdgeInPhases(e2,{(TOKEN_UNIT)ni});
	d->setPreload(e2,0);  // preload is M0

	VERBOSE_INFO ( "Win=" << ni << " Wout=" << nj << " Mo=" << gcd_value);

}


//Function to remove all the edges that connect a particular vertex
void removeAllEdgesVertex(models::Dataflow* d, Vertex vtx)
{
	{ForInputEdges(d, vtx, E)	{
		d->removeEdge(E);
	}}
	{ForOutputEdges(d, vtx, E)	{
		d->removeEdge(E);
	}}
	// d->removeVertex(vtx);
}


//Function to remove the edges and obtain phase vector values
//v1 -> cfg -> v2
void getEdgesPhaseVector(models::Dataflow* d, Vertex& v1, Vertex cfg, Vertex& v2, std::vector<TOKEN_UNIT>& in, std::vector<TOKEN_UNIT>& out)
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


bool resolveDestConflicts(models::Dataflow* d, Vertex dst, int origV)
{
	LARGE_INT mygcd = 0, a, b;
	std::vector<Edge> dstedges, rtredges;
	TOKEN_UNIT flow = 0, preload = 0;

	//1. Find all the edges from the vertex to router (dstedges)
	// and router to next edge in the NoC (rtredges)
	{ForInputEdges(d,dst,inE)	{
		auto prev_node = d->getEdgeSource(inE);
		if((int)d->getVertexId(prev_node) > origV && prev_node!= dst)
		{
			dstedges.push_back(inE);
			auto edgeout = d->getEdgeOut(inE);

			if(mygcd == 0)
				mygcd = edgeout;
			else
				mygcd = gcdExtended(edgeout, mygcd, &a, &b);

			{ForInputEdges(d,prev_node,inE_p)     {
				if(prev_node != d->getEdgeSource(inE_p))
					rtredges.push_back(inE_p);
			}}
		}
	}}


	//1B. Do some sanity check here
	VERBOSE_ASSERT (dstedges.size() == rtredges.size() ,  "BIG ERROR in resolveDestConflicts" );

	if(dstedges.size() <= 1)
		return false;

	//1C. Initialize token vector
	std::vector<TOKEN_UNIT> temp;
	std::vector< std::vector<TOKEN_UNIT> > token_vec (dstedges.size(), temp);

	//2. calculate the preload and flow values of all incoming edges from destination vertices
	//Remove the edges between router and destionation. Remove the router nodes
	for(int i = 0; i < (int)dstedges.size(); i++)
	{
		for(int j = 0; j < (int)dstedges.size(); j++)
		{
			for(int vi = 0; vi < (int)d->getEdgeOut(dstedges[i])/mygcd; vi++)
			{
				if (i == j)
					token_vec[j].push_back(1);
				else
					token_vec[j].push_back(0);
			}
		}
		flow += d->getEdgeOut( dstedges[i] );
		preload += d->getPreload( dstedges[i] );	
		VERBOSE_INFO ( "states=" << flow << ",preload=" << preload );
	}

	//2B. Divide flow by gcd to reduce the states
	flow = flow/mygcd;
	
	VERBOSE_ASSERT(flow > 0, "This case is not supported: flow between task has to be strictly positive");

	//2C. Create the phase duration and token per phase for the new router node
	std::vector<TOKEN_UNIT> dsttoken(flow, 1);
	std::vector<TIME_UNIT> phaseDurVec(flow, 1.0);

	//3. Create the BIG router, by combining all the router nodes that were used to reach the destination
	auto middle = d->addVertex();
	std::stringstream ss;
	ss << d->getVertexId(dst) << "-D";
	d->setVertexName(middle, ss.str());
	d->setPhasesQuantity(middle, flow); // number of state for the actor, only one in SDF
	d->setVertexDuration(middle, phaseDurVec); // is specify for every state , only one for SDF.
	d->setReentrancyFactor(middle, 1); // This is the reentrancy, it avoid a task to be executed more than once at the same time.
	VERBOSE_INFO ( "DST_CONF(ID="<< d->getVertexId(dst) << "):Done creating big router node  " << d->getVertexId(middle) << ",gcd=" << mygcd );

	//3B. Add edge between big router and destination vertex
	auto dstEdge = d->addEdge(middle,dst);
	d->setPreload(dstEdge, preload);
	d->setEdgeOutPhases(dstEdge, {flow*mygcd});
	d->setEdgeInPhases(dstEdge, dsttoken);


	//4. Setup the new edges. First remove the edge between router and the next NoC link. Setup the flow appropriately
	//Remove the router vertex and the edge between routet to the next NoC link
	for(int i = 0; i < (int)rtredges.size(); i++)
	{
		//(v1)  rtredges[i]  v2
		auto v1 = d->getEdgeSource(rtredges[i]);
		auto v2 = d->getEdgeTarget(rtredges[i]);
		auto loc_preload = d->getPreload(rtredges[i]);

		//std::cout << "dst=" << d->getVertexName(dst) << ",big_middle=" << d->getVertexName(middle) << ",v1=" << d->getVertexName(v1) << ",deleting=" << d->getVertexName(v2) << "\n";
		removeAllEdgesVertex(d, v2);

		auto new_edge = d->addEdge(v1, middle);
		d->setEdgeType(new_edge,EDGE_TYPE::BUFFERLESS_EDGE);
		d->setEdgeOutPhases(new_edge, token_vec[i]);
		d->setEdgeInPhases(new_edge, {1});
		d->setPreload(new_edge,loc_preload);  // preload is M0

	}
	return true;
}


bool mergeConfigNodes(models::Dataflow* d, std::string name , std::vector< mytuple > mergeNodes)
{
	if (mergeNodes.size() <= 1)
		return false;
	d->reset_computation();

	int mn_size = (int)mergeNodes.size();
	//std::cout << "mergeConfigNodes " << commons::toString(mergeNodes) << " for " << name << "\n";
	models::Dataflow* temp_d = new models::Dataflow(*d);
	VERBOSE_ASSERT(computeRepetitionVector(temp_d),"inconsistent graph");

	TOKEN_UNIT flow = (TOKEN_UNIT)mn_size, preload = 0, temp_flow = 0;
	LARGE_INT gcd_value = 0, a, b;
	//Find GCD value
	for(int i = 0; i < mn_size; i++)
	{
		Vertex vi = temp_d->getVertexById(get<0>(mergeNodes[i]));
		auto ni = (LARGE_INT) temp_d->getNi(vi);
		if(gcd_value == 0)
			gcd_value = ni;
		gcd_value = gcdExtended(gcd_value, ni, &a, &b);
	}
	//Find the total flow
	for(int i = 0; i < mn_size; i++)
	{
		Vertex vi = temp_d->getVertexById(get<0>(mergeNodes[i]));
		auto ni = (LARGE_INT) temp_d->getNi(vi);
		temp_flow += ni/gcd_value;
	}
	flow = temp_flow;
	//std::cout << "flow=" << flow << "\n";

	//1. Initialize token vecot
	std::vector<TOKEN_UNIT> temp(flow, 0);
	std::vector< std::vector<TOKEN_UNIT> > token_vec (mn_size, temp);
	std::vector<TIME_UNIT> phaseDurVec(flow, NULL_DURATION); //Create the phase duration
	LARGE_INT start = 0, end;
	for(int i = 0; i < mn_size; i++)
	{
		Vertex vi = temp_d->getVertexById(get<0>(mergeNodes[i]));
		auto ni = (LARGE_INT) temp_d->getNi(vi);
		end = start + ni/gcd_value;
		//std::cout << "ni=" << ni << ",s=" << start << ",end=" << end << "\n";
		for(LARGE_INT j = start; j < end; j++)
			token_vec[i][j] = 1;
		start = end;
	}
	//std::cout << "token=" << commons::toString(token_vec) << "\n";

	//3. Create the BIG router
	auto middle = d->addVertex();
	d->setVertexName(middle, name);
	d->setPhasesQuantity(middle, flow); // number of state for the actor, only one in SDF
	d->setVertexDuration(middle, phaseDurVec); // is specify for every state , only one for SDF.
	d->setReentrancyFactor(middle, 1); // This is the reentrancy, it avoid a task to be executed more than once at the same time.

	//4. Remove the edges before/after the config node. Setup the flow. Remove the router nodes.
	for(int i = 0; i < mn_size; i++)
	{
		Vertex cfgVtx = d->getVertexById( get<0>(mergeNodes[i]) );
		Vertex v1;// = d->getVertexById( get<1>(mergeNodes[i]) );
		Vertex v2;// = d->getVertexById( get<2>(mergeNodes[i]) );

		std::vector<TOKEN_UNIT> myin, myout;
		getEdgesPhaseVector(d, v1, cfgVtx, v2, myin, myout);

		//std::cout << "v1=" << d->getVertexName(v1) << ",cfg=" << d->getVertexName(cfgVtx) << ",v2=" << d->getVertexName(v2) << "\n";
		//std::cout << "myin " << commons::toString(myin) << "\n";
		//std::cout << "myout " << commons::toString(myout) << "\n";
		//std::cout << "token_vec_size=" << token_vec[i].size() << ",myin_size=" << myin.size() << "\n";
		//std::cout << "adding edge between " << v1 << " and " << middle << "\n";

		auto new_edge = d->addEdge(v1, middle);
		d->setEdgeType(new_edge,EDGE_TYPE::BUFFERLESS_EDGE);
		d->setPreload(new_edge, preload);
		//d->setEdgeInPhases(new_edge, {1});
		d->setEdgeInPhases(new_edge, myin);
		d->setEdgeOutPhases(new_edge, token_vec[i]);

		//std::cout << "adding edge between " << middle << " and " << v2 << "\n";
		new_edge = d->addEdge(middle, v2);
		d->setEdgeType(new_edge,EDGE_TYPE::BUFFERLESS_EDGE);
		d->setPreload(new_edge, preload);
		d->setEdgeInPhases(new_edge, token_vec[i]);
		//d->setEdgeOutPhases(new_edge, {1});
		d->setEdgeOutPhases(new_edge, myout);
	}
	//std::cout << "merging " << name << " done\n";
	d->reset_computation();
	return true;
}


bool resolveSrcConflicts(models::Dataflow* d, Vertex src, int origV)
{

	VERBOSE_INFO ( "resolveSrcConflicts for "  << d->getVertexName(src) << " with original number of vertices " << origV);

	std::vector<Edge> srcedges, rtredges;
	TOKEN_UNIT in_flow = 0;
	TOKEN_UNIT preload = 0;
	LARGE_INT mygcd = 0, a, b;


	//1. Find all the edges from the vertex to router 
	// and router to next edge in the NoC
	{ForOutputEdges(d,src,outE)	{
		auto next_node = d->getEdgeTarget(outE);
		if((int)d->getVertexId(next_node) > origV && next_node!= src)
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
		else if((int)d->getVertexId(next_node) <= origV)
		{
			std::cout << "src=" << d->getVertexName(src) << " still has outgoing edge to " << d->getVertexName(next_node) << "\n";
			exit(0);
		}
	}}
	in_flow /= mygcd;

	//1B. Do some sanity check here
	VERBOSE_ASSERT(srcedges.size() == rtredges.size(),  "BIG ERROR in resolveSrcConflicts\n");

	if(srcedges.size() <= 1) {
		//std::cout << "no resolveSrcConflicts for vertex " << d->getVertexId(src) << "\n";
		return false;
	}

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

void getPrevAndNextVertex(ARRAY_INDEX vtx_id, models::Dataflow* to, ARRAY_INDEX& prev, ARRAY_INDEX& next)
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

std::vector<mytuple> checkForConflicts(conflictEtype& conflictEdges, models::Dataflow* to, TIME_UNIT HP, scheduling_t& persched, models::Dataflow* original, std::string& conflictname)
{

	std::vector< mytuple > mergeNodes;
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
				auto taski_vtx = to->getVertexById(conflictEdges[edge_id][i].first);
				auto taskj_vtx = to->getVertexById(conflictEdges[edge_id][j].first);
				auto taski = conflictEdges[edge_id][i].first;
				auto taskj = conflictEdges[edge_id][j].first;

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
							conflictname = to->getVertexName(taski_vtx);
							get<0>(tuple_temp) = taski;
							getPrevAndNextVertex(taski, to, prev, next);
							get<1>(tuple_temp) = prev;
							get<2>(tuple_temp) = next;
							std::cout << prev << "-> " << taski << "->" << next << "\n";
							mergeNodes.push_back(tuple_temp);

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
								mergeNodes.push_back(tuple_temp);
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


void findHP(models::Dataflow* orig, models::Dataflow* to, scheduling_t& persched, TIME_UNIT* HP, unsigned long* LCM)
{
	//fins ratio between orig and new graph for the nodes
	float ratio = -1;
	VERBOSE_ASSERT(computeRepetitionVector(orig),"inconsistent graph");
	VERBOSE_ASSERT(computeRepetitionVector(to),"inconsistent graph");
	{ForEachVertex(orig,orig_v) {
		auto orig_vid = orig->getVertexId(orig_v);
                auto to_v = to->getVertexById(orig_vid);
                float new_ratio = (float) to->getNi(to_v) / (float)orig->getNi(orig_v);
		if(ratio != -1 && ratio != new_ratio)
			std::cout << "NODE " << to->getVertexName(to_v) << ",ni=" <<  to->getNi(to_v) << " failed\n";
		ratio = new_ratio;
	}}

	*HP = 0.0;
	*LCM = 1;
	for (auto  key : persched) {
		auto task = key.first;
		auto task_vtx = to->getVertexById(key.first);
		std::stringstream ss;
		for(int i = 0; i < (int)persched[task].second.size(); i++)
			ss << std::setprecision(13) << persched[task].second[i] << " ";

		*HP =    ( persched[task].first * to->getNi(task_vtx) ) / (persched[task].second.size()) ;
		VERBOSE_INFO ( "Task " <<  to->getVertexName(task_vtx) <<  " : duration=[ " << commons::toString(to->getVertexPhaseDuration(task_vtx)) <<  "] period=" <<  persched[task].first << " HP=" << *HP << " Ni=" << to->getNi(task_vtx) << " starts=[ " << ss.str() << "]");
		*LCM = boost::math::lcm(*LCM, to->getNi(task_vtx));

	}
	*LCM = boost::math::lcm(*LCM, (unsigned long)*HP);
}



bool algorithms::isConflictPresent(LARGE_INT HP, TIME_UNIT si, LARGE_INT ni, TIME_UNIT sj, LARGE_INT nj)
{
	LARGE_INT temp1, temp2;
	LARGE_INT lhs = (LARGE_INT)(si - sj) * ni * nj;
	LARGE_INT rhs_gcd = HP * gcdExtended(myabs(ni), myabs(nj), &temp1, &temp2);

	if(si == sj) //if start time is same conflict
	{
		return true;
	}

	if (myabs(lhs) % rhs_gcd != 0)
	{
		return false; //No integral solutions exist as the division will yield floating point
	}
	else //check if any other solution exists
	{
		return true; //there exists a solution for sure
	}
}

struct phase_info_struct
{
	int thds;
	TIME_UNIT exec_time;
	int send_pkts;
	std::string name;
};
typedef struct phase_info_struct phase_info;
phase_info getPhaseStruct(int t, TIME_UNIT e, int p, std::string n)
{
	phase_info ret{t, e, p, n};
	return ret;
}

void algorithms::generate_lte_sdf(models::Dataflow* const , parameters_list_t   param_list)
{
	double yscale = 1;
	if (param_list.count("yscale") == 1) yscale = std::stod(param_list["yscale"]);
	models::Dataflow to;

	std::vector<phase_info> phases;

/*
	//phases.push_back( getPhaseStruct(4, 392504, 16, "miwf"));
	//phases.push_back( getPhaseStruct(2, 230635, 32, "cwac"));

	phases.push_back( getPhaseStruct(2, 392504, 16, "miwf"));
	phases.push_back( getPhaseStruct(2, 230635, 32, "cwac"));
	phases.push_back( getPhaseStruct(2, 353448, 32, "ifft"));
	phases.push_back( getPhaseStruct(2, 267559, 32, "dd"));

	phases.push_back( getPhaseStruct(1, 392504, 64/4, "miwf"));
	phases.push_back( getPhaseStruct(5, 230635, 128/4, "cwac"));
	phases.push_back( getPhaseStruct(3, 353448, 128/4, "ifft"));
	phases.push_back( getPhaseStruct(3, 267559, 32, "dd"));

	phases.push_back( getPhaseStruct(16, 392504, 64/4, "miwf"));
	phases.push_back( getPhaseStruct(19, 230635, 128/4, "cwac"));
	phases.push_back( getPhaseStruct(12, 353448, 128/4, "ifft"));
	phases.push_back( getPhaseStruct(19, 267559, 32, "dd"));
*/
	phases.push_back( getPhaseStruct(64, 392504, 16, "miwf"));
	phases.push_back( getPhaseStruct(75, 230635, 32, "cwac"));
	phases.push_back( getPhaseStruct(24, 353448, 32, "ifft"));
	phases.push_back( getPhaseStruct(75, 267559, 32, "dd"));


	std::vector< std::vector<ARRAY_INDEX> > vertex_info;
	for(unsigned int i = 0; i < phases.size(); i++)
	{
		auto ph_time = phases[i].exec_time;
		int ph_thds = phases[i].thds;
		std::string ph_name = phases[i].name;
		std::vector<ARRAY_INDEX> temp_vertex_list;

		for(int j = 0; j < ph_thds; j++)
		{
			auto new_vtx = to.addVertex();
			temp_vertex_list.push_back( to.getVertexId(new_vtx) );
			std::stringstream ss;
			ss << ph_name << "_" << j;

			to.setVertexName(new_vtx,ss.str());
			to.setPhasesQuantity(new_vtx,1); // number of state for the actor, only one in SDF
			to.setVertexDuration(new_vtx,{ ph_time }); // is specify for every state , only one for SDF.
			to.setReentrancyFactor(new_vtx,1); // This is the reentrancy, it avoid a task to be executed more than once at the same time.
		}
		vertex_info.push_back(temp_vertex_list);
	}

	for(unsigned int i = 0; i < phases.size() - 1; i++)
	{
		auto pkts = phases[i].send_pkts;
		for(unsigned int j = 0; j < vertex_info[i].size(); j++)
		{
			for (unsigned int k = 0; k < vertex_info[i+1].size(); k++)
			{
				auto src = to.getVertexById( vertex_info[i][j] );
				auto dst = to.getVertexById( vertex_info[i+1][k] );
				auto e1 = to.addEdge(src, dst);
				to.setEdgeInPhases(e1,{pkts});
				to.setEdgeOutPhases(e1,{pkts}); // and the consumption rate (as many rates as states for the associated task)
				to.setPreload(e1,0);  // preload is M0
			}
		}
	}

	commons::writeSDF3File("lte_sdf_238.xml", &to);
}

void algorithms::software_noc_bufferless(models::Dataflow* const  dataflow, parameters_list_t   param_list)
{
	double yscale = 1;
	if (param_list.count("yscale") == 1) yscale = std::stod(param_list["yscale"]);

	conflictEtype conflictEdges; //stores details of flows that share noc edges
	conflictConfigs configs; //stores the details of the router configs that are shared
	int mesh_row = (int)ceil(sqrt(dataflow->getVerticesCount()));

	if(mesh_row <= 4)
		mesh_row = 4;

	NoC *noc = new NoC(mesh_row, mesh_row, 1); //Init NoC

	if(mesh_row <= 9)
		noc->generateShortestPaths();

	// STEP 0.2 - Assert SDF
	models::Dataflow* to = new models::Dataflow(*dataflow);
	models::Dataflow* original_df = new models::Dataflow(*dataflow);

	//print_graph(to, original_df);
	std::map<int, Edge> edge_list;

	//symbolic execution to find program execution order
	models::Dataflow* to2 = new models::Dataflow(*dataflow);
	std::vector<ARRAY_INDEX> prog_order = symbolic_execution(to2);
	delete to2;

	//do some processing to perfrom task mapping, NoC route determination and add intermediate nodes
	std::map<int, route_t> routes = graphProcessing(to, noc, prog_order);
	generateEdgesMap(to, edge_list, noc);

	//print_graph(to, original_df);
	for(auto it:routes) {
		Edge e = edge_list[it.first];
		Vertex esrc = to->getEdgeSource(e);
		VERBOSE_INFO("replace edge " << e << "by a sequence");
		addPathNode(to, e, it.second, conflictEdges, configs, true);
		//print_graph(to, original_df);
	}
	print_graph(to, original_df);

	//resolve cnflicts for all the  (a) sources that sent data to multiple nodes. 
	//				(b) destinations that receive data from multiple nodes.
	//				(c) nodes that correspond to the same coniguration
	//Use [1, origV] as it denotes the list of nodes in the original SDF
	int origV = (int)dataflow->getVerticesCount();
	for(int i = 1; i <= origV; i++)
	{
		auto src = to->getVertexById(i);
		resolveSrcConflicts(to, src, origV);
	}
	print_graph(to, original_df);

	for(int i = 1; i <= origV; i++)
	{
		auto dest = to->getVertexById(i);
		resolveDestConflicts(to, dest, origV);
	}
	print_graph(to, original_df);

	//if (param_list.count("skip_merge") != 1)
	//if (false)
	VERBOSE_INFO ("Call mergeConfigNodes");
	for(conflictConfigs::iterator it = configs.begin(); it != configs.end(); it++)
		mergeConfigNodes(to, it->first, it->second);

	print_graph(to, original_df);
	//Remove conflicts at source and destination router links as a big node has been created
	for(auto it:routes)
	{
		auto start_id = routes[it.first][0];
		auto cf_it = conflictEdges.find((unsigned int)start_id);
		if(cf_it != conflictEdges.end())
	 		conflictEdges.erase(cf_it);

		auto mysize = routes[it.first].size() - 1;
		auto end_id = routes[it.first][mysize];
		auto cf_it2 = conflictEdges.find((unsigned int)end_id);
		if(cf_it2 != conflictEdges.end())
	 		conflictEdges.erase(cf_it2);
	}

	VERBOSE_INFO("resolving conflicts done");
	//Original graph


	// This removes the vertices disconnected
	for (bool removeme = true; removeme; ) {
		{ForEachVertex(to,v) {

			if (to->getVertexDegree(v) == 0) {
				VERBOSE_INFO ( " I remove one task (" << to->getVertexId(v) << ",name=" << to->getVertexName(v) << ") lah!");
				to->removeVertex(v);
				//print_graph(to, original_df);
				removeme=true;
				break;
			}
			removeme=false;
		}}
	}

	print_graph(to, original_df);
	//std::cout << printers::GenerateDOT(to);
	//Given the graph "to" the perform the Kperiodic scheduling and get "persched" in return
	//scheduling_t persched = algorithms::scheduling::bufferless_kperiodic_scheduling (to, false, false);
	//scheduling_t persched = algorithms::scheduling::CSDF_KPeriodicScheduling    (to) ;

	//TIME_UNIT throughput;
	scheduling_t persched = algorithms::scheduling::CSDF_KPeriodicScheduling(to);// , DO_BUFFERLESS, throughput) ;
	//scheduling_t persched = algorithms::scheduling::bufferless_kperiodic_scheduling (to, DO_BUFFERLESS, STOP_AT_FIRST, GET_PREVIOUS);

	//VERBOSE_ASSERT(computeRepetitionVector(to),"inconsistent graph");
	//models::Scheduling scheduling_res = algorithms::scheduling::CSDF_KPeriodicScheduling_LP(to, algorithms::scheduling::generateNPeriodicVector(to));
	//TIME_UNIT omega = scheduling_res.getGraphPeriod();
	//std::cout << "Maximum throughput is " << std::scientific << std::setw( 11 ) << std::setprecision( 9 ) <<  1.0 / omega << std::endl;
	//std::cout << "Maximum period     is " << std::fixed << std::setw( 11 ) << std::setprecision( 6 ) << omega   << std::endl;

	VERBOSE_INFO("findHP");
	unsigned long LCM;
	TIME_UNIT HP;
	//findHP(dataflow, to, scheduling_res.getTaskSchedule(), &HP, &LCM);
	findHP(dataflow, to, persched, &HP, &LCM);


	while(true)
	{
		std::vector< mytuple > mergeNodes;
		std::string name;
		mergeNodes = checkForConflicts(conflictEdges, to, HP, persched, dataflow, name);
		if(mergeNodes.size() == 0)
			break;
		mergeConfigNodes(to, name, mergeNodes);
		to->reset_computation();
		persched = algorithms::scheduling::CSDF_KPeriodicScheduling(to);// , DO_BUFFERLESS, throughput) ;
		to->reset_computation();
		print_graph(to, original_df);
	}
	VERBOSE_INFO ( "LCM=" << LCM );
}


//Unused Function
void showstack(std::stack<Vertex> s) 
{
	while (!s.empty())
	{
		std::cout << '\t' << s.top();
		s.pop();
	}
	std::cout << '\n';
}

