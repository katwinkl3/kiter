/*
 * LTEGeneator.cpp
 *
 *  Created on: Apr 21, 2020
 *      Author: toky
 */

#include "LTEGenerator.h"
#include <models/Dataflow.h>
#include <commons/SDF3Wrapper.h>

models::Dataflow*  generators::generate_lte_sdf(parameters_list_t   param_list) {


	double yscale = 1;
	if (param_list.count("yscale") == 1) yscale = std::stod(param_list["yscale"]);

	models::Dataflow* to = new models::Dataflow();

	std::vector<phase_info> phases;

	phases.push_back( getPhaseStruct(4, 392504, 16, "miwf"));
	phases.push_back( getPhaseStruct(2, 230635, 32, "cwac"));
	phases.push_back( getPhaseStruct(6, 353448, 32, "ifft"));
	phases.push_back( getPhaseStruct(4, 267559, 32, "dd"));


	std::vector< std::vector<ARRAY_INDEX> > vertex_info;
	for(unsigned int i = 0; i < phases.size(); i++)
	{
		auto ph_time = phases[i].exec_time;
		int ph_thds = phases[i].thds;
		std::string ph_name = phases[i].name;
		std::vector<ARRAY_INDEX> temp_vertex_list;

		for(int j = 0; j < ph_thds; j++)
		{
			auto new_vtx = to->addVertex();
			temp_vertex_list.push_back( to->getVertexId(new_vtx) );
			std::stringstream ss;
			ss << ph_name << "_" << j;

			to->setVertexName(new_vtx,ss.str());
			to->setPhasesQuantity(new_vtx,1); // number of state for the actor, only one in SDF
			to->setVertexDuration(new_vtx,{ ph_time }); // is specify for every state , only one for SDF.
			to->setReentrancyFactor(new_vtx,1); // This is the reentrancy, it avoid a task to be executed more than once at the same time.

		}
		vertex_info.push_back(temp_vertex_list);
	}


	for(unsigned int i = 0; i < phases.size() - 1; i++)
	{
		auto pkts = phases[i].send_pkts;
		for(unsigned int j = 0; j < vertex_info[i].size(); j++)
		{
			for (unsigned int k = 0; k < vertex_info[i+1].size(); k++)
			{
				auto src = to->getVertexById( vertex_info[i][j] );
				auto dst = to->getVertexById( vertex_info[i+1][k] );
				auto e1 = to->addEdge(src, dst);
				to->setEdgeInPhases(e1,{pkts});
				to->setEdgeOutPhases(e1,{pkts}); // and the consumption rate (as many rates as states for the associated task)
				to->setPreload(e1,0);  // preload is M0
			}
		}
	}

	return to;
}



