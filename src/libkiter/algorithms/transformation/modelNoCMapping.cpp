/*
 * modelNoCMapping.cpp
 *
 *  Created on: Aug 21, 2019
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
#include <algorithms/transformations.h>


//remove the current edge between nodes
//add intermediate nodes based on the path between them
std::vector<Vertex> addPathNode(models::Dataflow* d, Edge c, std::map< unsigned int, std::vector< std::pair<Vertex, Vertex> > > & returnValue) {

	VERBOSE_ASSERT(not d->is_read_only(), "The graph must be writable to use addPathNode.");

	std::vector<Vertex> new_vertices;
	// We store infos about edge to be deleted
	auto source_vtx = d->getEdgeSource(c);
	auto target_vtx = d->getEdgeTarget(c);

	auto source_vtx_name = d->getVertexName(source_vtx);
	auto target_vtx_name = d->getVertexName(target_vtx);

	//Find the core index
	auto source = d->getMapping(source_vtx);
	auto target = d->getMapping(target_vtx);

	//use the inrate and route of the edges ans use it when creating the edges
	auto inrates = d->getEdgeInVector(c);
	auto outrates = d->getEdgeOutVector(c);
	auto preload = d->getPreload(c);  // preload is M0

	bool flag = true;
	if (source == target) //ignore this case
		return new_vertices;

	// we delete the edge
	d->removeEdge(c);

	//for every link in the path, add a corresponding node
	auto list = d->getNoC()->get_route(source, target);
	for (auto e : list) {
		//std::cout << e << " --> " ;
		// we create a new vertex "middle"
		auto middle = d->addVertex();
		new_vertices.push_back(middle);

		std::stringstream ss;
		ss << "mid_" << source_vtx_name << "_" << target_vtx_name << "_" << e;

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
			d->setEdgeInPhases(e1,inrates);  // we specify the production rates for the buffer
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
	d->setEdgeOutPhases(e2,outrates);
	d->setEdgeInPhases(e2,{1});
	d->setPreload(e2,0);  // preload is M0
	return new_vertices;

}


models::Dataflow* algorithms::transformation::modelNoCMapping(models::Dataflow* const  dataflow) {

	VERBOSE_ASSERT(dataflow,TXT_NEVER_HAPPEND);

	// STEP 0.2 - Assert SDF
	models::Dataflow* to = new models::Dataflow(*dataflow);
	std::map< unsigned int, std::vector< std::pair<Vertex, Vertex> > > conflictEdges;

	std::vector<Edge> edges_list;
	{ForEachEdge(to,e) {
		edges_list.push_back(e);
	}}

	for(auto e: edges_list) {
		addPathNode(to, e, conflictEdges);
	}

	return to;

}

