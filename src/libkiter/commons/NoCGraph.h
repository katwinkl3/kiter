// C++ program to print all paths from a source to destination. 
#include<iostream> 
#include <list> 
//using namespace std; 

// A directed graph using adjacency list representation 
class NoCGraph 
{ 
	int V; // No. of vertices in graph 
	std::list<int> *adj; // Pointer to an array containing adjacency lists 

	// A recursive function used by printAllPaths() 
	void printAllPathsUtil(int , int , bool [], int [], int &, int length); 
	int const_length;

	public: 
	NoCGraph(int V); // Constructor 

	NoCGraph()
	{ this->V = 1; adj = new std::list<int>[V]; } 

	void addEdge(int u, int v); 
	void printAllPaths(int s, int d, int length); 
}; 

NoCGraph::NoCGraph(int V) 
{ 
	this->V = V; 
	adj = new std::list<int>[V]; 
} 

void NoCGraph::addEdge(int u, int v) 
{ 
	adj[u].push_back(v); // Add v to u’s list. 
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
	length = 0;
	// Call the recursive helper function to print all paths 
	printAllPathsUtil(s, d, visited, path, path_index, length); 
	std::cout << "end\n";
}

// A recursive function to print all paths from 'u' to 'd'. 
// visited[] keeps track of vertices in current path. 
// path[] stores actual vertices and path_index is current 
// index in path[] 
void NoCGraph::printAllPathsUtil(int u, int d, bool visited[], 
		int path[], int &path_index, int length) 
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
		//std::cout << "Path_len=" << length << ":";
		for (int i = 0; i<path_index; i++) 
			std::cout << path[i] << " "; 
		std::cout << std::endl; 

	} 
	else // If current vertex is not destination 
	{

		if(length >= const_length)
		{
			//std::cout << "exiting " << u << "," << d << ",path_len=" << length << "\n";
			visited[u] = false;
			path_index--;
			return;
		}

		// Recur for all the vertices adjacent to current vertex 
		std::list<int>::iterator i; 
		for (i = adj[u].begin(); i != adj[u].end(); ++i) 
			if (!visited[*i])
			{
				int temp_length = length + 1;
				//std::cout << "calling " << *i << "," << d << ",path_len=" << temp_length << "\n";
				printAllPathsUtil(*i, d, visited, path, path_index, temp_length); 
			}
	} 

	// Remove current vertex from path[] and mark it as unvisited 
	path_index--; 
	visited[u] = false; 
}
