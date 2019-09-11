/*
 * kperiodic.cpp
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

//vector stores the cycle length in each of the vertices
std::vector< std::vector<unsigned int> > cycles;
std::vector< std::set<unsigned int> > cycid_per_vtxid;
std::vector< std::vector<unsigned int> > cyclen_per_vtxid;

struct node{
	ARRAY_INDEX index;
};

typedef std::map< unsigned int, std::vector< std::pair<Vertex, Vertex> > > conflictEtype;

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

	std::cout << "PROGRAMORDER=";
	for(unsigned int i = 0; i < prog_order.size(); i++)
		std::cout << prog_order[i] << " ";
	std::cout << "\n";

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


void printVector(std::vector<Vertex>& traversal, models::Dataflow* to)
{
	for(unsigned int trav_i = 0; trav_i < traversal.size(); trav_i++)
		std::cout << to->getVertexId(traversal[trav_i]) << " ";
	std::cout << std::endl;
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
	std::cout << "DFS Order: ";

	// Call the recursive helper function to print DFS traversal 
	std::vector<Vertex> traversal;
	DFSUtil(v, visited, to, traversal);

	printVector(traversal, to);
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
	my_dfs(adj,start,visited, start, path, mycycleids);

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
			printVector(traversal, to);

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


void setmap(std::vector<int> paths, std::map<int, int>& curr_util, NoC* noc)
{
	for(unsigned int p_j = 1; p_j < paths.size()-2; p_j++)
	{
		int mapindex = noc->getMapIndex(paths[p_j], paths[p_j+1]);
		if(curr_util.find(mapindex) == curr_util.end())
			curr_util[mapindex] = 0;	
		curr_util[mapindex] += 1;
	}
}


int findPaths(int src, NoC* noc, int core_considered, std::map<int, int>& curr_util, std::map<int, route_t>& store_path, int storepath_id)
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
				if(curr_util.find(mapindex) != curr_util.end())
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
	const int start_core = 5;
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
	std::map<int, int> best_util;
	int core_allocated = -1;

	for (auto core_considered : avail_cores)
	{
		//std::cout << "considering core " << core_considered << "\n";
		int cont_l1 = -1;
		std::map<int, int> curr_util = noc->getLinkUtil();
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
		if(routes.find(it.first) != routes.end())
			std::cout << "already one route is stored fro this\n";
		routes[it.first] = it.second;
	}
} 


//remove the current edge between nodes add intermediate nodes based on the path between them
void taskAndNoCMapping(models::Dataflow* input, models::Dataflow* to, Vertex start, NoC* noc, std::map<int, route_t>& routes, std::vector<ARRAY_INDEX>& prog_order)
{
	int V = to->getVerticesCount();
	std::priority_queue<node> pq;
	std::vector<bool> visited(V+1, false);
	std::vector<int> core_mapping(V+1, -1);

	//list of cores that are available
	std::vector<int> available_cores{5, 6, 10, 9, 8, 4, 0, 1, 2, 3, 7, 11, 15, 14, 13, 12};
	noc->clear();

	auto top = start;
	ARRAY_INDEX endid = to->getVertexId(top);

	node temp2;
	temp2.index = endid;
	visited[endid] = true;
	pq.push(temp2);

	/*for(unsigned int s = 0; s < prog_order.size(); s++)
	{
		auto top = to->getVertexById(prog_order[s]);
		mapping(top, core_mapping, noc, input, available_cores, routes);
	}*/

	for(ARRAY_INDEX s = 1; s <= (ARRAY_INDEX)(V-1); s++)
	{
		prog_order[s] = prog_order[s] + 1 - 1;
		auto top = to->getVertexById(s);
		mapping(top, core_mapping, noc, input, available_cores, routes);
	}

	std::cout << "srjkvr-mapping ";
	for(unsigned int ac_i = 1; ac_i < core_mapping.size()-1; ac_i++)
		std::cout << core_mapping[ac_i] << ",";
	std::cout << "\n";
}


