/*
 * modelCommunications.cpp
 *
 *  Created on: Apr 23, 2020
 *      Author: toky
 */


#include <models/Dataflow.h>
#include <commons/commons.h>
#include <unordered_map>
#include <algorithms/mappings.h>


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
		d->setMapping(middle,e); // This artifical task is mapped to a NetworkEdge
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
		if (list_idx == 0)
		d->setRoute(e1,list);

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
			d->setMapping(vtx,d->getNoC().getEdge(e).dst); // This artifical task is mapped to a NetworkNode


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

void algorithms::ModelNoCConflictFreeCommunication(models::Dataflow* const  dataflow, parameters_list_t   param_list ) {

	conflictEtype conflictEdges; //stores details of flows that share noc edges
	conflictConfigs configs; //stores the details of the router configs that are shared
	vid_to_nocEid vid_to_conflict_map; //used to index newly added nodes into the conflict table, so as to remove it easily

	std::vector<ARRAY_INDEX> edges_to_process;
	{ForEachChannel(dataflow,c) {
		edges_to_process.push_back(dataflow->getEdgeId(c));
	}}

	for (auto cid : edges_to_process) {
		Edge c = dataflow->getEdgeById(cid);
		auto route = dataflow->getRoute(c);
		VERBOSE_INFO("replace edge " << dataflow->getEdgeName(c) << "by a sequence");
		addPathNode(dataflow, c, route, conflictEdges, configs, vid_to_conflict_map, true);
	}


}

