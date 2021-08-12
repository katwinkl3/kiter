/*
 * singleOutput.cpp
 *
 *  Created on: 12 Aug 2021
 *      Author: toky
 */

#include <models/Dataflow.h>
#include "singleOutput.h"

void algorithms::transformation::singleOutput    (models::Dataflow* const dataflow, parameters_list_t  parameters  ) {

	// Check arguments

	if (parameters.find("name") == parameters.end() ) {
		VERBOSE_ERROR("Please specify the 'name' argument.");
		return;
	}


	Vertex t = dataflow->getVertexByName(parameters["name"]);

	VERBOSE_INFO("Out degree = " << dataflow->getVertexOutDegree(t) );

	ARRAY_INDEX cnt = 0;
	while (dataflow->getVertexOutDegree(t) > 1) {

		VERBOSE_INFO("remove outputs..");


		// add a Duplicate,
		std::string newDup_name = dataflow->getVertexName(t) + "_Dup" + commons::toString(cnt++);
		Vertex newDup = dataflow->addVertex(newDup_name);
		dataflow->setPhasesQuantity(newDup,1);
		dataflow->setVertexDuration(newDup, {1});
		dataflow->setVertexType(newDup, "Dup");
		//connect one to duplicate from the task

		VERBOSE_INFO("Create new egde from source");
		Edge ne = dataflow->addEdge(t,  newDup);
		dataflow->setEdgeInPhases(ne, {1});
		dataflow->setEdgeOutPhases(ne, {1});

		VERBOSE_INFO("Collect edges");
		Edge c1;
		Edge c2;
		int cnt = 0;
		{ForOutputEdges(dataflow,t,c)	{
			cnt++;
			if (cnt == 1) c1 =  c;
			if (cnt == 2) c2 =  c;
			if (cnt >= 3) break;
		}}


		VERBOSE_INFO("Create new egde to targets");
		Edge nc1 = dataflow->addEdge(newDup,  dataflow->getEdgeTarget(c1));
		dataflow->setEdgeInPhases(nc1, {1});
		dataflow->setEdgeOutPhases(nc1, {1});
		Edge nc2 = dataflow->addEdge(newDup,  dataflow->getEdgeTarget(c2));
		dataflow->setEdgeInPhases(nc2, {1});
		dataflow->setEdgeOutPhases(nc2, {1});


		// remote the two buffer
		VERBOSE_INFO("Remove edges");
		dataflow->removeEdge(c1);
		dataflow->removeEdge(c2);



	}





}


