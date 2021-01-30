/*
 * GraphMapper.h
 *
 *  Created on: Apr 22, 2020
 *      Author: toky
 */


#include <vector>
#include <models/Dataflow.h>
#include <models/NoCGraph.h>

namespace algorithms {

	struct node{
		ARRAY_INDEX index;
	};

class NodeComparator {

private :

		const std::vector< std::vector<unsigned int> >  &   cyclen_per_vtxid;

public :
		NodeComparator(const std::vector< std::vector<unsigned int> >  &   cyclen_per_vtxid) : cyclen_per_vtxid(cyclen_per_vtxid) {

		};

		bool lessthan(const node& a, const node& b) {

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
	};



class GraphMapper {

public :
	GraphMapper (const std::vector< std::vector<unsigned int> >  &   cyclen_per_vtxid) : cyclen_per_vtxid(cyclen_per_vtxid) {

	}

private :
	const std::vector< std::vector<unsigned int> >  &   cyclen_per_vtxid;
public:



	//remove the current edge between nodes add intermediate nodes based on the path between them
	std::map<int, route_t> taskAndNoCMapping(const models::Dataflow* input, models::Dataflow* to, Vertex start,  NoCGraph * noc) {
		std::map<int, route_t> routes;
		ARRAY_INDEX V = to->getVerticesCount();
		//std::priority_queue<node> pq;
		std::function<bool(node, node)> comparator([&](node a, node b) { NodeComparator comp (cyclen_per_vtxid); return comp.lessthan(a,b); });

		std::priority_queue<node, std::vector<node>, std::function<bool(node, node)>> pq(comparator);

		std::vector<bool> visited(V+1, false);
		std::vector<int> core_mapping(V+1, -1);

		//list of cores that are available
		std::vector<int> available_cores;//{5, 6, 10, 9, 8, 4, 0, 1, 2, 3, 7, 11, 15, 14, 13, 12};

		{
			for(int i = 0; i < noc->getMeshSize(); i++)
				available_cores.push_back(i);
		}
		VERBOSE_INFO ( "available_cores " << commons::toString(available_cores) ) ;

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
				//std::cout << "mapping " << to->getVertexId(top) << ",mesh=" << noc->getMeshSize() << "\n";
				//if( noc->getMeshSize() <= 9*9)
				//	mapping(top, core_mapping, noc, input, available_cores, routes);
				//else
					//hwconstrainted_mapping(top, core_mapping, noc, input, available_cores, routes);
					dijkstra_mapping(top, core_mapping, noc, input, available_cores, routes);
					//new_mapping(top, core_mapping, noc, input, available_cores, routes);

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
		//std::cout << "SRJKVR";
		//for(auto i = 1; i < core_mapping.size()-1; i++)
			//std::cout << "," << core_mapping[i];
		//std::cout << "\n";
		return routes;
	}
private :


	void setmap(std::vector<int> paths, std::vector<int>& curr_util, NoCGraph* noc)
	{
		for(unsigned int p_j = 1;  p_j < paths.size()-2; p_j++)
		{
			int mapindex = noc->getMapIndex(paths[p_j], paths[p_j+1]);
			curr_util[mapindex] += 1;
		}
	}

	int dijkstra_findPaths(int src, NoCGraph* noc, int core_considered, std::vector<int>& curr_util, std::map<int, route_t>& store_path, int storepath_id)
	{
		if(src != -1 && core_considered != -1)
		{
			std::vector<int> paths = noc->findPathDijkstra(src, core_considered);
			//std::cout << "PATH FOUND BETW SRJKVR " << src << " and " << core_considered << " is " << commons::toString(paths) << "\n";

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


	void dijkstra_mapping(Vertex vtx, std::vector<int>& core_mapping, NoCGraph* noc, const models::Dataflow* d, std::vector<int>& avail_cores, std::map<int, route_t>& routes)
	{
		const int start_core = avail_cores[0];
		auto index = d->getVertexId(vtx);
		if((int)avail_cores.size() == noc->size())
		{
			core_mapping[index] = start_core;
			std::remove(avail_cores.begin(), avail_cores.end(), start_core);
			avail_cores.resize( avail_cores.size() - 1);

			//std::cout << "allocating " << index << " to " << start_core << "\n";
			return;
		}

		float best_contention_l1 = -1;
		std::map<int, route_t> best_store_path;
		std::vector<int> best_util;
		int core_allocated = -1;

		//in the list of avaiable cores
		for (auto core_considered : avail_cores)
		{
			//std::cout << "dijk core_considered=" << core_considered << "\n";
			int cont_l1 = -1;
			std::vector<int> curr_util = noc->getLinkUtil(); //get the utilization of every link in the NoC
			std::map<int, route_t> store_path; //variable to store the route to be utilized by the (src,dest) core
			unsigned int counter = 0;
			unsigned int pathlen = 0;


			{ForInputEdges(d, vtx, e){	//Find the core index
				Vertex source_vtx = d->getEdgeSource(e);
				auto source = d->getVertexId(source_vtx);
				int src_core = core_mapping[source];
				//int storepath_id = noc->getMapIndex((int)source, (int)index);
				if(src_core != -1)
					pathlen += noc->getPathLength(src_core, core_considered);
			}}

			{ForOutputEdges(d, vtx, e){
				Vertex target_vtx = d->getEdgeTarget(e);
				auto target = d->getVertexId(target_vtx);
				int tgt_core = core_mapping[target];
				//int storepath_id = noc->getMapIndex((int)index, (int)target);
				if(tgt_core != -1)
					pathlen += noc->getPathLength(core_considered, tgt_core);
			}}


			if(best_contention_l1 != -1 && (float)pathlen > best_contention_l1) //reducing search space
				continue;

			{ForInputEdges(d, vtx, e){	//Find the core index
				Vertex source_vtx = d->getEdgeSource(e);
				auto source = d->getVertexId(source_vtx);
				int src_core = core_mapping[source];
				int storepath_id = noc->getMapIndex((int)source, (int)index);
				if(src_core != -1)
				{
					counter++;
					auto temp_cont_l1 = dijkstra_findPaths(src_core, noc, core_considered, curr_util, store_path, storepath_id);
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
					auto temp_cont_l1 = dijkstra_findPaths(core_considered, noc, tgt_core, curr_util, store_path, storepath_id);
					cont_l1 = std::max( temp_cont_l1, cont_l1 );
				}
			}}

			float cost = (float)cont_l1;
			if(counter > 0 )
				cost += (float)pathlen/(float)counter;


			//std::cout << "cost=" << cost << "\n";

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
				VERBOSE_INFO ( "already one route is stored for this");
			routes[it.first] = it.second;
		}
		//std::cout << "allocating " << index << " to " << core_allocated << "\n";
	}





};

}

