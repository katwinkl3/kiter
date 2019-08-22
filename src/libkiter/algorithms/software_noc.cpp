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
#include <bits/stdc++.h>
#include <set>
#include <queue>

//vector stores the cycle length in each of the vertices
std::vector< std::vector<unsigned int> > cycles;
std::vector< std::set<unsigned int> > cycid_per_vtxid;
std::vector< std::vector<unsigned int> > cyclen_per_vtxid;

struct node{
   ARRAY_INDEX index;
};

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


LARGE_INT gcdExtended(LARGE_INT x, LARGE_INT y, LARGE_INT *a, LARGE_INT *b)
{
    // Base Case
    if (x == 0)
    {
        *a = 0;
        *b = 1;
        return y;
    }

    LARGE_INT a1, b1; // To store results of recursive call
    LARGE_INT gcd = gcdExtended(y%x, x, &a1, &b1);

    // Update x and y using results of recursive
    // call
    *a = b1 - (y/x) * a1;
    *b = a1;

    return gcd;
}


void showstack(std::stack<Vertex> s) 
{
    while (!s.empty()) 
    { 
        std::cout << '\t' << s.top(); 
        s.pop(); 
    } 
    std::cout << '\n'; 
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
			//std::cout << "found a path, len=" << path.size() << "\n";
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
	//int V = to->getVerticesCount();
	//std::cout << "vertices count: " << V << "\n";

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
			//std::cout << "out: " << t_id << "," << s_id << "\n";
			out.addEdge(out_vtx_map[t_id], out_vtx_map[s_id]);
			//std::cout << "end: \n";
		}}
	}}

	return out;
} 


void fillOrder(Vertex vtx, std::vector<bool>& visited, std::stack<Vertex> &Stack, models::Dataflow* to) 
{ 
	auto v = to->getVertexId(vtx);
	// Mark the current node as visited and print it 
	visited[v] = true; 

	//std::cout << "v=" << v << "\n";
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

		if(cycid_per_vtxid[i].size() > 0)
			std::cout << "vtx=" << i << ":";
		for(auto it = cycid_per_vtxid[i].begin(); it != cycid_per_vtxid[i].end(); it++)
		{
			std::cout << cycles[*it].size() << " ";
			cyclen_per_vtxid[i].push_back( (unsigned int)cycles[*it].size() );
		}
		if(cycid_per_vtxid[i].size() > 0)
		{
			std::cout << "\n";
			sort(cyclen_per_vtxid[i].begin(), cyclen_per_vtxid[i].end());
		}
	}
}


