// C++ program to print all paths from a source to destination. 
#include<iostream> 
#include <list> 
//using namespace std; 

// A directed graph using adjacency list representation 
class NoCGraph 
{ 
	int V; // No. of vertices in graph 
	std::list<int> *adj; // Pointer to an array containing adjacency lists 
	std::map<int, std::vector< std::vector<int> > > paths;
	bool dumpPaths;
	int MESH_SIZE;

	// A recursive function used by printAllPaths() 
	void printAllPathsUtil(int , int , bool [], int [], int &); 
	int const_length;

	public: 
	NoCGraph(int V); // Constructor 

	NoCGraph()
	{ this->V = 1; adj = new std::list<int>[V]; dumpPaths = false; MESH_SIZE = (V/2);} 

	void addEdge(int u, int v); 
	void printAllPaths(int s, int d, int length);

	void generateAllShortestPaths();
	int getMapIndex(int x, int y) {return (x*V + y);}
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

NoCGraph::NoCGraph(int V) 
{ 
	this->V = V; 
	adj = new std::list<int>[V];
	dumpPaths = false;
	MESH_SIZE = (V/2);
} 

void NoCGraph::addEdge(int u, int v) 
{ 
	adj[u].push_back(v); // Add v to u’s list. 
} 

void NoCGraph::generateAllShortestPaths()
{
	int mesh_size = V/2;
	dumpPaths = true;

	for(int s = 0; s < mesh_size; s++)
	{
		for(int d = 0; d < mesh_size; d++)
		{
			// Mark all the vertices as not visited 
			bool *visited = new bool[V]; 

			// Create an array to store paths 
			int *path = new int[V]; 
			int path_index = 0; // Initialize path[] as empty 

			// Initialize all vertices as not visited 
			for (int i = 0; i < V; i++) 
				visited[i] = false; 

			const_length = getPathLength(s, d);
			int myindex = getMapIndex(s, d);

			if (s == d)
			{
				std::vector< std::vector<int> > mypath_vec;
				paths[myindex] = mypath_vec;
			}
			else
			{
				// Call the recursive helper function to print all paths 
				printAllPathsUtil(s+MESH_SIZE, d+MESH_SIZE, visited, path, path_index);
			}

			std::cout << "s=" << s << ",d=" << d << ",len=" << const_length << ",paths=" << paths[myindex].size()  << "\n";
		}
	}
	dumpPaths = false;
}

// Prints all paths from 's' to 'd' 
void NoCGraph::printAllPaths(int s, int d, int length) 
{ 
	// Mark all the vertices as not visited 
	bool *visited = new bool[V]; 

	// Create an array to store paths 
	int *path = new int[V]; 
	int path_index = 0; // Initialize path[] as empty 

	// Initialize all vertices as not visited 
	for (int i = 0; i < V; i++) 
		visited[i] = false; 

	std::cout << "here start:" << s << "," << d << ",len=" << length << "\n";
	const_length = length;

	// Call the recursive helper function to print all paths 
	if (s != d)
		printAllPathsUtil(s, d, visited, path, path_index); 
	std::cout << "end\n";
}

// A recursive function to print all paths from 'u' to 'd'. 
// visited[] keeps track of vertices in current path. 
// path[] stores actual vertices and path_index is current 
// index in path[] 
void NoCGraph::printAllPathsUtil(int u, int d, bool visited[], 
		int path[], int &path_index)
{ 
	// Mark the current node and store it in path[] 
	visited[u] = true; 
	path[path_index] = u; 
	path_index++; 

	// If current vertex is same as destination, then print 
	// current path[] 
	if (u == d) 
	{
		//std::cout << "calling " << d << "\n"; 
		//std::cout << "Path_len=" << path_index << ":";
		//for (int i = 0; i<path_index; i++) 
		//	std::cout << path[i] << " "; 
		//std::cout << std::endl; 

		if(dumpPaths && path[0] != d)
		{
			int myindex = getMapIndex(path[0]-MESH_SIZE, d-MESH_SIZE);
			std::vector<int> mypath;
			for (int i = 0; i<path_index; i++)
				mypath.push_back(path[i]);
			paths[myindex].push_back(mypath);
		}
	} 
	else // If current vertex is not destination 
	{

		if( (path_index-1) >= const_length)
		{
			//std::cout << "exiting " << u << "," << d << ",path_len=" << (path_index-1) << "\n";
			visited[u] = false;
			path_index--;
			return;
		}

		// Recur for all the vertices adjacent to current vertex 
		std::list<int>::iterator i; 
		for (i = adj[u].begin(); i != adj[u].end(); ++i) 
			if (!visited[*i])
			{
				//std::cout << "calling " << *i << "," << d << ",path_len=" << path_index << "\n";
				printAllPathsUtil(*i, d, visited, path, path_index); 
			}
	} 

	// Remove current vertex from path[] and mark it as unvisited 
	path_index--; 
	visited[u] = false; 
}
