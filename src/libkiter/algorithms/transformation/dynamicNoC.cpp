/*
 * dynamicNoC.cpp
 *
 *  Created on: Apr 21, 2020
 *      Author: toky
 */

#include "dynamicNoC.h"
#include <models/Dataflow.h>
//
//void algorithms::dynamic_noc(models::Dataflow* const  dataflow, parameters_list_t   param_list)
//{
//	double yscale = 1;
//	if (param_list.count("yscale") == 1) yscale = std::stod(param_list["yscale"]);
//
//	conflictEtype conflictEdges; //stores details of flows that share noc edges
//	vid_to_nocEid vid_to_conflict_map; //used to index newly added nodes into the conflict table, so as to remove it easily
//	int mesh_row = (int)ceil(sqrt((double)dataflow->getVerticesCount()));
//
//	if(mesh_row <= 4)
//		mesh_row = 4;
//
//	NoC *noc = new NoC(mesh_row, mesh_row, 1); //Init NoC
//
//	if(mesh_row <= 9)
//		noc->generateShortestPaths();
//
//	// STEP 0.2 - Assert SDF
//	models::Dataflow* to = new models::Dataflow(*dataflow);
//	conflictConfigs configs;
//	/// Store a list of origina l actors id.
//	std::vector <ARRAY_INDEX> original_vertex_ids;
//	for (auto v : to->vertices()) {
//		original_vertex_ids.push_back(to->getVertexId(v));
//	}
//
//
//
//	//symbolic execution to find program execution order
//	models::Dataflow* to2 = new models::Dataflow(*dataflow);
//	std::vector<ARRAY_INDEX> prog_order = symbolic_execution(to2);
//	delete to2;
//
//	//do some processing to perfrom task mapping, NoC route determination and add intermediate nodes
//	std::map<int, route_t> routes = graphProcessing(to, noc, prog_order);
//	std::map<int, Edge> edge_list = generateEdgesMap(to, noc);
//	std::cout << "dont path finding, add paths now\n";
//
//	for(auto it:routes) {
//		Edge e = edge_list[it.first];
//		Vertex esrc = to->getEdgeSource(e);
//		VERBOSE_INFO("replace edge " << e << "by a sequence");
//		addPathNode(to, e, it.second, conflictEdges, configs, vid_to_conflict_map, false);
//	}
//
//	std::cout << "adding path nodes done\n";
//	//resolve cnflicts for all the  (a) sources that sent data to multiple nodes.
//	//				(b) destinations that receive data from multiple nodes.
//	//				(c) nodes that correspond to the same coniguration
//	//Use [1, origV] as it denotes the list of nodes in the original SDF
//	for (auto vid : original_vertex_ids) {
//		auto src = to->getVertexById(vid);
//		resolveSrcConflicts(to, src, original_vertex_ids);
//	}
//
///*
//	for (auto vid : original_vertex_ids)
//	{
//		auto dest = to->getVertexById(vid);
//		resolveDestConflicts(to, dest, original_vertex_ids);
//	}
//	std::cout << "done dest conflict resolve\n";
//*/
//	//Remove conflicts at source and destination router links as a big node has been created
//	for(auto it:routes)
//	{
//		auto start_id = routes[it.first][0];
//		auto cf_it = conflictEdges.find((unsigned int)start_id);
//		if(cf_it != conflictEdges.end())
//	 		conflictEdges.erase(cf_it);
//
///*
//		auto mysize = routes[it.first].size() - 1;
//		auto end_id = routes[it.first][mysize];
//		auto cf_it2 = conflictEdges.find((unsigned int)end_id);
//		if(cf_it2 != conflictEdges.end())
//	 		conflictEdges.erase(cf_it2);
//*/
//	}
//
//	VERBOSE_INFO("resolving conflicts done");
//	//Original graph
//	removeOrphanNodes(to, vid_to_conflict_map, conflictEdges);
//	std::cout << "calling scheduling\n";
//
//	commons::writeSDF3File(to->getFilename()+"_dyn.xml", to);
//
//
//
///*
//	VERBOSE_ASSERT(computeRepetitionVector(to),"inconsistent graph");
//	models::Scheduling scheduling_res = algorithms::scheduling::CSDF_KPeriodicScheduling(to);
//	scheduling_t persched = scheduling_res.getTaskSchedule();
//	VERBOSE_INFO("findHP");
//	unsigned long LCM;
//	TIME_UNIT HP;
//	findHP(dataflow, to, persched, &HP, &LCM);
//*/
//
//}