//Process the graph for DFS, etc. in this function
Vertex graphProcessing(models::Dataflow* to)
{
	bool myflag = false;
	bool myflag2 = false;
	Vertex top;

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
		else if(to->getVertexInDegree(t) == 0)
		{
			myflag = true;
			to->addEdge(start, t);
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
	return start;
}



//remove the current edge between nodes add intermediate nodes based on the path between them
void taskAndNoCMapping(models::Dataflow* to, Vertex start)//, Edge c, NoC* noc,  std::map< unsigned int, std::vector< std::pair<Vertex, Vertex> > > & returnValue)
{
	int V = to->getVerticesCount();
	std::priority_queue<node> pq;
	std::vector<bool> visited(V+1, false);

	auto top = start;
	ARRAY_INDEX endid = to->getVertexId(top);
	node temp2;
	temp2.index = endid;
	visited[endid] = true;
	pq.push(temp2);
	std::cout << "adding " << top << "\n";

	while(!pq.empty())
	{
		top = to->getVertexById(pq.top().index);
		pq.pop();

		if(top != start)
			std::cout << "mapping " << to->getVertexId(top) << "\n";
		{ForOutputEdges(to, top, e){
			Vertex end = to->getEdgeTarget(e);
			ARRAY_INDEX endid = to->getVertexId(end);
			if(!visited[endid])
			{
				node temp;
				temp.index = endid;
				visited[endid] = true;
				pq.push(temp);
			}
		}}
	}


	/*
	// We store infos about edge to be deleted
	auto source_vtx = d->getEdgeSource(c);
	auto target_vtx = d->getEdgeTarget(c);

	//Find the core index
	auto source = d->getVertexId(source_vtx)-1; //minus 1 since the nodes start from 1 instead of zero
	auto target = d->getVertexId(target_vtx)-1;

	//Core mapping, modulo scheduling
	source = source % noc->getMeshSize();
	target = target % noc->getMeshSize();

	//use the inrate and route of the edges ans use it when creating the edges
	auto inrate = d->getEdgeIn(c);
	auto outrate = d->getEdgeOut(c);
	auto preload = d->getPreload(c);  // preload is M0

	bool flag = true;
	if (source == target) //ignore this case
		return;

	// we delete the edge
	d->removeEdge(c);

	//for every link in the path, add a corresponding node
	auto list = noc->get_route(source, target);
	for (auto e : list) {
		//std::cout << e << " --> " ;
		// we create a new vertex "middle"
		auto middle = d->addVertex();

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
		}

		d->setEdgeOutPhases(e1,{1}); // and the consumption rate (as many rates as states for the associated task)
		d->setPreload(e1,preload);  // preload is M0

		source_vtx = middle;
	}

	//find the final edge
	auto e2 = d->addEdge(source_vtx, target_vtx);
	d->setEdgeOutPhases(e2,{outrate});
	d->setEdgeInPhases(e2,{1});
	d->setPreload(e2,0);  // preload is M0

	*/
}
















//remove the current edge between nodes
//add intermediate nodes based on the path between them
void addPathNode(models::Dataflow* d, Edge c, NoC* noc,  std::map< unsigned int, std::vector< std::pair<Vertex, Vertex> > > & returnValue)
{
	// We store infos about edge to be deleted
	auto source_vtx = d->getEdgeSource(c);
	auto target_vtx = d->getEdgeTarget(c);

	//Find the core index
	auto source = d->getVertexId(source_vtx)-1; //minus 1 since the nodes start from 1 instead of zero
	auto target = d->getVertexId(target_vtx)-1;

	//Core mapping, modulo scheduling
	source = source % noc->getMeshSize();
	target = target % noc->getMeshSize();

	//use the inrate and route of the edges ans use it when creating the edges
	auto inrate = d->getEdgeIn(c);
	auto outrate = d->getEdgeOut(c);
	auto preload = d->getPreload(c);  // preload is M0

	bool flag = true;
	if (source == target) //ignore this case
		return;

	// we delete the edge
	d->removeEdge(c);

	//for every link in the path, add a corresponding node
	auto list = noc->get_route(source, target);
	for (auto e : list) {
		//std::cout << e << " --> " ;
		// we create a new vertex "middle"
		auto middle = d->addVertex();

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
		}

		d->setEdgeOutPhases(e1,{1}); // and the consumption rate (as many rates as states for the associated task)
		d->setPreload(e1,preload);  // preload is M0

		source_vtx = middle;
	}

	//find the final edge
	auto e2 = d->addEdge(source_vtx, target_vtx);
	d->setEdgeOutPhases(e2,{outrate});
	d->setEdgeInPhases(e2,{1});
	d->setPreload(e2,0);  // preload is M0
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


void algorithms::software_noc(models::Dataflow* const  dataflow, parameters_list_t param_list)
{
	std::map< unsigned int, std::vector< std::pair<Vertex, Vertex> > > conflictEdges; //stores details of flows that share noc edges
	int total_conflict = 0;

	//Init NoC
    	NoC *noc = new NoC(4, 4, 1);
	noc->generateShortestPaths();

	// STEP 0.2 - Assert SDF
	models::Dataflow* to = new models::Dataflow(*dataflow);
	models::Dataflow* grProc = new models::Dataflow(*dataflow); //variable used for intermediate graph processing

	//stub start srjkvr
	Vertex start = graphProcessing(grProc);
	taskAndNoCMapping(grProc, start);

	exit(0);
	//stub end srjkvr

	double xscale  = 1;
	double yscale = 1;
	if (param_list.count("xscale") == 1) xscale = std::stod(param_list["xscale"]);
	if (param_list.count("yscale") == 1) yscale = std::stod(param_list["yscale"]);

	VERBOSE_ASSERT(dataflow,TXT_NEVER_HAPPEND);

	//Original graph
	std::string inputdot = printers::GenerateDOT (dataflow);
	std::ofstream outfile;
	outfile.open("input.dot");
	outfile << inputdot;
	outfile.close();

	std::cerr << " ================ " <<  to->getName() <<  " ===================== EDGE CONTENT" << std::endl;
	//Store the current edges list first
	std::vector<Edge> edges_list;
	{ForEachEdge(to,e) {
		std::cerr << to->getVertexId(to->getEdgeSource(e)) << "->" << to->getVertexId(to->getEdgeTarget(e)) << ":name:" << to->getEdgeName(e) << "[" << to->getEdgeIn(e) << "]" << "[" << to->getEdgeOut(e) << "]" << to->getEdgeId(e) << std::endl;
		edges_list.push_back(e);
	}}

 	//add intermediate nodes
	for(auto e: edges_list) {
		addPathNode(to, e, noc,conflictEdges);
	}

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

	VERBOSE_ASSERT(computeRepetitionVector(to),"inconsistent graph");
	// To fix: RUN 

	std::map<Vertex,std::pair<TIME_UNIT,std::vector<TIME_UNIT>>>  persched =  algorithms::generateKperiodicSchedule   (to , false) ;
	std::cerr << "Size = "<< persched.size() << std::endl;
	TIME_UNIT HP = 0.0;
	unsigned long LCM = 1;
	for (auto  key : persched) {
		auto task = key.first;
		//TIME_UNIT
		HP =    ( persched[task].first * to->getNi(task) ) / ( persched[task].second.size() *  to->getPhasesQuantity(task)) ;
		std::cout << "Task " <<  to->getVertexName(task) <<  " : duration=" <<  to->getVertexDuration(task) <<  " period=" <<  persched[task].first << " HP=" << HP << " Ni=" << to->getNi(task)<< " starts=[ ";

		LCM = boost::math::lcm(LCM, to->getNi(task));

		for (auto  skey : persched[task].second) {

			std::cout << skey << " " ;
		}
		std::cout << "]" << std::endl;

	}

	LCM = boost::math::lcm(LCM, (unsigned long)HP);
	std::cout << "LCM=" << LCM << "\n";

	std::cout <<  printers::PeriodicScheduling2DOT    (to, persched, false,  xscale , yscale);

	unsigned long max_router_size = 0;
	for (auto  key : conflictEdges) {
		auto edge_id = key.first;
		auto mysize = conflictEdges[edge_id].size();
		max_router_size = std::max(max_router_size, mysize);
		if(mysize > 1)
		{
			//std::cout << "potential conflict=" << mysize << "\n";
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

					auto srci = conflictEdges[edge_id][i].second;
					auto srcj = conflictEdges[edge_id][j].second;

					auto src_ni = to->getNi(srci);
					auto src_nj = to->getNi(srcj);

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

							if( isConflictPresent((LARGE_INT) HP, si, (LARGE_INT) ni, sj, (LARGE_INT) nj) )
							{
								std::cout << "conflict between " << to->getVertexName(taski) << " and " << to->getVertexName(taskj) << "\n";
								total_conflict += 1;

								addDependency(to, srci, srcj, src_ni, src_nj);

								// create a buffer 
								// the 
								break;
							}
						}
					}
				}
			}
		}
	}


	std::cout << "max_router_size=" << max_router_size << "\n";
	std::cout << "GGGGGGGGGGGG\n";


	std::cout << "total_conflict=" << total_conflict << "\n";
	to->reset_repetition_vector();
	VERBOSE_ASSERT(computeRepetitionVector(to),"inconsistent graph");

	std::map<Vertex,std::pair<TIME_UNIT,std::vector<TIME_UNIT>>>  persched2 =  algorithms::generateKperiodicSchedule   (to , false) ;
	for (auto  key : persched2) {
		auto task = key.first;
		//TIME_UNIT
		HP =    ( persched2[task].first * to->getNi(task) ) / ( persched2[task].second.size() *  to->getPhasesQuantity(task)) ;
		std::cout << "Task " <<  to->getVertexName(task) <<  " : duration=" <<  to->getVertexDuration(task) <<  " period=" <<  persched2[task].first << " HP=" << HP << " Ni=" << to->getNi(task)<< " starts=[ ";

		for (auto  skey : persched2[task].second) {

			std::cout << skey << " " ;
		}
		std::cout << "]" << std::endl;

	}

	for (auto  key : conflictEdges) {
			auto edge_id = key.first;
			auto mysize = conflictEdges[edge_id].size();
			if(mysize > 1)
			{
				//std::cout << "potential conflict=" << mysize << "\n";
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

						auto srci = conflictEdges[edge_id][i].second;
						auto srcj = conflictEdges[edge_id][j].second;

						auto src_ni = to->getNi(srci);
						auto src_nj = to->getNi(srcj);

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

								if( isConflictPresent((LARGE_INT) HP, si, (LARGE_INT) ni, sj, (LARGE_INT) nj) )
								{
									std::cout << "conflict between " << to->getVertexName(taski) << " and " << to->getVertexName(taskj) << "\n";
									total_conflict += 1;

									addDependency(to, srci, srcj, src_ni, src_nj);

									// create a buffer
									// the
									break;
								}
							}
						}
					}
				}
			}
		}
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


		/*
	LARGE_INT rhs_term1 = HP * ni; 
	LARGE_INT rhs_term2 = HP * nj;

	//LARGE_INT cj, ci, soln_j, soln_i; //constant values that need to be found
		//first divide by gcd to simplify the equation
		lhs /= rhs_gcd;
		rhs_term1 /= rhs_gcd;
		rhs_term2 /= rhs_gcd;

		//find initial solution (cj, ci) for the equation:  cj*a + ci*b = m
		//cj*abs(rhs_term1) + ci*abs(rhs_term2) = gcd( abs(rhs_term1), abs(rhs_term2)) = 1

		//original equation is:
		//cj*abs(rhs_term1) - ci*abs(rhs_term2) = lhs
		std::cout << "new_gcd=" << gcdExtended(myabs(rhs_term1), myabs(rhs_term2), &cj, &ci) << "\n";
		//Apart from multiplying by lhs, Value for cj remains as it is in the original equation, while ci needs to be multiplied by (-1)
		cj = cj * lhs;
		ci = ci * lhs * (-1);

		unsigned int m = 0;
		for(; ; m++)
		{
			//Value for cj remains as it is in the original equation, while ci needs to be multiplied by (-1)
			soln_j = cj + m * rhs_term2;
			soln_i = ci - m * rhs_term1;

			std::cout << "solution is ci=" << soln_i << ",cj=" << soln_j;

			if (soln_i >=0 && soln_i < ni && soln_j >= 0 && soln_j < nj)
			{
				std::cout << ", true\n";
				return true;
			}
			if (soln_i < 0 || soln_j > nj) //break as solution value is decreasing
			{
				std::cout << ", false\n";
				return false;
			}
			std::cout << "\n";
		}
		std::cout << ", false\n";
		return false;
		*/


