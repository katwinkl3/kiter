
#ifndef _NOCGRAPH_H_
#define _NOCGRAPH_H_

// C++ program to print all paths from a source to destination. 
#include <iostream>
#include <list> 
#include <map>
#include <vector>
#include <cmath>
//using namespace std; 

// A directed graph using adjacency list representation 
class NoCGraph 
{ 
	int V; // No. of vertices in graph 
	std::list<int> *adj; // Pointer to an array containing adjacency lists 
	std::map<int, std::vector< std::vector<int> > > paths;
	std::vector<int> returnPath;
	//std::map<int, int> linkUtil;
	std::vector<int> linkUtil;
	bool dumpPaths;
	int MESH_SIZE;
	int HWLIMIT;

	// A recursive function used by printAllPaths() 
	void printAllPathsUtil(int , int , bool [], int [], int &); 
	int const_length;

	public: 
	NoCGraph(int V); // Constructor 

	int size () const {return MESH_SIZE;}
	int getMeshSize  () const {return size();}
	NoCGraph()
	{ this->V = 1; adj = new std::list<int>[V]; dumpPaths = false; MESH_SIZE = (V/2); linkUtil.resize(V*V, 0); HWLIMIT = 4;} 

	std::vector< std::vector<int> > getShortestPaths(int s, int d)
	{
		int index = getMapIndex(s, d);
		return paths[index];
	}

	int findPathCost(std::vector<int>& mypath);
	void findLeastCostPath(int u, int d, bool visited[], int path[], int &path_index);
	std::vector<int> findPathDijkstra(int u, int d);
	std::vector<int> findLowContentionPath(int u, int d);
	std::vector<int> getPath(int s, int d)
	{
		dumpPaths = true;

		// Mark all the vertices as not visited
		bool *visited = new bool[V];

		// Create an array to store paths
		int *path = new int[V];
		int path_index = 0; // Initialize path[] as empty

		// Initialize all vertices as not visited
		for (int i = 0; i < V; i++)
			visited[i] = false;

		returnPath.clear();
		const_length = getPathLength(s, d);
		if (s != d)
			findLeastCostPath(s+MESH_SIZE, d+MESH_SIZE, visited, path, path_index);
		dumpPaths = false;
		return returnPath;
	}


	//std::map<int, int> getLinkUtil() { return linkUtil; }
	std::vector<int> getLinkUtil() { return linkUtil; }

	void clear() { linkUtil.resize(V*V, 0); }//linkUtil.clear(); }
	void addEdge(int u, int v); 
	void printAllPaths(int s, int d, int length);
        //void setLinkUtil(std::map<int, int> u) { linkUtil = u; }
        void setLinkUtil(std::vector<int> u) { linkUtil = u; }

	void generateAllShortestPaths();
	int getMapIndex(int x, int y) const {return (x*V + y);}
	std::pair<int,int> getMapIndexPair(int index) {  return {index/V, index%V}; }
	int getPathLength(int src, int dst)
	{
		if(src == dst)
			return 0;

		int _XSIZE = (int) sqrt(V / 2);
		auto srcx = src % _XSIZE;
		auto srcy  = src / _XSIZE;
		auto dstx = dst % _XSIZE;
		auto dsty = dst / _XSIZE;

		//std::cout << "srcx=" << srcx << ",srcy=" << srcy << ",dstx=" << dstx << ",dsty=" << dsty << ",_XSIZE=" << _XSIZE << "\n";
		return abs(srcx-dstx) + abs(srcy-dsty) + 2;
	}
}; 


#endif
