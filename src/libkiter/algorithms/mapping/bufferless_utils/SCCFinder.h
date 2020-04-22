/*
 * SCCFinder.h
 *
 *  Created on: Apr 22, 2020
 *      Author: toky
 */

#include <models/Dataflow.h>
#include <stack>
#include <vector>

namespace algorithms {
class SCCFinder {


	std::vector< std::vector<unsigned int> >     cycles;
	std::vector< std::set<unsigned int> >        cycid_per_vtxid;
	std::vector< std::vector<unsigned int> >     cyclen_per_vtxid;

public :
	SCCFinder (const models::Dataflow* to) {
		ARRAY_INDEX V = to->getVerticesCount();
		for(ARRAY_INDEX i = 0; i < V+1; i++)
		{
			std::set<unsigned int> temp;
			cycid_per_vtxid.push_back(temp);
		}

	}

	const std::vector< std::vector<unsigned int> > & getcyclen_per_vtxid() { return cyclen_per_vtxid;}

	// The main function that finds and prints all strongly connected components
		void printSCCs(const models::Dataflow* to, Vertex start)
		{
			ARRAY_INDEX V = to->getVerticesCount();
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


private :
	std::string vertexVector2str(std::vector<Vertex>& traversal, const models::Dataflow* to)
	{
		 std::stringstream s;
		for(unsigned int trav_i = 0; trav_i < traversal.size(); trav_i++)
			s << to->getVertexId(traversal[trav_i]) << " ";
		return s.str();
	}


	//find all parents and set their value with size
	void updateAncestorsHelper(Vertex vtx, const models::Dataflow* to, std::set<unsigned int>& myset, std::vector<bool>& visited)
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



	void updateAncestorsCycle(Vertex vtx, std::set<unsigned int>& myset, const models::Dataflow* to)
	{
		ARRAY_INDEX V = to->getVerticesCount();
		std::vector<bool> visited(V+1, false);
		updateAncestorsHelper(vtx, to, myset, visited);
	}


	// DFS traversal of the vertices reachable from v. It uses recursive DFSUtil()
	void DFS(const models::Dataflow* to, Vertex v)
	{
		ARRAY_INDEX V = to->getVerticesCount();
		// Mark all the vertices as not visited
		std::vector<bool> visited(V+1, false);

		// Call the recursive helper function to print DFS traversal
		std::vector<Vertex> traversal;
		DFSUtil(v, visited, to, traversal);

		std::string traverseStr = vertexVector2str(traversal, to);
		VERBOSE_INFO ( "DFS Order: "  << traverseStr) ;
	}


	void my_dfs(const models::Dataflow* adj, Vertex node, std::vector<bool> visited, Vertex start, std::vector<unsigned int>& path, std::set<unsigned int>& cycleids)
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


	std::set<unsigned int> my_dfs_wrapper(const models::Dataflow* adj, Vertex start)
	{
		ARRAY_INDEX V = adj->getVerticesCount();
		std::vector<bool> visited(V+1, false);
		std::vector<unsigned int> path;

		//Call the above function with the start node:
		std::set<unsigned int> mycycleids;
		my_dfs(adj, start, visited, start, path, mycycleids);

		return mycycleids;
	}


	void DFSUtil(Vertex v, std::vector<bool>& visited, const models::Dataflow* to, std::vector<Vertex>& traversal)
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

	models::Dataflow getTranspose(const models::Dataflow* to)
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


	void fillOrder(Vertex vtx, std::vector<bool>& visited, std::stack<Vertex> &Stack, const models::Dataflow* to)
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




};

}