/*
bool sameset(models::Dataflow* const dataflow, std::set<Edge> *cc1 , std::set<Edge>* cc2) {

    VERBOSE_DEBUG_ASSERT(cc1,"cc1 is not valid");
    VERBOSE_DEBUG_ASSERT(cc2,"cc2 is not valid");

    if (cc1->size() != cc2->size()) return false;

    bool result = false;

    for (std::set<Edge>::iterator it1 = cc1->begin() ; it1 != cc1->end() ; it1++ ) {
        result = false;
        for (std::set<Edge>::iterator it2 = cc2->begin() ; it2 != cc2->end() ; it2++ ) {
            if (dataflow->getEdgeId(*it1) == dataflow->getEdgeId(*it2)) result = true;
        }
        if (result == false) return false;
    }
    return true;
}



std::string cc2string  (models::Dataflow* const dataflow,std::set<Edge>* cc) {
    std::ostringstream returnStream;
    for (std::set<Edge>::iterator it = cc->begin() ; it != cc->end() ; it++ ) {
        returnStream <<  dataflow->getEdgeName(*it) <<
	  " (" << dataflow->getVertexName(dataflow->getEdgeSource(*it)) << "->"
	       << dataflow->getVertexName(dataflow->getEdgeTarget(*it)) << "), ";
    }
    return returnStream.str();

}

std::string print_schedule (models::EventGraph* eg, models::Dataflow* const  dataflow,  std::map<Vertex,EXEC_COUNT> & kvector , TIME_UNIT res ) {
	std::ostringstream returnStream;

    TIME_UNIT SCHEDULING_SIZE = 26;

    eg->computeStartingTime (res);
    TIME_UNIT omega = 1 / res ;
    returnStream << "\\begin{scheduling}{" << dataflow->getVerticesCount() <<  "}{" << SCHEDULING_SIZE <<  "}{3.2}{5}" << std::endl;

    {ForEachVertex(dataflow,v) {
    	returnStream << "\\taskname{"  << dataflow->getVertexId(v) <<  "}{"  << dataflow->getVertexName(v) <<  "}" << "% ki=" << kvector[v] << std::endl;
    }}

    {ForEachEvent(eg,e) {
        models::SchedulingEvent se = eg->getEvent(e);
        EXEC_COUNT ti = se.getTaskId();
        EXEC_COUNT tp = se.getTaskPhase();
        TIME_UNIT start = eg->getStartingTime(e);
        Vertex v = dataflow->getVertexById(ti);
        TIME_UNIT duration = dataflow->getVertexDuration(v,tp);
        if (start + duration <= SCHEDULING_SIZE){
        	returnStream << "\\addexecution[premier]{"  << ti <<  "}{$"  << dataflow->getVertexName(v) <<  "_"  << tp <<  "$}{"  << duration <<  "}{"  << start <<  "}" << std::endl;
        }
    }}

    {ForEachEvent(eg,e) {
        models::SchedulingEvent se = eg->getEvent(e);
        EXEC_COUNT ti = se.getTaskId();
        EXEC_COUNT tp = se.getTaskPhase();
        TIME_UNIT start = eg->getStartingTime(e);
        Vertex v = dataflow->getVertexById(ti);
        TIME_UNIT duration = dataflow->getVertexDuration(v,tp);
        TIME_UNIT period = kvector[v] *  dataflow->getPhasesQuantity(v) * omega / dataflow->getNi(v);
        TIME_UNIT iteration = 0;
        while ((start + period * (iteration + 1) + duration) <= SCHEDULING_SIZE) {
            iteration += 1;
            //returnStream << "\\addexecution[suivant]{"  << ti <<  "}{$"  << dataflow->getVertexName(v) <<  "_"  << tp <<  "$}{"  << duration <<  "}{"  << start + period * iteration <<  "}" << std::endl;
        }
        if (iteration > 0) {
        	returnStream << "\\addperiodictask[suivant]{"  << ti <<  "}{$"  << dataflow->getVertexName(v) <<  "_"  << tp <<  "$}{"  << duration <<  "}{"  << start + period <<  "}{"  << period  <<  "}{"  << iteration - 1 <<   "}" << std::endl;
        }
    }}

    returnStream << "\\end{scheduling}"  << std::endl;
    return returnStream.str();
}
void print_function    (models::Dataflow* const  dataflow,  std::map<Vertex,EXEC_COUNT> & kvector , TIME_UNIT res , bool printXML = false, bool printTikz = false , bool printSchedule = false ) {


    //STEP 1 - Generate Event Graph
    models::EventGraph* eg = algorithms::generateKPeriodicEventGraph(dataflow,&kvector);

    if (printXML)      std::cout << eg->printXML();
    if (printTikz)     std::cout << eg->printTikz();
    if (printSchedule) std::cout << print_schedule(eg,dataflow,kvector,res);

}
void algorithms::print_kperiodic_scheduling    (models::Dataflow* const  dataflow, parameters_list_t param_list) {


    EXEC_COUNT iteration_count = 0;
    VERBOSE_ASSERT(dataflow,TXT_NEVER_HAPPEND);

    // STEP 1 - generate 1-periodic schedule
    std::map<Vertex,EXEC_COUNT> kvector;
    {ForEachVertex(dataflow,v) {
        kvector[v] = 1;
        if (param_list.count(dataflow->getVertexName(v)) == 1) {
            std::string str_value = param_list[dataflow->getVertexName(v)];
            kvector[v] =  commons::fromString<EXEC_COUNT> ( str_value );
        }
    }}
    std::pair<TIME_UNIT, std::set<Edge> > result = KSchedule(dataflow,&kvector);
    print_function    ( dataflow, kvector , result.first , true, true, true);
    VERBOSE_INFO("   Critical circuit is " << cc2string(dataflow,&(result.second)) <<  "");

    if (result.second.size() != 0) {


        while (true) {
            iteration_count++;
            updateVectorWithLocalNi(dataflow,&kvector,&(result.second));
            std::pair<TIME_UNIT, std::set<Edge> > resultprime = KSchedule(dataflow,&kvector);
            print_function    ( dataflow, kvector , resultprime.first , true, true, true);
            VERBOSE_INFO("   Critical circuit is " << cc2string(dataflow,&(resultprime.second)) <<  "");
            if (sameset(dataflow,&(resultprime.second),&(result.second)))  {
                VERBOSE_INFO("Critical circuit is the same");
                result = resultprime;
                break;
            }
            result = resultprime;
            VERBOSE_INFO("Current K-periodic throughput (" << result.first <<  ") is not enough.");
        }

    } {
        iteration_count++;
    }

    VERBOSE_INFO( "K-periodic schedule - iterations count is " << iteration_count);


    EXEC_COUNT total_ni = 0;
    EXEC_COUNT total_ki = 0;
    {ForEachVertex(dataflow,t) {
        total_ni += dataflow->getNi(t);
        total_ki += kvector[t];
    }}

    VERBOSE_INFO("K-periodic schedule - total_ki=" << total_ki << " total_ni=" << total_ni );
    TIME_UNIT res = result.first;
    std::cout << "Maximum throughput is " << std::scientific << std::setw( 11 ) << std::setprecision( 9 ) <<  res   << std::endl;
    std::cout << "Maximum period     is " << std::fixed << std::setw( 11 ) << std::setprecision( 6 ) << 1.0/res   << std::endl;


}

std::pair<TIME_UNIT, std::set<Edge> > algorithms::KSchedule(models::Dataflow *  const dataflow ,std::map<Vertex,EXEC_COUNT> * kvector , TIME_UNIT bound )  {

    std::pair<TIME_UNIT, std::set<Edge> > result;

    // STEP 0.1 - PRE
    VERBOSE_ASSERT(dataflow,TXT_NEVER_HAPPEND);
    VERBOSE_ASSERT(computeRepetitionVector(dataflow),"inconsistent graph");
    //VERBOSE_ASSERT( algorithms::normalize(dataflow),"inconsistent graph");


    VERBOSE_INFO("KPeriodic EventGraph generation");

    //STEP 1 - Generate Event Graph
    models::EventGraph* eg = generateKPeriodicEventGraph(dataflow,kvector);

    VERBOSE_INFO("KPeriodic EventGraph generation Done, edges = " << eg->getConstraintsCount() << " vertex = " << eg->getEventCount());

    //STEP 2 - resolve the MCRP on this Event Graph
    std::pair<TIME_UNIT,std::vector<models::EventGraphEdge> > howard_res = (bound > 0) ? eg->MinCycleRatio(bound) : eg->MinCycleRatio();

    std::vector<models::EventGraphEdge> * critical_circuit = &(howard_res.second);

    //STEP 3 - convert CC(eg) => CC(graph)
    VERBOSE_DEBUG("Critical circuit is about " << critical_circuit->size() << " edges.");
    for (std::vector<models::EventGraphEdge>::iterator it = critical_circuit->begin() ; it != critical_circuit->end() ; it++ ) {
        VERBOSE_DEBUG("   -> " << eg->getChannelId(*it) << " : " << eg->getSchedulingEvent(eg->getSource(*it)).toString() << " to " <<  eg->getSchedulingEvent(eg->getTarget(*it)).toString() <<  " = (" << eg->getConstraint(*it)._w << "," << eg->getConstraint(*it)._d << ")" );
        ARRAY_INDEX channel_id = eg->getChannelId(*it);
        try {
            Edge        channel    = dataflow->getEdgeById(channel_id);
            result.second.insert(channel);
        } catch(...) {
            VERBOSE_DEBUG("      is loopback");
        }
    }


    TIME_UNIT frequency = howard_res.first;

    // return the best omega found in sdf3 way
    VERBOSE_INFO("KSchedule function get " << frequency << " from MCRP." );
    VERBOSE_INFO("  ->  then omega =  " <<  1 / frequency );
    //VERBOSE_INFO("  -> considering task " << dataflow->getVertexName(first) << ", mu_t = " << mui );
    //VERBOSE_INFO("  -> then sdf3 normalize frequency is " << thg );


    //result.first = thg;
    result.first = frequency;
    delete eg;

    return result;
}



models::EventGraph* algorithms::generateKPeriodicEventGraph(models::Dataflow * const dataflow , std::map<Vertex,EXEC_COUNT> * kValues ) {


    VERBOSE_ASSERT(computeRepetitionVector(dataflow),"inconsistent graph");

    models::EventGraph * g = new models::EventGraph();

    VERBOSE_DEBUG_ASSERT(dataflow->is_normalized() == false,"looser");

    // generate nodes
    {ForEachVertex(dataflow,t) {
        EXEC_COUNT phase_count = dataflow->getPhasesQuantity(t);
        for (EXEC_COUNT j = 1; j <= kValues->at(t) ; j++ ) {
            for (EXEC_COUNT i = 1; i <= phase_count ; i++ ) {
                g->addEvent(models::SchedulingEvent(dataflow->getVertexId(t),i,j));
            }
        }
    }}


    // DEFINITION DES REENTRANCES

    {ForEachVertex(dataflow,pTask) {
        EXEC_COUNT start_count = kValues->at(pTask);
        generateKperiodicSelfloop(dataflow,start_count,g,pTask);
    }}


    // DEFINITION DES CONTRAINTES DE PRECEDENCES
    {ForEachChannel(dataflow,c) {
        VERBOSE_INFO("one more edge ... ");
        generateKPeriodicConstraint(dataflow , kValues,   g ,  c);
    }}



    return g;



}

void algorithms::print_NKperiodic_eventgraph    (models::Dataflow* const  dataflow, parameters_list_t) {

    VERBOSE_ASSERT(dataflow,TXT_NEVER_HAPPEND);

    // STEP 0 - CSDF Graph should be normalized
    VERBOSE_ASSERT(computeRepetitionVector(dataflow),"inconsistent");


    // STEP 0.2
    std::map<Vertex,EXEC_COUNT> kvector;
    {ForEachVertex(dataflow,t) {
        kvector[t] = dataflow->getNi(t);
    }}
    // STEP 1 - Generate Event Graph
    models::EventGraph* eg = algorithms::generateKPeriodicEventGraph(dataflow,&kvector);
    // STEP 2 - print it


    std::cout << eg->printXML();


}

void algorithms::print_2periodic_eventgraph    (models::Dataflow* const  dataflow, parameters_list_t) {

    VERBOSE_ASSERT(dataflow,TXT_NEVER_HAPPEND);

    // STEP 0 - CSDF Graph should be normalized
    VERBOSE_ASSERT(computeRepetitionVector(dataflow),"inconsistent");


    // STEP 0.2
    std::map<Vertex,EXEC_COUNT> kvector;
    {ForEachVertex(dataflow,t) {
        kvector[t] = 2;
    }}
    // STEP 1 - Generate Event Graph
    models::EventGraph* eg = algorithms::generateKPeriodicEventGraph(dataflow,&kvector);
    // STEP 2 - print it


    std::cout << eg->printXML();


}


void algorithms::print_1periodic_eventgraph    (models::Dataflow* const  dataflow, parameters_list_t) {

    VERBOSE_ASSERT(dataflow,TXT_NEVER_HAPPEND);

    // STEP 0 - CSDF Graph should be normalized
    VERBOSE_ASSERT(computeRepetitionVector(dataflow),"inconsistent");


    // STEP 0.2
    std::map<Vertex,EXEC_COUNT> kvector;
    {ForEachVertex(dataflow,t) {
        kvector[t] = 1;
    }}
    // STEP 1 - Generate Event Graph
    models::EventGraph* eg = algorithms::generateKPeriodicEventGraph(dataflow,&kvector);
    // STEP 2 - print it


    std::cout << eg->printXML();


}



bool algorithms::updateVectorWithFineNi( models::Dataflow * const dataflow , std::map<Vertex,EXEC_COUNT> * oldkvector, std::set<Edge>* cc ) {

    VERBOSE_ASSERT(dataflow,"error");
    VERBOSE_ASSERT(oldkvector,"error");
    VERBOSE_ASSERT(cc,"error");

    std::map<Vertex,EXEC_COUNT>  kvector ;
    for ( std::map<Vertex,EXEC_COUNT>::iterator it = oldkvector->begin() ; it != oldkvector->end(); it++ ) {
        kvector[it->first] = it->second;
    }

    // STEP 1 = GENERATE OLD KI AND NEW KI VECTORS == GENERATE addconstraint

    if (!updateVectorWithLocalNi(dataflow,&kvector,cc)) return false;

    models::EventGraph*  cycle_eg =  generateCycleOnly( dataflow , &kvector,   cc ) ;
    //
    std::pair<TIME_UNIT,std::vector<models::EventGraphEdge> > howard_res = cycle_eg->MinCycleRatio();
    VERBOSE_INFO("With this cycle the new bound is " << howard_res.first);


    for ( std::map<Vertex,EXEC_COUNT>::iterator it = oldkvector->begin() ; it != oldkvector->end(); it++ ) {
        oldkvector->at(it->first) = kvector.at(it->first);
    }

    delete cycle_eg;

    return true;

}








models::EventGraph* algorithms::updateEventGraph( models::Dataflow * const dataflow , std::map<Vertex,EXEC_COUNT> * oldkvector, std::set<Edge>* cc, models::EventGraph* g, bool verbose = false) {



    VERBOSE_ASSERT(dataflow,"error");
    VERBOSE_ASSERT(oldkvector,"error");
    VERBOSE_ASSERT(g,"error");

    EXEC_COUNT addconstraint = 0;

    // Compute old and new vector


    VERBOSE_INFO("Update event graph - Step 0 - Compute new K and check changed");

    std::map<Vertex,EXEC_COUNT>  kvector ;
    for ( std::map<Vertex,EXEC_COUNT>::iterator it = oldkvector->begin() ; it != oldkvector->end(); it++ ) {
        kvector[it->first] = it->second;
    }

    // STEP 1 = GENERATE OLD KI AND NEW KI VECTORS == GENERATE addconstraint


    bool changed = updateVectorWithLocalNi(dataflow,&kvector,cc);

    if (!changed) {
        VERBOSE_INFO("Unchanged ...");
        return NULL;
    }


    VERBOSE_INFO("Update event graph - Step 1 - Delete edges and add task");
    // STEP 1
    //remove all connected edges
    EXEC_COUNT total = cc->size();
    EXEC_COUNT current = 0;
    for (std::set<Edge>::iterator it = cc->begin() ; it != cc->end(); it++ ) {
        current ++ ;
        if (verbose) {std::cout << "\rdelete edges ... " << std::setw(3) << ((current* 100)  / (total))   << "%" << std::setw( 11 ) << " ";std::cout.flush();}
        Vertex source = dataflow->getEdgeSource(*it);
        EXEC_COUNT ki = oldkvector->at(source);
        EXEC_COUNT newki =  kvector.at(source);
        if (ki < newki) {
            ARRAY_INDEX task_id= dataflow->getVertexId(source);
            for (EXEC_COUNT p = 1 ; p <= dataflow->getPhasesQuantity(source) ; p++) {
                for(ARRAY_INDEX k = 1 ; k <= ki ; k++) {
                    g->removeConnectedEdges(g->getEventGraphVertex(task_id,p,k));
                }
            }

            for (EXEC_COUNT p = 1 ; p <= dataflow->getPhasesQuantity(source) ; p++) {
                g->addEventGroup(task_id,p,ki+1,newki);
            }
            if (newki > dataflow->getNi(source)) {
                VERBOSE_ERROR("newki=" << newki << " Ni=" << dataflow->getNi(source));
            }
            VERBOSE_ASSERT(newki <= dataflow->getNi(source),"Bad idea !");
        }
    }



    VERBOSE_INFO("Update event graph - Step 2 - Reentrancy");

    current = 0;

    for (std::set<Edge>::iterator it = cc->begin() ; it != cc->end(); it++ ) {
        if (verbose) {std::cout << "\rreentrency edges ... " << std::setw(3) << ((current* 100)  / (total) ) << "%" << std::setw( 11 ) << " ";std::cout.flush();}
        current ++ ;
        Vertex t = dataflow->getEdgeSource(*it);

        const EXEC_COUNT oldki =  oldkvector->at(t);
        const EXEC_COUNT newki =     kvector.at(t);

        // SKIP IF KI NOT CHANGED (no loopback to create, because loopback not deleted)
        if (oldki == newki) continue;




        VERBOSE_DEBUG("generate reentrancy loops for task " <<  t << " with newki=" << newki);
        EXEC_COUNT start_count = kvector.at(t);
        generateKperiodicSelfloop(dataflow,start_count,g,t);

    }


    VERBOSE_INFO("Update event graph - Step 3 - add " << addconstraint << " constraints.");

    // STEP 3
    // add all edges

    current = 0;
    for (std::set<Edge>::iterator it = cc->begin() ; it != cc->end() ; it++) {
        if (verbose) {std::cout << "\rconstraint edges ... " << std::setw(3) << ((current* 100)  / (total) )  << "% (" << std::setw( 11 ) << total << " edges)";std::cout.flush();}
        current ++ ;
        std::set<Edge>::iterator it_previous = it;
        if (it == cc->begin())  {
            it_previous = cc->end();
        }
        it_previous--;
        Edge current = *it;
        Edge previous = *it_previous;
        Vertex source = dataflow->getEdgeSource(current);
        Vertex target = dataflow->getEdgeTarget(current);
        const EXEC_COUNT oldki =  oldkvector->at(source);
        const EXEC_COUNT newki =     kvector.at(source);
        const EXEC_COUNT oldkj =  oldkvector->at(target);
        const EXEC_COUNT newkj =     kvector.at(target);

        // SKIP IF KI NOT CHANGED (no loopback to create, because loopback not deleted
        if (oldki == newki) continue;

        {ForOutputEdges(dataflow,source,c) {
            if ((c != current) || (oldkj == newkj)) {
                generateKPeriodicConstraint(dataflow , &kvector,   g ,  c);
            }

        }}

        {ForInputEdges(dataflow,source,c) {
            if (c != previous) {
                generateKPeriodicConstraint(dataflow , &kvector,   g ,  c);
            } else {
                generateKPeriodicConstraint(dataflow , &kvector,   g ,  c); //speed up here only
            }
        }}
    }


    for ( std::map<Vertex,EXEC_COUNT>::iterator it = oldkvector->begin() ; it != oldkvector->end(); it++ ) {
        oldkvector->at(it->first) = kvector.at(it->first);
    }

    if (verbose) {std::cout << "End of generation" << std::setw(30)  << "\r";std::cout.flush();}


    return g;

}






models::EventGraph*  algorithms::generateCycleOnly(models::Dataflow * const dataflow , std::map<Vertex,EXEC_COUNT> * kValues,   std::set<Edge> * cc ) {


    VERBOSE_ASSERT(computeRepetitionVector(dataflow),"inconsistent graph");

    models::EventGraph * g = new models::EventGraph();

    VERBOSE_DEBUG_ASSERT(dataflow->is_normalized() == false,"looser");


    // generate nodes
    for (std::set<Edge>::iterator it = cc->begin() ; it != cc->end(); it++ ) {
        Vertex pTask = dataflow->getEdgeSource(*it);
        EXEC_COUNT start_count = kValues->at(pTask);
        {ForEachPhase(dataflow,pTask,p) {
            VERBOSE_DEBUG("generate " << start_count << " node for task " << dataflow->getVertexId(pTask) << " phase " << p);
            g->addEventGroup(dataflow->getVertexId(pTask),p,start_count);
        }}


    }


    for (std::set<Edge>::iterator it = cc->begin() ; it != cc->end(); it++ ) {
        Vertex pTask = dataflow->getEdgeSource(*it);
        EXEC_COUNT start_count = kValues->at(pTask);
        generateKperiodicSelfloop(dataflow,start_count,g,pTask);

    }




    // DEFINITION DES CONTRAINTES DE PRECEDENCES
    //


    for (std::set<Edge>::iterator it = cc->begin() ; it != cc->end(); it++ ) {
        generateKPeriodicConstraint(dataflow , kValues,   g ,  *it);
    }




    return g;

}



void algorithms::generateKPeriodicConstraint(models::Dataflow * const dataflow , std::map<Vertex,EXEC_COUNT> * kValues,  models::EventGraph* g , Edge c) {


    VERBOSE_DEBUG("Constraint for " << dataflow->getEdgeName(c) );

    ARRAY_INDEX id = dataflow->getEdgeId(c);

    Vertex source = dataflow->getEdgeSource(c);
    Vertex target = dataflow->getEdgeTarget(c);

    const ARRAY_INDEX source_id = dataflow->getVertexId(source);
    const ARRAY_INDEX target_id = dataflow->getVertexId(target);

    EXEC_COUNT source_phase_count = dataflow->getEdgeInPhasesCount(c);
    EXEC_COUNT target_phase_count = dataflow->getEdgeOutPhasesCount(c);

    const EXEC_COUNT  maxki        = kValues->at(source) ;
    const EXEC_COUNT  maxkj        = kValues->at(target) ;

    TOKEN_UNIT mop     =  commons::floor(dataflow->getPreload(c),dataflow->getFineGCD(c));

    const TOKEN_UNIT  Wc        =  dataflow->getEdgeIn(c) * maxki;
    const TOKEN_UNIT  Rc        =  dataflow->getEdgeOut(c) * maxkj;

    const TOKEN_UNIT  gcdz       = boost::math::gcd((Wc),(Rc));
    const TOKEN_UNIT  stepa      = dataflow->getFineGCD(c);

    TOKEN_UNIT normdapkm1 = 0;
    TOKEN_UNIT normdapk   = 0;



    for (EXEC_COUNT ki = 1; ki <= maxki ; ki++ ) {
        for (EXEC_COUNT pi = 1; pi <= source_phase_count ; pi++ ) {

            TIME_UNIT  d =  dataflow->getVertexDuration(source, pi); // L(a) = l(ti)
            TOKEN_UNIT normdamkp = 0;
            const TOKEN_UNIT wak        = dataflow->getEdgeInPhase(c,pi)   ;
            normdapk += wak;
            models::EventGraphVertex source_event = g->getEventGraphVertex(source_id,pi,ki);


            for (EXEC_COUNT kj = 1; kj <= maxkj ; kj++ ) {
                for (EXEC_COUNT pj = 1; pj <= target_phase_count ; pj++ ) {

                    const TOKEN_UNIT vakp       = dataflow->getEdgeOutPhase(c,pj) ;
                    normdamkp += vakp;

#ifdef NOT_OPTIMIZED
                    const TOKEN_UNIT  alphamin = (TOKEN_UNIT)  commons::ceil(std::max((TOKEN_UNIT)0, wak - vakp) - mop - normdapk + normdamkp,gcdz);
                    const TOKEN_UNIT alphamax  =  commons::floor( - mop - normdapkm1 + normdamkp - stepa ,gcdz);
#else

                    const TOKEN_UNIT lpimax =    ( - mop - normdapkm1 + normdamkp - stepa) ;
                    const TOKEN_UNIT rpimax =     lpimax % gcdz;
                    const TOKEN_UNIT alphamax =   (rpimax >= 0) ? ( lpimax - rpimax ) : ( lpimax - rpimax - gcdz );
                    const TOKEN_UNIT lpimin =    (std::max((TOKEN_UNIT)0, wak - vakp) - mop - normdapk + normdamkp) ;

                    if (lpimin > alphamax ) continue; // ca ne fera qu'empirer

                    const TOKEN_UNIT rpimin =     lpimin % gcdz;
                    const TOKEN_UNIT alphamin =   (rpimin <= 0) ? ( lpimin - rpimin ) : ( lpimin - rpimin + gcdz );
#endif


                    if (alphamin <= alphamax) {

                        models::EventGraphVertex target_event = g->getEventGraphVertex(target_id,pj,kj);
                        VERBOSE_DEBUG("  ki=" << ki<<" kj=" << kj << " (" <<  source_event  << "," << target_event  << ")");
                        VERBOSE_DEBUG("  alphamin=" << alphamin <<"   alphamax=" << alphamax );
                        TIME_UNIT w = ((TIME_UNIT) alphamax * source_phase_count * maxki ) / ( (TIME_UNIT) Wc  * (TIME_UNIT) dataflow->getNi(source) );
                        VERBOSE_DEBUG("   w = (" << alphamax << " * " << dataflow->getPhasesQuantity(source) * maxki << ") / (" << Wc << " * " << dataflow->getNi(source) / maxki << ")");
                        VERBOSE_DEBUG("   d = (" << d << ")");
                        g->addEventConstraint(source_event ,target_event,-w,d,id);
                    }
                }
            }
            normdapkm1 = normdapk;
        }
    }
}


void algorithms::generateKperiodicSelfloop(models::Dataflow * const dataflow , EXEC_COUNT ki,  models::EventGraph* g , Vertex t  ) {

    const ARRAY_INDEX task_id = dataflow->getVertexId(t);

    const TIME_UNIT timefactor = (dataflow->getReentrancyFactor(t) <= 0)?0:1;

    VERBOSE_DEBUG("generate reentrancy loops for task " <<  dataflow->getVertexId(t) << " with ki=" << ki);


    models::EventGraphVertex source_event = g->getEventGraphVertex(task_id,1,1);
    models::EventGraphVertex target_event = g->getEventGraphVertex(task_id,1,1);
    EXEC_COUNT pq = dataflow->getPhasesQuantity(t);
    TIME_UNIT lti = timefactor * dataflow->getVertexDuration(t,1);
    EXEC_COUNT i = 1;
    EXEC_COUNT p = 1;
    while (i <= ki) {
        p += 1;
        if (p > pq) {p = 1; i += 1;}
        if (i > ki) continue;

        target_event = g->getEventGraphVertex(task_id,p,i);

        TIME_UNIT d =  lti;
        TIME_UNIT w =  0;
        g->addEventConstraint(source_event ,target_event,w,d,0);

        source_event = target_event;
        lti = timefactor * dataflow->getVertexDuration(t,p);
    }






    if (dataflow->getReentrancyFactor(t) > 0) // consider only a factor 1
    {

        // constraintes last_k --> 1

        EXEC_COUNT p = dataflow->getPhasesQuantity(t);
        EXEC_COUNT i = ki;

        EXEC_COUNT np = 1;
        EXEC_COUNT ni = 1;

        const TIME_UNIT lti = timefactor * dataflow->getVertexDuration(t,p);

        models::EventGraphVertex source_event = g->getEventGraphVertex(task_id,p,i);
        models::EventGraphVertex target_event = g->getEventGraphVertex(task_id,np,ni);


        const TIME_UNIT       d     =  lti;
        const TIME_UNIT       w     =   -  (TIME_UNIT) (p * i) / (  (TIME_UNIT) dataflow->getNi(t)  );


        VERBOSE_DEBUG("add task constraint :" << dataflow->getVertexName(t) << " : " <<   target_event  << " --> " <<  source_event << " : " <<  " (- " << w << " / " << dataflow->getNi(t)  << ") ," << d );
        g->addEventConstraint(source_event ,target_event,-w,d,0);
    }



}




void algorithms::updateVectorWithFullNi(models::Dataflow *  const dataflow ,std::map<Vertex,EXEC_COUNT> * kvector , std::set<Edge> * cc)  {

    EXEC_COUNT before = 0;
    EXEC_COUNT after = 0;
    for (std::set<Edge>::iterator it = cc->begin() ; it != cc->end(); it++ ) {
        Vertex source = dataflow->getEdgeSource(*it);
        Vertex target = dataflow->getEdgeTarget(*it);
        before += kvector->at(source) * kvector->at(target);
        kvector->at(source) = dataflow->getNi(source);
        after += dataflow->getNi(source) * dataflow->getNi(target);
    }

    VERBOSE_INFO("   Up complexity from " << before << " to " << after);

}

bool algorithms::updateVectorWithLocalNi(models::Dataflow *  const dataflow ,std::map<Vertex,EXEC_COUNT> * kvector , std::set<Edge> * cc)  {

    bool changed = false;

    EXEC_COUNT gcdNi = 0;
    for (std::set<Edge>::iterator it = cc->begin() ; it != cc->end(); it++ ) {
        Vertex source = dataflow->getEdgeSource(*it);
        gcdNi = boost::math::gcd(gcdNi,dataflow->getNi(source) );
    }

    for (std::set<Edge>::iterator it = cc->begin() ; it != cc->end(); it++ ) {
        Vertex source = dataflow->getEdgeSource(*it);
        Vertex target = dataflow->getEdgeTarget(*it);

        EXEC_COUNT Ni =  dataflow->getNi(source);
        EXEC_COUNT Nj =  dataflow->getNi(target);

        EXEC_COUNT newki = boost::math::lcm( kvector->at(source),  Ni / gcdNi);

        if (newki != kvector->at(source) ) changed = true;

        kvector->at(source) = newki;
        VERBOSE_ASSERT(kvector->at(source) <= Ni , "Bad idea");
        VERBOSE_ASSERT(kvector->at(target) <= Nj , "Bad idea");
    }

    return changed;

}

void algorithms::compute_NKperiodic_throughput            (models::Dataflow* const  dataflow, parameters_list_t) {

    VERBOSE_ASSERT(dataflow,TXT_NEVER_HAPPEND);
    VERBOSE_ASSERT(computeRepetitionVector(dataflow),"inconsistent graph");

    // STEP 0.2 - Assert SDF
    std::map<Vertex,EXEC_COUNT> kvector;
    {ForEachVertex(dataflow,t) {
        kvector[t] = dataflow->getNi(t);
    }}

    std::pair<TIME_UNIT, std::set<Edge> > result = KSchedule(dataflow,&kvector);

    TIME_UNIT res = result.first;
    std::cout << "Maximum throughput is " << std::scientific << std::setw( 11 ) << std::setprecision( 9 ) <<  res   << std::endl;
    std::cout << "Maximum period     is " << std::fixed << std::setw( 11 ) << std::setprecision( 6 ) << 1.0/res   << std::endl;


}



void algorithms::compute_2Kperiodic_throughput            (models::Dataflow* const  dataflow, parameters_list_t) {

    VERBOSE_ASSERT(dataflow,TXT_NEVER_HAPPEND);

    // STEP 0.2 - Assert SDF
    std::map<Vertex,EXEC_COUNT> kvector;
    {ForEachVertex(dataflow,t) {
        kvector[t] = 2;
    }}

    std::pair<TIME_UNIT, std::set<Edge> > result = KSchedule(dataflow,&kvector);
    TIME_UNIT res = result.first;
    std::cout << "Maximum throughput is " << std::scientific << std::setw( 11 ) << std::setprecision( 9 ) <<  res   << std::endl;
    std::cout << "Maximum period     is " << std::fixed << std::setw( 11 ) << std::setprecision( 6 ) << 1.0/res   << std::endl;


}


void algorithms::compute_1Kperiodic_throughput            (models::Dataflow* const  dataflow, parameters_list_t param_list) {

    VERBOSE_ASSERT(dataflow,TXT_NEVER_HAPPEND);

    bool printRequired = (param_list.count("print") == 1);


    // STEP 0.2 - Assert SDF
    std::map<Vertex,EXEC_COUNT> kvector;
    {ForEachVertex(dataflow,t) {
        kvector[t] = 1;
    }}

    std::pair<TIME_UNIT, std::set<Edge> > result = KSchedule(dataflow,&kvector);

    if (printRequired) {
    	print_function    ( dataflow, kvector , result.first , false,false,true);
    } else {
    	TIME_UNIT res = result.first;
    	std::cout << "Maximum throughput is " << std::scientific << std::setw( 11 ) << std::setprecision( 9 ) <<  res   << std::endl;
    	std::cout << "Maximum period     is " << std::fixed << std::setw( 11 ) << std::setprecision( 6 ) << 1.0/res   << std::endl;
    }

}



std::map<Vertex,EXEC_COUNT> algorithms::get_Kvector(models::Dataflow *  const dataflow ) {

    // STEP 0.1 - PRE
    VERBOSE_ASSERT(dataflow,TXT_NEVER_HAPPEND);
    VERBOSE_ASSERT(computeRepetitionVector(dataflow),"inconsistent graph");
    EXEC_COUNT iteration_count = 0;

    // STEP 1 - generate initial vector
    std::map<Vertex,EXEC_COUNT> kvector;
    {ForEachVertex(dataflow,t) {
        kvector[t] = 1;

    }}


    std::pair<TIME_UNIT, std::set<Edge> > result;



    VERBOSE_INFO("KPeriodic EventGraph generation");

    //STEP 1 - Generate Event Graph
    models::EventGraph* eg = generateKPeriodicEventGraph(dataflow,&kvector);


    VERBOSE_INFO("KPeriodic EventGraph generation Done");

    //STEP 2 - resolve the MCRP on this Event Graph
    std::pair<TIME_UNIT,std::vector<models::EventGraphEdge> > howard_res = eg->MinCycleRatio();

    std::vector<models::EventGraphEdge> * critical_circuit = &(howard_res.second);

    //STEP 3 - convert CC(eg) => CC(graph)
    VERBOSE_DEBUG("Critical circuit is about " << critical_circuit->size() << " edges.");
    for (std::vector<models::EventGraphEdge>::iterator it = critical_circuit->begin() ; it != critical_circuit->end() ; it++ ) {
        VERBOSE_DEBUG("   -> " << eg->getChannelId(*it) << " : " << eg->getSchedulingEvent(eg->getSource(*it)).toString() << " to " <<  eg->getSchedulingEvent(eg->getTarget(*it)).toString() <<  " = (" << eg->getConstraint(*it)._w << "," << eg->getConstraint(*it)._d << ")" );
        ARRAY_INDEX channel_id = eg->getChannelId(*it);
        try {
            Edge        channel    = dataflow->getEdgeById(channel_id);
            result.second.insert(channel);
        } catch(...) {
            VERBOSE_DEBUG("      is loopback");
        }
    }

    TIME_UNIT frequency = howard_res.first;

    VERBOSE_INFO("KSchedule function get " << frequency << " from MCRP." );
    VERBOSE_INFO("  ->  then omega =  " <<  1 / frequency );

    result.first = frequency;

    ////////////// SCHEDULE CALL // END

    if (result.second.size() != 0) {

        VERBOSE_INFO("1-periodic throughput (" << result.first <<  ") is not enough.");
        VERBOSE_INFO("   Critical circuit is " << cc2string(dataflow,&(result.second)) <<  "");

        while (true) {
            iteration_count++;
            ////////////// SCHEDULE CALL // BEGIN : resultprime = KSchedule(dataflow,&kvector);

            std::pair<TIME_UNIT, std::set<Edge> > resultprime;

            //VERBOSE_ASSERT( algorithms::normalize(dataflow),"inconsistent graph");
            VERBOSE_INFO("KPeriodic EventGraph generation");

            //STEP 1 - Generate Event Graph and update vector
            if (!updateEventGraph( dataflow ,  &kvector, &(result.second), eg)) break ;

            VERBOSE_INFO("KPeriodic EventGraph generation Done");

            //STEP 2 - resolve the MCRP on this Event Graph
            std::pair<TIME_UNIT,std::vector<models::EventGraphEdge> > howard_res_bis = eg->MinCycleRatio();

            std::vector<models::EventGraphEdge> * critical_circuit = &(howard_res_bis.second);

            //STEP 3 - convert CC(eg) => CC(graph)
            VERBOSE_DEBUG("Critical circuit is about " << critical_circuit->size() << " edges.");
            for (std::vector<models::EventGraphEdge>::iterator it = critical_circuit->begin() ; it != critical_circuit->end() ; it++ ) {
                VERBOSE_DEBUG("   -> " << eg->getChannelId(*it) << " : " << eg->getSchedulingEvent(eg->getSource(*it)).toString() << " to " <<  eg->getSchedulingEvent(eg->getTarget(*it)).toString() <<  " = (" << eg->getConstraint(*it)._w << "," << eg->getConstraint(*it)._d << ")" );
                ARRAY_INDEX channel_id = eg->getChannelId(*it);
                try {
                    Edge        channel    = dataflow->getEdgeById(channel_id);
                    resultprime.second.insert(channel);
                } catch(...) {
                    VERBOSE_DEBUG("      is loopback");
                }
            }

            TIME_UNIT frequency = howard_res_bis.first;

            VERBOSE_INFO("KSchedule function get " << frequency << " from MCRP." );
            VERBOSE_INFO("  ->  then omega =  " <<  1 / frequency );

            resultprime.first = frequency;

            ////////////// SCHEDULE CALL // END

            if (sameset(dataflow,&(resultprime.second),&(result.second)))  {
                VERBOSE_INFO("Critical circuit is the same");
                result = resultprime;

                break;
            }
            result = resultprime;
            VERBOSE_INFO("Current K-periodic throughput (" << result.first <<  ") is not enough.");
            VERBOSE_DEBUG("   Critical circuit is " << cc2string(dataflow,&(result.second)) <<  "");
        }

    }


    VERBOSE_INFO( "K-periodic schedule - iterations count is " << iteration_count << "  final size is " << eg->getEventCount() << " events and " << eg->getConstraintsCount() << " constraints.");
    delete eg;

    EXEC_COUNT total_ni = 0;
    EXEC_COUNT total_ki = 0;
    {ForEachVertex(dataflow,t) {
        total_ni += dataflow->getNi(t);
        total_ki += kvector[t];
    }}

    VERBOSE_INFO("K-periodic schedule - total_ki=" << total_ki << " total_ni=" << total_ni );
    return kvector;
}

void algorithms::compute_KperiodicSlow_throughput    (models::Dataflow* const dataflow, parameters_list_t) {

    EXEC_COUNT iteration_count = 0;
    VERBOSE_ASSERT(dataflow,TXT_NEVER_HAPPEND);

    // STEP 1 - generate 1-periodic schedule
    std::map<Vertex,EXEC_COUNT> kvector;
    {ForEachVertex(dataflow,t) {
        kvector[t] = 1;
    }}
    std::pair<TIME_UNIT, std::set<Edge> > result = KSchedule(dataflow,&kvector);

    if (result.second.size() != 0) {

        VERBOSE_INFO("1-periodic throughput (" << result.first <<  ") is not enough.");
        VERBOSE_INFO("   Critical circuit is " << cc2string(dataflow,&(result.second)) <<  "");

        while (true) {
            iteration_count++;
            updateVectorWithLocalNi(dataflow,&kvector,&(result.second));
            std::pair<TIME_UNIT, std::set<Edge> > resultprime = KSchedule(dataflow,&kvector);
            if (sameset(dataflow,&(resultprime.second),&(result.second)))  {
                VERBOSE_INFO("Critical circuit is the same");
                result = resultprime;
                break;
            }
            result = resultprime;
            VERBOSE_INFO("Current K-periodic throughput (" << result.first <<  ") is not enough.");
            VERBOSE_INFO("   Critical circuit is " << cc2string(dataflow,&(result.second)) <<  "");
        }

    } {
        iteration_count++;
    }

    VERBOSE_INFO( "K-periodic schedule - iterations count is " << iteration_count);


    EXEC_COUNT total_ni = 0;
    EXEC_COUNT total_ki = 0;
    {ForEachVertex(dataflow,t) {
        total_ni += dataflow->getNi(t);
        total_ki += kvector[t];
    }}

    VERBOSE_INFO("K-periodic schedule - total_ki=" << total_ki << " total_ni=" << total_ni );
    TIME_UNIT res = result.first;
    std::cout << "Maximum throughput is " << std::scientific << std::setw( 11 ) << std::setprecision( 9 ) <<  res   << std::endl;
    std::cout << "Maximum period     is " << std::fixed << std::setw( 11 ) << std::setprecision( 6 ) << 1.0/res   << std::endl;



}

// Compute K-periodic schedule by using an update module
void algorithms::compute_Kperiodic_throughput    (models::Dataflow* const dataflow, parameters_list_t  parameters  ) {


    VERBOSE_ASSERT(computeRepetitionVector(dataflow),"inconsistent graph");

    bool verbose = false;
    if (parameters.find("PRINT") != parameters.end() ) {
        verbose = true;
    }

    EXEC_COUNT sumNi = 0;
    EXEC_COUNT sumKi = dataflow->getVerticesCount();

    {ForEachTask(dataflow,t) {
        sumNi += dataflow->getNi(t) ;
    }}

    // STEP 0.1 - PRE
    VERBOSE_ASSERT(dataflow,TXT_NEVER_HAPPEND);
    VERBOSE_ASSERT(computeRepetitionVector(dataflow),"inconsistent graph");
    EXEC_COUNT iteration_count = 0;

    // STEP 1 - generate initial vector
    std::map<Vertex,EXEC_COUNT> kvector;
    {ForEachVertex(dataflow,t) {
        kvector[t] = 1;

    }}


    std::pair<TIME_UNIT, std::set<Edge> > result;



    VERBOSE_INFO("KPeriodic EventGraph generation");

    //STEP 1 - Generate Event Graph
    models::EventGraph* eg = generateKPeriodicEventGraph(dataflow,&kvector);


    VERBOSE_INFO("KPeriodic EventGraph generation Done");

    //STEP 2 - resolve the MCRP on this Event Graph
    std::pair<TIME_UNIT,std::vector<models::EventGraphEdge> > howard_res = eg->MinCycleRatio();

    std::vector<models::EventGraphEdge> * critical_circuit = &(howard_res.second);

    //STEP 3 - convert CC(eg) => CC(graph)
    VERBOSE_DEBUG("Critical circuit is about " << critical_circuit->size() << " edges.");
    for (std::vector<models::EventGraphEdge>::iterator it = critical_circuit->begin() ; it != critical_circuit->end() ; it++ ) {
        VERBOSE_DEBUG("   -> " << eg->getChannelId(*it) << " : " << eg->getSchedulingEvent(eg->getSource(*it)).toString() << " to " <<  eg->getSchedulingEvent(eg->getTarget(*it)).toString() <<  " = (" << eg->getConstraint(*it)._w << "," << eg->getConstraint(*it)._d << ")" );
        ARRAY_INDEX channel_id = eg->getChannelId(*it);
        try {
            Edge        channel    = dataflow->getEdgeById(channel_id);
            result.second.insert(channel);
        } catch(...) {
            VERBOSE_DEBUG("      is loopback");
        }
    }

    TIME_UNIT frequency = howard_res.first;

    VERBOSE_INFO("KSchedule function get " << frequency << " from MCRP." );
    VERBOSE_INFO("  ->  then omega =  " <<  1 / frequency );

    result.first = frequency;

    ////////////// SCHEDULE CALL // END


    if (verbose) {
        std::cout << "Iteration "<< std::fixed << std::setw( 4 ) << iteration_count <<  "      period = "  << std::fixed << std::setw( 15 ) << std::setprecision( 2 ) << 1.0/result.first  <<  "      complexity = "  << std::setw( 4 )  << (sumKi * 100) / sumNi << std::endl ;
    }

    if (result.second.size() != 0) {


        VERBOSE_INFO("1-periodic throughput (" << result.first <<  ") is not enough.");
        VERBOSE_INFO("   Critical circuit is " << cc2string(dataflow,&(result.second)) <<  "");

        while (true) {



            iteration_count++;
            ////////////// SCHEDULE CALL // BEGIN : resultprime = KSchedule(dataflow,&kvector);

            std::pair<TIME_UNIT, std::set<Edge> > resultprime;

            //VERBOSE_ASSERT( algorithms::normalize(dataflow),"inconsistent graph");
            VERBOSE_INFO("KPeriodic EventGraph generation");

            //STEP 1 - Generate Event Graph and update vector
            if (!updateEventGraph( dataflow ,  &kvector, &(result.second), eg, verbose)) break ;

            VERBOSE_INFO("KPeriodic EventGraph generation Done");

            //STEP 2 - resolve the MCRP on this Event Graph
            std::pair<TIME_UNIT,std::vector<models::EventGraphEdge> > howard_res_bis = eg->MinCycleRatio();

            std::vector<models::EventGraphEdge> * critical_circuit = &(howard_res_bis.second);

            //STEP 3 - convert CC(eg) => CC(graph)
            VERBOSE_DEBUG("Critical circuit is about " << critical_circuit->size() << " edges.");
            for (std::vector<models::EventGraphEdge>::iterator it = critical_circuit->begin() ; it != critical_circuit->end() ; it++ ) {
                VERBOSE_DEBUG("   -> " << eg->getChannelId(*it) << " : " << eg->getSchedulingEvent(eg->getSource(*it)).toString() << " to " <<  eg->getSchedulingEvent(eg->getTarget(*it)).toString() <<  " = (" << eg->getConstraint(*it)._w << "," << eg->getConstraint(*it)._d << ")" );
                ARRAY_INDEX channel_id = eg->getChannelId(*it);
                try {
                    Edge        channel    = dataflow->getEdgeById(channel_id);
                    resultprime.second.insert(channel);
                } catch(...) {
                    VERBOSE_DEBUG("      is loopback");
                }
            }

            TIME_UNIT frequency = howard_res_bis.first;

            VERBOSE_INFO("KSchedule function get " << frequency << " from MCRP." );
            VERBOSE_INFO("  ->  then omega =  " <<  1 / frequency );

            resultprime.first = frequency;

            ////////////// SCHEDULE CALL // END

            if (sameset(dataflow,&(resultprime.second),&(result.second)))  {
                VERBOSE_INFO("Critical circuit is the same");
                result = resultprime;

                break;
            }
            result = resultprime;
            VERBOSE_INFO("Current K-periodic throughput (" << result.first <<  ") is not enough.");
            VERBOSE_DEBUG("   Critical circuit is " << cc2string(dataflow,&(result.second)) <<  "");



            if (verbose) {
                sumKi = 0;
                {ForEachVertex(dataflow,t) {
                    sumKi += kvector[t];
                }}

                std::cout << "Iteration "<< std::fixed << std::setw( 4 ) << iteration_count <<  "      period = "  << std::fixed << std::setw( 15 ) << std::setprecision( 2 ) << 1.0/result.first  <<  "      complexity = "  << std::setw( 4 )  << (sumKi * 100) / sumNi << std::endl ;
            }

        }

    }


    VERBOSE_INFO( "K-periodic schedule - iterations count is " << iteration_count << "  final size is " << eg->getEventCount() << " events and " << eg->getConstraintsCount() << " constraints.");
    delete eg;

    EXEC_COUNT total_ki = 0;
    {ForEachVertex(dataflow,t) {
        total_ki += kvector[t];
    }}

    VERBOSE_INFO("K-periodic schedule - total_ki=" << sumKi << " total_ni=" << sumNi );

    TIME_UNIT res = result.first;
    std::cout << "Maximum throughput is " << std::scientific << std::setw( 11 ) << std::setprecision( 9 ) <<  res   << std::endl;
    std::cout << "Maximum period     is " << std::fixed << std::setw( 11 ) << std::setprecision( 6 ) << 1.0/res   << std::endl;



}




// Compute K-periodic schedule by using an update module
void algorithms::compute_KperiodicSlow2_throughput    (models::Dataflow* const dataflow, parameters_list_t) {

    EXEC_COUNT iteration_count = 0;
    VERBOSE_ASSERT(dataflow,TXT_NEVER_HAPPEND);

    // STEP 1 - generate 1-periodic schedule
    std::map<Vertex,EXEC_COUNT> kvector;
    {ForEachVertex(dataflow,t) {
        kvector[t] = 1;
    }}
    std::pair<TIME_UNIT, std::set<Edge> > result = KSchedule(dataflow,&kvector);

    if (result.second.size() != 0) {

        VERBOSE_INFO("1-periodic throughput (" << result.first <<  ") is not enough.");
        VERBOSE_INFO("   Critical circuit is " << cc2string(dataflow,&(result.second)) <<  "");

        while (true) {
            iteration_count++;
            updateVectorWithFineNi(dataflow,&kvector,&(result.second));
	    {ForEachVertex(dataflow,v) {
		VERBOSE_INFO("New vector " << dataflow->getVertexName(v) << " = " << kvector[v]  << "( Ni=" << dataflow->getNi(v) << ")" );
	      }}
	    std::pair<TIME_UNIT, std::set<Edge> > resultprime = KSchedule(dataflow,&kvector);
            if (sameset(dataflow,&(resultprime.second),&(result.second)))  {
                VERBOSE_INFO("Critical circuit is the same");
                result = resultprime;
                break;
            }
            result = resultprime;
            VERBOSE_INFO("Current K-periodic throughput (" << result.first <<  ") is not enough.");
            VERBOSE_INFO("   Critical circuit is " << cc2string(dataflow,&(result.second)) <<  "");
        }

    } {
        iteration_count++;
    }

    VERBOSE_INFO( "K-periodic schedule - iterations count is " << iteration_count);


    EXEC_COUNT total_ni = 0;
    EXEC_COUNT total_ki = 0;
    {ForEachVertex(dataflow,t) {
        total_ni += dataflow->getNi(t);
        total_ki += kvector[t];
    }}

    VERBOSE_INFO("K-periodic schedule - total_ki=" << total_ki << " total_ni=" << total_ni );

    TIME_UNIT res = result.first;
    std::cout << "Maximum throughput is " << std::scientific << std::setw( 11 ) << std::setprecision( 9 ) <<  res   << std::endl;
    std::cout << "Maximum period     is " << std::fixed << std::setw( 11 ) << std::setprecision( 6 ) << 1.0/res   << std::endl;

}

EXEC_COUNT algorithms::test_Kperiodic_throughput    (models::Dataflow* const dataflow, parameters_list_t) {

    EXEC_COUNT iteration_count = 0;
    VERBOSE_ASSERT(dataflow,TXT_NEVER_HAPPEND);

    // STEP 1 - generate 1-periodic schedule
    std::map<Vertex,EXEC_COUNT> kvector;
    {ForEachVertex(dataflow,t) {
        kvector[t] = 1;
    }}
    std::pair<TIME_UNIT, std::set<Edge> > result = KSchedule(dataflow,&kvector);

    if (result.second.size() != 0) {

        VERBOSE_INFO("1-periodic throughput (" << result.first <<  ") is not enough.");
        VERBOSE_INFO("   Critical circuit is " << cc2string(dataflow,&(result.second)) <<  "");

        while (true) {
            iteration_count++;
            updateVectorWithLocalNi(dataflow,&kvector,&(result.second));
            std::pair<TIME_UNIT, std::set<Edge> > resultprime = KSchedule(dataflow,&kvector);
            if (sameset(dataflow,&(resultprime.second),&(result.second)))  {
                VERBOSE_INFO("Critical circuit is the same");
                result = resultprime;
                break;
            }
            result = resultprime;
            VERBOSE_INFO("Current K-periodic throughput (" << result.first <<  ") is not enough.");
            VERBOSE_INFO("   Critical circuit is " << cc2string(dataflow,&(result.second)) <<  "");
        }

    } {
        iteration_count++;
    }

    VERBOSE_INFO( "K-periodic schedule - iterations count is " << iteration_count);


    EXEC_COUNT total_ni = 0;
    EXEC_COUNT total_ki = 0;
    {ForEachVertex(dataflow,t) {
        total_ni += dataflow->getNi(t);
        total_ki += kvector[t];
    }}

    VERBOSE_INFO("K-periodic schedule - total_ki=" << total_ki << " total_ni=" << total_ni );
    TIME_UNIT res = result.first;
    std::cout << "Maximum throughput is " << std::scientific << std::setw( 11 ) << std::setprecision( 9 ) <<  res   << std::endl;
    std::cout << "Maximum period     is " << std::fixed << std::setw( 11 ) << std::setprecision( 6 ) << 1.0/res   << std::endl;

    return iteration_count;

}
*/