//remove the current edge between nodes
//add intermediate nodes based on the path between them
std::vector<Vertex> addPathNode(models::Dataflow* d, Edge c, route_t list, conflictEtype& returnValue, NoC* noc)
		{
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

	//std::cout << "flow:" << source << "->" << target << ":";
	//for every link in the path, add a corresponding node
	for (auto e : list) {
		// we create a new vertex "middle"
		auto middle = d->addVertex();
		new_vertices.push_back(middle);

		////PLLLEASE DONT CHANGE THE "mid-" value in the name"
		std::stringstream ss;
		ss << "mid-" << source << "," << target << "-" << e;

		std::pair<Vertex, Vertex> pair_temp;
		pair_temp.first = middle;
		pair_temp.second = source_vtx;

		returnValue[(unsigned int)e].push_back(pair_temp);
		d->setVertexName(middle,ss.str());

		d->setPhasesQuantity(middle,1); // number of state for the actor, only one in SDF
		d->setVertexDuration(middle,{1}); // is specify for every state , only one for SDF.
		d->setReentrancyFactor(middle,1); // This is the reentrancy, it avoid a task to be executed more than once at the same time.

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

		int v1_i, v2_i;
		noc->getMapIndexPair((int)e, &v1_i, &v2_i);
		//std::cout << "{" << v1_i << "," << v2_i << "}" << " ";
	}
	//std::cout << "\n";
	//find the final edge
	auto e2 = d->addEdge(source_vtx, target_vtx);
	d->setEdgeOutPhases(e2,{outrate});
	d->setEdgeInPhases(e2,{1});
	d->setPreload(e2,preload);  // preload is M0
	return new_vertices;
		}


//Process the graph for DFS, etc. in this function
std::map<int, route_t> graphProcessing(models::Dataflow* dataflow, NoC* noc, std::vector<ARRAY_INDEX>& prog_order)
		{
	models::Dataflow* to = new models::Dataflow(*dataflow);
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
	taskAndNoCMapping(dataflow, to, start, noc, routes, prog_order);

	return routes;
		}


void addIntermediateNodes(models::Dataflow* dataflow, NoC* noc, conflictEtype& returnValue, std::map<int, route_t>& routes)
{
	std::map<int, Edge> edge_list;
	generateEdgesMap(dataflow, edge_list, noc);

	for(auto it:routes)
		addPathNode(dataflow, edge_list[it.first], it.second, returnValue, noc);
}


void addDependency(models::Dataflow* d, const Vertex vi, const Vertex vj, EXEC_COUNT ni, EXEC_COUNT nj)
{
	LARGE_INT a, b;
	auto gcd_value = gcdExtended((LARGE_INT) ni, (LARGE_INT) nj, &a, &b);

	ni /= gcd_value;
	nj /= gcd_value;
	if (d->is_read_only()) {
		d->set_writable();
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

	std::cout << "Win=" << ni << " Wout=" << nj << " Mo=" << gcd_value << std::endl;

}


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


void resolveDestConflicts(models::Dataflow* d, Vertex src, int origV)
{
	std::vector<Edge> srcedges, rtredges;
	TOKEN_UNIT flow = 0;
	TOKEN_UNIT preload = 0;

	//1. Find all the edges from the vertex to router 
	// and router to next edge in the NoC
	{ForInputEdges(d,src,inE)	{
		auto prev_node = d->getEdgeSource(inE);
		if((int)d->getVertexId(prev_node) > origV && prev_node!= src)
		{
			srcedges.push_back(inE);
			{ForInputEdges(d,prev_node,inE_p)     {
				if(prev_node != d->getEdgeSource(inE_p))
					rtredges.push_back(inE_p);
			}}
		}
	}}

	//1B. Do some sanity check here
	if(srcedges.size() != rtredges.size())
	{
		std::cout << "BIG ERROR in resolveDestConflicts\n";
		exit(0);
	}
	else if(srcedges.size() <= 1)
	{
		//std::cout << "no resolveDestConflicts for vertex " << d->getVertexId(src) << "\n";
		return;
	}

	//1C. Initialize token vecot
	std::vector<TOKEN_UNIT> temp;
	std::vector< std::vector<TOKEN_UNIT> > token_vec (srcedges.size(), temp);

	//2. calculate the preload and flow values of all outgoing edges from source vertices
	//Remove the edges between source and router.
	//Remove the router nodes
	for(int i = 0; i < (int)rtredges.size(); i++)
	{
		for(int j = 0; j < (int)rtredges.size(); j++)
		{
			for(int vi = 0; vi < (int)d->getEdgeOut(rtredges[i]); vi++)
			{
				if (i == j)
					token_vec[j].push_back(1);
				else
					token_vec[j].push_back(0);
			}
		}
		flow += d->getEdgeOut( rtredges[i] );
		preload += d->getPreload( rtredges[i] );	
		std::cout << "states=" << flow << ",preload=" << preload << "\n";
	}
	//std::cout << "done with removing edges\n";
	
	VERBOSE_ASSERT(flow > 0, "This case is not supported: flow between task has to be strictly positive");

	//2B. Create the phase duration and token per phase for the new router node
	std::vector<TOKEN_UNIT> srctoken(flow, 1);
	std::vector<TIME_UNIT> phaseDurVec(flow, 1.0);

	//3. Create the BIG router
	auto middle = d->addVertex();
	std::stringstream ss;
	ss << src << "-D";
	d->setVertexName(middle, ss.str());
	d->setPhasesQuantity(middle, flow); // number of state for the actor, only one in SDF
	d->setVertexDuration(middle, phaseDurVec); // is specify for every state , only one for SDF.
	d->setReentrancyFactor(middle, 1); // This is the reentrancy, it avoid a task to be executed more than once at the same time.
	std::cout << "DST_CONF(ID="<< d->getVertexId(src) << "):Done creating big router node  " << d->getVertexId(middle) << "\n";

	//3B. Add edge between big router and source vertex
	auto srcEdge = d->addEdge(middle,src);
	d->setPreload(srcEdge, preload);
	d->setEdgeOutPhases(srcEdge, {flow});
	d->setEdgeInPhases(srcEdge, srctoken);
	//std::cout << "Done creating big router edge\n";


	//4. Setup the new edges. First remove the edge between router and the next NoC link
	//Setup the flow appropriately
	//Remove the router vertex and the edge between routet to the next NoC link
	for(int i = 0; i < (int)rtredges.size(); i++)
	{
		//(v1)  rtredges[i]  v2
		auto v1 = d->getEdgeSource(rtredges[i]);
		auto v2 = d->getEdgeTarget(rtredges[i]);
		auto loc_preload = d->getPreload(rtredges[i]);

		removeAllEdgesVertex(d, v2);

		auto new_edge = d->addEdge(v1, middle);
		d->setEdgeType(new_edge,EDGE_TYPE::BUFFERLESS_EDGE);
		d->setEdgeOutPhases(new_edge, token_vec[i]);
		d->setEdgeInPhases(new_edge, {1});
		d->setPreload(new_edge,loc_preload);  // preload is M0
	}
	//std::cout << "done with function\n";
}


bool resolveSrcConflicts(models::Dataflow* d, Vertex src, int origV)
{
	std::vector<Edge> srcedges, rtredges;
	TOKEN_UNIT flow = 0;
	TOKEN_UNIT preload = 0;

	//1. Find all the edges from the vertex to router 
	// and router to next edge in the NoC
	{ForOutputEdges(d,src,outE)	{
		auto next_node = d->getEdgeTarget(outE);
		if((int)d->getVertexId(next_node) > origV && next_node!= src)
		{
			srcedges.push_back(outE);
			{ForOutputEdges(d,next_node,inE)     {
				if(next_node != d->getEdgeTarget(inE))
					rtredges.push_back(inE);
			}}
		}
	}}

	//1B. Do some sanity check here
	if(srcedges.size() != rtredges.size())
	{
		std::cout << "BIG ERROR in resolveSrcConflicts\n";
		exit(0);
	}
	else if(srcedges.size() <= 1)
	{
		//std::cout << "no resolveSrcConflicts for vertex " << d->getVertexId(src) << "\n";
		return false;
	}

	//1C. Initialize token vecot
	std::vector<TOKEN_UNIT> temp;
	std::vector< std::vector<TOKEN_UNIT> > token_vec (srcedges.size(), temp);

	//2. calculate the preload and flow values of all outgoing edges from source vertices
	//Remove the edges between source and router.
	//Remove the router nodes
	for(int i = 0; i < (int)srcedges.size(); i++)
	{
		for(int j = 0; j < (int)srcedges.size(); j++)
		{
			for(int vi = 0; vi < (int)d->getEdgeIn(srcedges[i]); vi++)
			{
				if (i == j)
					token_vec[j].push_back(1);
				else
					token_vec[j].push_back(0);
			}
		}
		flow += d->getEdgeIn( srcedges[i] );
		preload += d->getPreload( srcedges[i] );	
		std::cout << "states=" << flow << ",preload=" << preload << "\n";
	}
	std::cout << "done with removing edges\n";

	VERBOSE_ASSERT(flow > 0, "This case is not supported: flow between task has to be strictly positive");

	//2B. Create the phase duration and token per phase for the new router node
	std::vector<TOKEN_UNIT> srctoken(flow, 1);
	std::vector<TIME_UNIT> phaseDurVec(flow, 1.0);

	//3. Create the BIG router
	auto middle = d->addVertex();
	std::stringstream ss;
	ss << src << "-R";
	d->setVertexName(middle, ss.str());
	d->setPhasesQuantity(middle, flow); // number of state for the actor, only one in SDF
	d->setVertexDuration(middle, phaseDurVec); // is specify for every state , only one for SDF.
	d->setReentrancyFactor(middle, 1); // This is the reentrancy, it avoid a task to be executed more than once at the same time.
	std::cout << "SRC_CONF(ID="<< d->getVertexId(src) << "):Done creating big router node  " << d->getVertexId(middle) << "\n";

	//3B. Add edge between source vertex and big router node
	auto srcEdge = d->addEdge(src, middle);
	d->setPreload(srcEdge, preload);
	d->setEdgeInPhases(srcEdge, {flow});
	d->setEdgeOutPhases(srcEdge, srctoken);
	//std::cout << "Done creating big router edge\n";


	//4. Setup the new edges. First remove the edge between router and the next NoC link
	//Setup the flow appropriately
	//Remove the router vertex and the edge between routet to the next NoC link
	for(int i = 0; i < (int)rtredges.size(); i++)
	{
		//(v1)  rtredges[i]  v2
		auto v1 = d->getEdgeSource(rtredges[i]);
		auto v2 = d->getEdgeTarget(rtredges[i]);
		auto loc_preload = d->getPreload(rtredges[i]);

		removeAllEdgesVertex(d, v1);

		auto new_edge = d->addEdge(middle, v2);
		d->setEdgeType(new_edge,EDGE_TYPE::BUFFERLESS_EDGE);
		d->setEdgeInPhases(new_edge, token_vec[i]);
		d->setEdgeOutPhases(new_edge, {1});
		d->setPreload(new_edge,loc_preload);  // preload is M0
	}
	std::cout << "done with function\n";
	return true;
}


void checkForConflicts(conflictEtype& conflictEdges, models::Dataflow* to, TIME_UNIT HP, scheduling_t& persched)
{
	int total_conflict = 0;
	for (auto  key : conflictEdges) {
		auto edge_id = key.first;
		auto mysize = conflictEdges[edge_id].size();
		if(mysize > 1)
		{
			for(unsigned int i = 0; i < mysize; i++)
			{
				for(unsigned int j = 0; j < mysize; j++)
				{
					if (i <= j) //ignore the upper triangle
						continue;
					auto taski = conflictEdges[edge_id][i].first;
					auto taskj = conflictEdges[edge_id][j].first;

					auto ni = to->getNi(taski);
					auto nj = to->getNi(taskj);

					//auto srci = conflictEdges[edge_id][i].second;
					//auto srcj = conflictEdges[edge_id][j].second;

					//auto src_ni = to->getNi(srci);
					//auto src_nj = to->getNi(srcj);

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
								std::cout << "conflict between " << to->getVertexName(taski) << " and " << to->getVertexName(taskj) << "\n";
								total_conflict += 1;
								//addDependency(to, srci, srcj, src_ni, src_nj); //NEED TO RESOLVE SOMEHOW
								break;
							}
						}
					}
				}
			}
		}
	}
	std::cout << "total_conflict=" << total_conflict << "\n";
}


void findHP(models::Dataflow* to, scheduling_t& persched, TIME_UNIT* HP, unsigned long* LCM)
{
	*HP = 0.0;
	*LCM = 1;
	for (auto  key : persched) {
		auto task = key.first;
		*HP =    ( persched[task].first * to->getNi(task) ) / (persched[task].second.size()) ;
		std::cout << "Task " <<  to->getVertexName(task) <<  " : duration=" << to->getVertexTotalDuration(task) <<  " period=" <<  persched[task].first << " HP=" << *HP << " Ni=" << to->getNi(task)<< " starts=[ ";

		*LCM = boost::math::lcm(*LCM, to->getNi(task));

		for (auto  skey : persched[task].second) {

			std::cout << skey << " " ;
		}
		std::cout << "]" << std::endl;

	}
	*LCM = boost::math::lcm(*LCM, (unsigned long)*HP);
}


void algorithms::software_noc(models::Dataflow* const  dataflow, parameters_list_t param_list)
{
	unsigned long LCM;
	TIME_UNIT HP;
	conflictEtype conflictEdges; //stores details of flows that share noc edges

	//Init NoC
	NoC *noc = new NoC(4, 4, 1);
	noc->generateShortestPaths();

	// STEP 0.2 - Assert SDF
	models::Dataflow* to = new models::Dataflow(*dataflow);
	models::Dataflow* to2 = new models::Dataflow(*dataflow);

	//symbolic execution to find program execution order
	std::vector<ARRAY_INDEX> prog_order = symbolic_execution(to2);
	//do some processing to perfrom task mapping, NoC route determination and add intermediate nodes
	std::map<int, route_t> routes = graphProcessing(to, noc, prog_order);
	addIntermediateNodes(to, noc, conflictEdges, routes);

	double xscale  = 1;
	double yscale = 1;
	if (param_list.count("xscale") == 1) xscale = std::stod(param_list["xscale"]);
	if (param_list.count("yscale") == 1) yscale = std::stod(param_list["yscale"]);

	VERBOSE_ASSERT(dataflow,TXT_NEVER_HAPPEND);
	VERBOSE_ASSERT(computeRepetitionVector(to),"inconsistent graph");

	scheduling_t persched =  algorithms::generateKperiodicSchedule   (to , false) ;
	findHP(to, persched, &HP, &LCM);
	checkForConflicts(conflictEdges, to, HP, persched);
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


void algorithms::softwarenoc_bufferless(models::Dataflow* const  dataflow, parameters_list_t   param_list)
{
	{
		models::Dataflow* to = new models::Dataflow(*dataflow);
		VERBOSE_INFO("Generate KVector");
		std::map<Vertex,EXEC_COUNT> kvector;
		{ForEachVertex(to,v) {
			kvector[v] = 1;
			if (param_list.count(to->getVertexName(v)) == 1)
			{
				std::string str_value = param_list[to->getVertexName(v)];
				kvector[v] =  commons::fromString<EXEC_COUNT> ( str_value );
			}
		}}

		scheduling_t persched = algorithms::scheduling::bufferless_scheduling (to,  kvector);
	}
	conflictEtype conflictEdges; //stores details of flows that share noc edges
	NoC *noc = new NoC(4, 4, 1); //Init NoC
	noc->generateShortestPaths();

	// STEP 0.2 - Assert SDF
	models::Dataflow* to = new models::Dataflow(*dataflow);
	models::Dataflow* to2 = new models::Dataflow(*dataflow);

	std::map<int, Edge> edge_list;
	{ForEachVertex(dataflow,v) {
		VERBOSE_INFO("Task " << dataflow->getVertexName(v) << " - mapping="<< dataflow->getMapping(v));
	}}

	//symbolic execution to find program execution order
	std::vector<ARRAY_INDEX> prog_order = symbolic_execution(to2);
	//do some processing to perfrom task mapping, NoC route determination and add intermediate nodes
	std::map<int, route_t> routes = graphProcessing(to, noc, prog_order);
	generateEdgesMap(to, edge_list, noc);

	for(auto it:routes) {
		Edge e = edge_list[it.first];
		Vertex esrc = to->getEdgeSource(e);
		VERBOSE_INFO("replace edge " << e << "by a sequence");
		addPathNode(to, e, it.second, conflictEdges, noc);
	}

/*
	int origV = (int)dataflow->getVerticesCount();

	for(int i = 1; i <= origV; i++)
	{
		auto src = to->getVertexById(i);
		resolveSrcConflicts(to, src, origV);
		resolveDestConflicts(to, src, origV);
	}

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


	// This remove the vertices disconnected
	for (bool removeme = true; removeme; ) {
		{ForEachVertex(to,v) {

			if (to->getVertexDegree(v) == 0) {
				std::cout << " I remove one task (" << to->getVertexId(v) << ") lah!\n";
				to->removeVertex(v);
				removeme=true;
				break;
			}
			removeme=false;

		}}
	}
*/
	//std::cout << printers::GenerateDOT(to);

    // Given the graph "to" the perform the Kperiodic scheduling and get "persched" in return
	scheduling_t persched = algorithms::scheduling::bufferless_kperiodic_scheduling (to);

    // Given "persched", we print the scheduling result.
	{ForEachVertex(to,v) {
		std::cout << "Task " << to->getVertexName(v) << " Period=" << persched[v].first<< " Starts=" << commons::toString(persched[v].second) << std::endl;
	}}


	VERBOSE_INFO("findHP");
	unsigned long LCM;
	TIME_UNIT HP;
	findHP(to, persched, &HP, &LCM);
	checkForConflicts(conflictEdges, to, HP, persched);
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


//Process the graph for DFS, etc. in this function
void postProcessing(models::Dataflow* to, Vertex start, NoC* noc) 
{
	std::cout << "inside post Processing,cycles="<< cycles.size() << "\n";
	std::map<int, Edge> edge_list;
	generateEdgesMap(to, edge_list, noc);

	for(unsigned int i = 0; i < cycles.size(); i++)
	{
		std::vector<unsigned int> temp = cycles[i];

		std::cout << "cycle-" << i << ",len=" << temp.size();
		for(unsigned int j = 0; j < temp.size()-1; j++)
		{
			auto e2 = temp[j];
			auto e1 = temp[j+1];
			int index = noc->getMapIndex(e1,e2);
			int index2 = noc->getMapIndex(e2,e1); 

			//remove edge e1->e2
			if(edge_list.find(index) != edge_list.end())
			{
				std::cout << "removing the edge " << e1 << "->" << e2 << ",breaking\n";
				to->removeEdge(edge_list[index]);
				edge_list.erase(index);
				break;
			}
			else if(edge_list.find(index2) != edge_list.end())
			{
				std::cout << "removing the edge " << e2 << "->" << e1 << ",breaking\n";
				to->removeEdge(edge_list[index2]);
				edge_list.erase(index2);
				break;
			}
			else
				std::cout << "cannot remove " << e1 << "->" << e2 << ",continuing\n";
		}
	}

	{ForEachVertex(to,t) {
		if(t == start)
		{
			continue;
		}
		if(to->getVertexInDegree(t) == 0)
		{
			to->addEdge(start, t);
			std::cout << "adding new edge between start " << to->getVertexId(t) << "\n";
		}
		else
		{
			std::cout << "vertex " << to->getVertexId(t) << ",in_deg=" << to->getVertexInDegree(t) << "\n";
		}
	}}
}



/*
	//Original graph
	std::string inputdot = printers::GenerateDOT (dataflow);
	std::ofstream outfile;
	outfile.open("input.dot");
	outfile << inputdot;
	outfile.close();

	std::cerr << " ================ " <<  to->getName() <<  " ===================== EDGE CONTENT" << std::endl;

	// This is for modulo scheuling
	//Store the current edges list first
	//std::vector<Edge> edges_list;
	//{ForEachEdge(to,e) {
	//	edges_list.push_back(e);
	//}}

 	//add intermediate nodes
	//for(auto e: edges_list) {
	//	route_t list_par = get_route_wrapper(to, e, noc);
	//	addPathNode(to, e, list_par, conflictEdges, noc);
	//}

	std::string outputdot = printers::GenerateDOT (to);

	outfile.open("output.dot");
	outfile << outputdot;
	outfile.close();

	std::cerr << " ================ " <<  to->getName() <<  " ===================== " << std::endl;

	// Note: getEdgeOut and getEdgeIn are Output and input Rates of a buffer
	{ForEachVertex(to,t) {
		std::cerr << " vertex:" << to->getVertexName(t) << ":" << to->getVertexId(t) << std::endl;
		{ForInputEdges(to,t,e){						    
			std::cerr << " in:" << to->getEdgeName(e) << "[" << to->getEdgeOut(e) << "]"  << to->getEdgeId(e) << std::endl;
		}}
		{ForOutputEdges(to,t,e) {
			std::cerr << " out:" << to->getEdgeName(e)  << "[" << to->getEdgeIn(e) << "]"  << to->getEdgeId(e) << std::endl;
		}}
	}}

	std::cout <<  printers::PeriodicScheduling2DOT    (to, persched, false,  xscale , yscale);
 */

/*
	while(!pq.empty())
	{
		top = to->getVertexById(pq.top().index);
		pq.pop();
		if(top != start)
		{
			std::cout << "mapping " << to->getVertexId(top) << "\n";
			mapping(top, core_mapping, noc, input, available_cores, routes);
		}

		{ForOutputEdges(to, top, e){
			Vertex end = to->getEdgeTarget(e);
			ARRAY_INDEX endid = to->getVertexId(end);

			std::cout << "trying node " << endid << "\n";
			if(!visited[endid])
			{
				bool flag = true;
				//{ForInputEdges(to, end, e2){
				//	Vertex e2end = to->getEdgeSource(e2);
				//	ARRAY_INDEX e2endid = to->getVertexId(e2end);
				//	if(!visited[e2endid])//if parent is not executed, cannot map this node
				//		flag = false;
				//}}

				if(flag)
				{
					std::cout << "adding " << endid << " to queue\n";
					node temp;
					temp.index = endid;
					visited[endid] = true;
					pq.push(temp);
				}
			}
		}}
	}
 */
