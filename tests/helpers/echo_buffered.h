/*
 * echo_buffered.h
 *
 *  Created on: Dec 3, 2019
 *      Author: toky
 */

#ifndef TESTS_HELPERS_ECHO_BUFFERED_H_
#define TESTS_HELPERS_ECHO_BUFFERED_H_
#include <models/Dataflow.h>



models::Dataflow* generateEchoBuffered () {
	// Auto-generate by Kiter for Kiter
	//

	models::Dataflow* new_graph = new models::Dataflow();

	{

	auto new_vertex = new_graph->addVertex(1);
	 new_graph->setVertexName(new_vertex,"audio_in_1");
	 new_graph->setPhasesQuantity(new_vertex,1);
	 new_graph->setReentrancyFactor(new_vertex,1);
	 new_graph->setVertexDuration(new_vertex,{1.44738e+07});
	}
	{

	auto new_vertex = new_graph->addVertex(2);
	 new_graph->setVertexName(new_vertex,"audio_in_2");
	 new_graph->setPhasesQuantity(new_vertex,1);
	 new_graph->setReentrancyFactor(new_vertex,1);
	 new_graph->setVertexDuration(new_vertex,{1.78487e+07});
	}
	{

	auto new_vertex = new_graph->addVertex(3);
	 new_graph->setVertexName(new_vertex,"audio_out_3");
	 new_graph->setPhasesQuantity(new_vertex,1);
	 new_graph->setReentrancyFactor(new_vertex,1);
	 new_graph->setVertexDuration(new_vertex,{1.49562e+07});
	}
	{

	auto new_vertex = new_graph->addVertex(4);
	 new_graph->setVertexName(new_vertex,"Dup_5");
	 new_graph->setPhasesQuantity(new_vertex,1);
	 new_graph->setReentrancyFactor(new_vertex,1);
	 new_graph->setVertexDuration(new_vertex,{255779});
	}
	{

	auto new_vertex = new_graph->addVertex(5);
	 new_graph->setVertexName(new_vertex,"Dup_7");
	 new_graph->setPhasesQuantity(new_vertex,1);
	 new_graph->setReentrancyFactor(new_vertex,1);
	 new_graph->setVertexDuration(new_vertex,{3.84457e+06});
	}
	{

	auto new_vertex = new_graph->addVertex(6);
	 new_graph->setVertexName(new_vertex,"Norm_factor_elem_optim_8");
	 new_graph->setPhasesQuantity(new_vertex,1);
	 new_graph->setReentrancyFactor(new_vertex,1);
	 new_graph->setVertexDuration(new_vertex,{1.66006e+06});
	}
	{

	auto new_vertex = new_graph->addVertex(7);
	 new_graph->setVertexName(new_vertex,"Norm_factor_elem_optim_9");
	 new_graph->setPhasesQuantity(new_vertex,1);
	 new_graph->setReentrancyFactor(new_vertex,1);
	 new_graph->setVertexDuration(new_vertex,{1.45096e+06});
	}
	{

	auto new_vertex = new_graph->addVertex(8);
	 new_graph->setVertexName(new_vertex,"Norm_factor_elem_optim_10");
	 new_graph->setPhasesQuantity(new_vertex,1);
	 new_graph->setReentrancyFactor(new_vertex,1);
	 new_graph->setVertexDuration(new_vertex,{1.98053e+06});
	}
	{

	auto new_vertex = new_graph->addVertex(9);
	 new_graph->setVertexName(new_vertex,"Norm_factor_elem_optim_11");
	 new_graph->setPhasesQuantity(new_vertex,1);
	 new_graph->setReentrancyFactor(new_vertex,1);
	 new_graph->setVertexDuration(new_vertex,{1.95366e+06});
	}
	{

	auto new_vertex = new_graph->addVertex(10);
	 new_graph->setVertexName(new_vertex,"Norm_factor_elem_optim_12");
	 new_graph->setPhasesQuantity(new_vertex,1);
	 new_graph->setReentrancyFactor(new_vertex,1);
	 new_graph->setVertexDuration(new_vertex,{1.47265e+06});
	}
	{

	auto new_vertex = new_graph->addVertex(11);
	 new_graph->setVertexName(new_vertex,"Norm_factor_elem_optim_13");
	 new_graph->setPhasesQuantity(new_vertex,1);
	 new_graph->setReentrancyFactor(new_vertex,1);
	 new_graph->setVertexDuration(new_vertex,{1.96723e+06});
	}
	{

	auto new_vertex = new_graph->addVertex(12);
	 new_graph->setVertexName(new_vertex,"Norm_factor_elem_optim_14");
	 new_graph->setPhasesQuantity(new_vertex,1);
	 new_graph->setReentrancyFactor(new_vertex,1);
	 new_graph->setVertexDuration(new_vertex,{1.3753e+06});
	}
	{

	auto new_vertex = new_graph->addVertex(13);
	 new_graph->setVertexName(new_vertex,"Norm_factor_elem_optim_15");
	 new_graph->setPhasesQuantity(new_vertex,1);
	 new_graph->setReentrancyFactor(new_vertex,1);
	 new_graph->setVertexDuration(new_vertex,{2.14934e+06});
	}
	{

	auto new_vertex = new_graph->addVertex(14);
	 new_graph->setVertexName(new_vertex,"Sum_Invert_16");
	 new_graph->setPhasesQuantity(new_vertex,1);
	 new_graph->setReentrancyFactor(new_vertex,1);
	 new_graph->setVertexDuration(new_vertex,{384939});
	}
	{

	auto new_vertex = new_graph->addVertex(15);
	 new_graph->setVertexName(new_vertex,"Dup_18");
	 new_graph->setPhasesQuantity(new_vertex,1);
	 new_graph->setReentrancyFactor(new_vertex,1);
	 new_graph->setVertexDuration(new_vertex,{620475});
	}
	{

	auto new_vertex = new_graph->addVertex(16);
	 new_graph->setVertexName(new_vertex,"Wfilter_elem_19");
	 new_graph->setPhasesQuantity(new_vertex,1);
	 new_graph->setReentrancyFactor(new_vertex,1);
	 new_graph->setVertexDuration(new_vertex,{457890});
	}
	{

	auto new_vertex = new_graph->addVertex(17);
	 new_graph->setVertexName(new_vertex,"Wfilter_elem_20");
	 new_graph->setPhasesQuantity(new_vertex,1);
	 new_graph->setReentrancyFactor(new_vertex,1);
	 new_graph->setVertexDuration(new_vertex,{421128});
	}
	{

	auto new_vertex = new_graph->addVertex(18);
	 new_graph->setVertexName(new_vertex,"Wfilter_elem_21");
	 new_graph->setPhasesQuantity(new_vertex,1);
	 new_graph->setReentrancyFactor(new_vertex,1);
	 new_graph->setVertexDuration(new_vertex,{247453});
	}
	{

	auto new_vertex = new_graph->addVertex(19);
	 new_graph->setVertexName(new_vertex,"Wfilter_elem_22");
	 new_graph->setPhasesQuantity(new_vertex,1);
	 new_graph->setReentrancyFactor(new_vertex,1);
	 new_graph->setVertexDuration(new_vertex,{435181});
	}
	{

	auto new_vertex = new_graph->addVertex(20);
	 new_graph->setVertexName(new_vertex,"Wfilter_elem_23");
	 new_graph->setPhasesQuantity(new_vertex,1);
	 new_graph->setReentrancyFactor(new_vertex,1);
	 new_graph->setVertexDuration(new_vertex,{422413});
	}
	{

	auto new_vertex = new_graph->addVertex(21);
	 new_graph->setVertexName(new_vertex,"Wfilter_elem_24");
	 new_graph->setPhasesQuantity(new_vertex,1);
	 new_graph->setReentrancyFactor(new_vertex,1);
	 new_graph->setVertexDuration(new_vertex,{185566});
	}
	{

	auto new_vertex = new_graph->addVertex(22);
	 new_graph->setVertexName(new_vertex,"Wfilter_elem_25");
	 new_graph->setPhasesQuantity(new_vertex,1);
	 new_graph->setReentrancyFactor(new_vertex,1);
	 new_graph->setVertexDuration(new_vertex,{413265});
	}
	{

	auto new_vertex = new_graph->addVertex(23);
	 new_graph->setVertexName(new_vertex,"Wfilter_elem_26");
	 new_graph->setPhasesQuantity(new_vertex,1);
	 new_graph->setReentrancyFactor(new_vertex,1);
	 new_graph->setVertexDuration(new_vertex,{622419});
	}
	{

	auto new_vertex = new_graph->addVertex(24);
	 new_graph->setVertexName(new_vertex,"Dup_27");
	 new_graph->setPhasesQuantity(new_vertex,1);
	 new_graph->setReentrancyFactor(new_vertex,1);
	 new_graph->setVertexDuration(new_vertex,{323679});
	}
	{

	auto new_vertex = new_graph->addVertex(25);
	 new_graph->setVertexName(new_vertex,"Dup_29");
	 new_graph->setPhasesQuantity(new_vertex,1);
	 new_graph->setReentrancyFactor(new_vertex,1);
	 new_graph->setVertexDuration(new_vertex,{306939});
	}
	{

	auto new_vertex = new_graph->addVertex(26);
	 new_graph->setVertexName(new_vertex,"error_calculation_30");
	 new_graph->setPhasesQuantity(new_vertex,1);
	 new_graph->setReentrancyFactor(new_vertex,1);
	 new_graph->setVertexDuration(new_vertex,{419920});
	}
	{

	auto new_vertex = new_graph->addVertex(27);
	 new_graph->setVertexName(new_vertex,"Dup_32");
	 new_graph->setPhasesQuantity(new_vertex,1);
	 new_graph->setReentrancyFactor(new_vertex,1);
	 new_graph->setVertexDuration(new_vertex,{583601});
	}
	{

	auto new_vertex = new_graph->addVertex(28);
	 new_graph->setVertexName(new_vertex,"Dup_33");
	 new_graph->setPhasesQuantity(new_vertex,1);
	 new_graph->setReentrancyFactor(new_vertex,1);
	 new_graph->setVertexDuration(new_vertex,{1.15223e+06});
	}
	{

	auto new_vertex = new_graph->addVertex(29);
	 new_graph->setVertexName(new_vertex,"Dup_34");
	 new_graph->setPhasesQuantity(new_vertex,1);
	 new_graph->setReentrancyFactor(new_vertex,1);
	 new_graph->setVertexDuration(new_vertex,{501488});
	}
	{

	auto new_vertex = new_graph->addVertex(30);
	 new_graph->setVertexName(new_vertex,"Wupdate_elem_35");
	 new_graph->setPhasesQuantity(new_vertex,1);
	 new_graph->setReentrancyFactor(new_vertex,1);
	 new_graph->setVertexDuration(new_vertex,{508300});
	}
	{

	auto new_vertex = new_graph->addVertex(31);
	 new_graph->setVertexName(new_vertex,"Wupdate_elem_36");
	 new_graph->setPhasesQuantity(new_vertex,1);
	 new_graph->setReentrancyFactor(new_vertex,1);
	 new_graph->setVertexDuration(new_vertex,{255688});
	}
	{

	auto new_vertex = new_graph->addVertex(32);
	 new_graph->setVertexName(new_vertex,"Wupdate_elem_37");
	 new_graph->setPhasesQuantity(new_vertex,1);
	 new_graph->setReentrancyFactor(new_vertex,1);
	 new_graph->setVertexDuration(new_vertex,{335501});
	}
	{

	auto new_vertex = new_graph->addVertex(33);
	 new_graph->setVertexName(new_vertex,"Wupdate_elem_38");
	 new_graph->setPhasesQuantity(new_vertex,1);
	 new_graph->setReentrancyFactor(new_vertex,1);
	 new_graph->setVertexDuration(new_vertex,{337028});
	}
	{

	auto new_vertex = new_graph->addVertex(34);
	 new_graph->setVertexName(new_vertex,"Wupdate_elem_39");
	 new_graph->setPhasesQuantity(new_vertex,1);
	 new_graph->setReentrancyFactor(new_vertex,1);
	 new_graph->setVertexDuration(new_vertex,{429331});
	}
	{

	auto new_vertex = new_graph->addVertex(35);
	 new_graph->setVertexName(new_vertex,"Wupdate_elem_40");
	 new_graph->setPhasesQuantity(new_vertex,1);
	 new_graph->setReentrancyFactor(new_vertex,1);
	 new_graph->setVertexDuration(new_vertex,{385595});
	}
	{

	auto new_vertex = new_graph->addVertex(36);
	 new_graph->setVertexName(new_vertex,"Wupdate_elem_41");
	 new_graph->setPhasesQuantity(new_vertex,1);
	 new_graph->setReentrancyFactor(new_vertex,1);
	 new_graph->setVertexDuration(new_vertex,{414125});
	}
	{

	auto new_vertex = new_graph->addVertex(37);
	 new_graph->setVertexName(new_vertex,"Wupdate_elem_42");
	 new_graph->setPhasesQuantity(new_vertex,1);
	 new_graph->setReentrancyFactor(new_vertex,1);
	 new_graph->setVertexDuration(new_vertex,{354902});
	}
	{

	auto new_vertex = new_graph->addVertex(38);
	 new_graph->setVertexName(new_vertex,"Join_43");
	 new_graph->setPhasesQuantity(new_vertex,8);
	 new_graph->setReentrancyFactor(new_vertex,1);
	 new_graph->setVertexDuration(new_vertex,{361639,222689,362437,380756,268630,228866,186859,102795});
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(5), new_graph->getVertexById(6));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_0");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(6), new_graph->getVertexById(14));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_1");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(5), new_graph->getVertexById(7));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_2");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(7), new_graph->getVertexById(14));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_3");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(5), new_graph->getVertexById(8));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_4");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(8), new_graph->getVertexById(14));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_5");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(5), new_graph->getVertexById(9));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_6");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(9), new_graph->getVertexById(14));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_7");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(5), new_graph->getVertexById(10));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_8");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(10), new_graph->getVertexById(14));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_9");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(5), new_graph->getVertexById(11));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_10");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(11), new_graph->getVertexById(14));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_11");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(5), new_graph->getVertexById(12));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_12");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(12), new_graph->getVertexById(14));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_13");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(5), new_graph->getVertexById(13));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_14");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(13), new_graph->getVertexById(14));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_15");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(24), new_graph->getVertexById(16));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_16");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(24), new_graph->getVertexById(17));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_17");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(24), new_graph->getVertexById(18));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_18");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(24), new_graph->getVertexById(19));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_19");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(24), new_graph->getVertexById(20));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_20");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(24), new_graph->getVertexById(21));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_21");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(24), new_graph->getVertexById(22));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_22");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(24), new_graph->getVertexById(23));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_23");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(15), new_graph->getVertexById(16));
	 new_graph->setEdgeInPhases(new_edge,{2496});
	 new_graph->setEdgeOutPhases(new_edge,{2496});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_24");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(15), new_graph->getVertexById(17));
	 new_graph->setEdgeInPhases(new_edge,{2496});
	 new_graph->setEdgeOutPhases(new_edge,{2496});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_25");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(15), new_graph->getVertexById(18));
	 new_graph->setEdgeInPhases(new_edge,{2496});
	 new_graph->setEdgeOutPhases(new_edge,{2496});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_26");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(15), new_graph->getVertexById(19));
	 new_graph->setEdgeInPhases(new_edge,{2496});
	 new_graph->setEdgeOutPhases(new_edge,{2496});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_27");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(15), new_graph->getVertexById(20));
	 new_graph->setEdgeInPhases(new_edge,{2496});
	 new_graph->setEdgeOutPhases(new_edge,{2496});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_28");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(15), new_graph->getVertexById(21));
	 new_graph->setEdgeInPhases(new_edge,{2496});
	 new_graph->setEdgeOutPhases(new_edge,{2496});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_29");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(15), new_graph->getVertexById(22));
	 new_graph->setEdgeInPhases(new_edge,{2496});
	 new_graph->setEdgeOutPhases(new_edge,{2496});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_30");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(15), new_graph->getVertexById(23));
	 new_graph->setEdgeInPhases(new_edge,{2496});
	 new_graph->setEdgeOutPhases(new_edge,{2496});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_31");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(27), new_graph->getVertexById(30));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_32");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(27), new_graph->getVertexById(31));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_33");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(27), new_graph->getVertexById(32));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_34");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(27), new_graph->getVertexById(33));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_35");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(27), new_graph->getVertexById(34));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_36");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(27), new_graph->getVertexById(35));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_37");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(27), new_graph->getVertexById(36));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_38");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(27), new_graph->getVertexById(37));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_39");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(28), new_graph->getVertexById(30));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_40");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(28), new_graph->getVertexById(31));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_41");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(28), new_graph->getVertexById(32));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_42");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(28), new_graph->getVertexById(33));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_43");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(28), new_graph->getVertexById(34));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_44");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(28), new_graph->getVertexById(35));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_45");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(28), new_graph->getVertexById(36));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_46");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(28), new_graph->getVertexById(37));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_47");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(29), new_graph->getVertexById(30));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_48");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(29), new_graph->getVertexById(31));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_49");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(29), new_graph->getVertexById(32));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_50");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(29), new_graph->getVertexById(33));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_51");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(29), new_graph->getVertexById(34));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_52");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(29), new_graph->getVertexById(35));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_53");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(29), new_graph->getVertexById(36));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_54");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(29), new_graph->getVertexById(37));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_55");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(30), new_graph->getVertexById(38));
	 new_graph->setEdgeInPhases(new_edge,{312});
	 new_graph->setEdgeOutPhases(new_edge,{312,0,0,0,0,0,0,0});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_56");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(31), new_graph->getVertexById(38));
	 new_graph->setEdgeInPhases(new_edge,{312});
	 new_graph->setEdgeOutPhases(new_edge,{0,312,0,0,0,0,0,0});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_57");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(32), new_graph->getVertexById(38));
	 new_graph->setEdgeInPhases(new_edge,{312});
	 new_graph->setEdgeOutPhases(new_edge,{0,0,312,0,0,0,0,0});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_58");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(33), new_graph->getVertexById(38));
	 new_graph->setEdgeInPhases(new_edge,{312});
	 new_graph->setEdgeOutPhases(new_edge,{0,0,0,312,0,0,0,0});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_59");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(34), new_graph->getVertexById(38));
	 new_graph->setEdgeInPhases(new_edge,{312});
	 new_graph->setEdgeOutPhases(new_edge,{0,0,0,0,312,0,0,0});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_60");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(35), new_graph->getVertexById(38));
	 new_graph->setEdgeInPhases(new_edge,{312});
	 new_graph->setEdgeOutPhases(new_edge,{0,0,0,0,0,312,0,0});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_61");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(36), new_graph->getVertexById(38));
	 new_graph->setEdgeInPhases(new_edge,{312});
	 new_graph->setEdgeOutPhases(new_edge,{0,0,0,0,0,0,312,0});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_62");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(37), new_graph->getVertexById(38));
	 new_graph->setEdgeInPhases(new_edge,{312});
	 new_graph->setEdgeOutPhases(new_edge,{0,0,0,0,0,0,0,312});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_63");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(26), new_graph->getVertexById(25));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_64");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(25), new_graph->getVertexById(29));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_65");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(4), new_graph->getVertexById(24));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_66");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(4), new_graph->getVertexById(5));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_67");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(4), new_graph->getVertexById(27));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_68");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(38), new_graph->getVertexById(15));
	 new_graph->setEdgeInPhases(new_edge,{312,312,312,312,312,312,312,312});
	 new_graph->setEdgeOutPhases(new_edge,{2496});
	 new_graph->setPreload(new_edge,2496);
	 new_graph->setEdgeName(new_edge,"channel_69");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(14), new_graph->getVertexById(28));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_70");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(16), new_graph->getVertexById(26));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_71");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(17), new_graph->getVertexById(26));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_72");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(18), new_graph->getVertexById(26));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_73");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(19), new_graph->getVertexById(26));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_74");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(20), new_graph->getVertexById(26));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_75");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(21), new_graph->getVertexById(26));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_76");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(22), new_graph->getVertexById(26));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_77");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(23), new_graph->getVertexById(26));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_78");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(1), new_graph->getVertexById(4));
	 new_graph->setEdgeInPhases(new_edge,{1000});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_79");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(2), new_graph->getVertexById(26));
	 new_graph->setEdgeInPhases(new_edge,{1000});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_80");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(25), new_graph->getVertexById(3));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1000});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_81");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(6), new_graph->getVertexById(5));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_0_prime");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(14), new_graph->getVertexById(6));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_1_prime");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(7), new_graph->getVertexById(5));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_2_prime");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(14), new_graph->getVertexById(7));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_3_prime");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(8), new_graph->getVertexById(5));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_4_prime");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(14), new_graph->getVertexById(8));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_5_prime");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(9), new_graph->getVertexById(5));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_6_prime");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(14), new_graph->getVertexById(9));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_7_prime");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(10), new_graph->getVertexById(5));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_8_prime");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(14), new_graph->getVertexById(10));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_9_prime");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(11), new_graph->getVertexById(5));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_10_prime");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(14), new_graph->getVertexById(11));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_11_prime");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(12), new_graph->getVertexById(5));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_12_prime");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(14), new_graph->getVertexById(12));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_13_prime");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(13), new_graph->getVertexById(5));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_14_prime");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(14), new_graph->getVertexById(13));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_15_prime");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(16), new_graph->getVertexById(24));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_16_prime");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(17), new_graph->getVertexById(24));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_17_prime");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(18), new_graph->getVertexById(24));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_18_prime");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(19), new_graph->getVertexById(24));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_19_prime");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(20), new_graph->getVertexById(24));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_20_prime");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(21), new_graph->getVertexById(24));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_21_prime");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(22), new_graph->getVertexById(24));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_22_prime");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(23), new_graph->getVertexById(24));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_23_prime");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(16), new_graph->getVertexById(15));
	 new_graph->setEdgeInPhases(new_edge,{2496});
	 new_graph->setEdgeOutPhases(new_edge,{2496});
	 new_graph->setPreload(new_edge,2496);
	 new_graph->setEdgeName(new_edge,"channel_24_prime");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(17), new_graph->getVertexById(15));
	 new_graph->setEdgeInPhases(new_edge,{2496});
	 new_graph->setEdgeOutPhases(new_edge,{2496});
	 new_graph->setPreload(new_edge,2496);
	 new_graph->setEdgeName(new_edge,"channel_25_prime");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(18), new_graph->getVertexById(15));
	 new_graph->setEdgeInPhases(new_edge,{2496});
	 new_graph->setEdgeOutPhases(new_edge,{2496});
	 new_graph->setPreload(new_edge,2496);
	 new_graph->setEdgeName(new_edge,"channel_26_prime");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(19), new_graph->getVertexById(15));
	 new_graph->setEdgeInPhases(new_edge,{2496});
	 new_graph->setEdgeOutPhases(new_edge,{2496});
	 new_graph->setPreload(new_edge,2496);
	 new_graph->setEdgeName(new_edge,"channel_27_prime");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(20), new_graph->getVertexById(15));
	 new_graph->setEdgeInPhases(new_edge,{2496});
	 new_graph->setEdgeOutPhases(new_edge,{2496});
	 new_graph->setPreload(new_edge,2496);
	 new_graph->setEdgeName(new_edge,"channel_28_prime");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(21), new_graph->getVertexById(15));
	 new_graph->setEdgeInPhases(new_edge,{2496});
	 new_graph->setEdgeOutPhases(new_edge,{2496});
	 new_graph->setPreload(new_edge,2496);
	 new_graph->setEdgeName(new_edge,"channel_29_prime");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(22), new_graph->getVertexById(15));
	 new_graph->setEdgeInPhases(new_edge,{2496});
	 new_graph->setEdgeOutPhases(new_edge,{2496});
	 new_graph->setPreload(new_edge,2496);
	 new_graph->setEdgeName(new_edge,"channel_30_prime");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(23), new_graph->getVertexById(15));
	 new_graph->setEdgeInPhases(new_edge,{2496});
	 new_graph->setEdgeOutPhases(new_edge,{2496});
	 new_graph->setPreload(new_edge,2496);
	 new_graph->setEdgeName(new_edge,"channel_31_prime");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(30), new_graph->getVertexById(27));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_32_prime");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(31), new_graph->getVertexById(27));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_33_prime");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(32), new_graph->getVertexById(27));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_34_prime");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(33), new_graph->getVertexById(27));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_35_prime");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(34), new_graph->getVertexById(27));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_36_prime");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(35), new_graph->getVertexById(27));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_37_prime");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(36), new_graph->getVertexById(27));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_38_prime");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(37), new_graph->getVertexById(27));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_39_prime");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(30), new_graph->getVertexById(28));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_40_prime");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(31), new_graph->getVertexById(28));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_41_prime");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(32), new_graph->getVertexById(28));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_42_prime");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(33), new_graph->getVertexById(28));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_43_prime");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(34), new_graph->getVertexById(28));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_44_prime");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(35), new_graph->getVertexById(28));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_45_prime");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(36), new_graph->getVertexById(28));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_46_prime");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(37), new_graph->getVertexById(28));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_47_prime");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(30), new_graph->getVertexById(29));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_48_prime");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(31), new_graph->getVertexById(29));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_49_prime");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(32), new_graph->getVertexById(29));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_50_prime");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(33), new_graph->getVertexById(29));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_51_prime");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(34), new_graph->getVertexById(29));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_52_prime");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(35), new_graph->getVertexById(29));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_53_prime");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(36), new_graph->getVertexById(29));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_54_prime");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(37), new_graph->getVertexById(29));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_55_prime");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(38), new_graph->getVertexById(30));
	 new_graph->setEdgeInPhases(new_edge,{312,0,0,0,0,0,0,0});
	 new_graph->setEdgeOutPhases(new_edge,{312});
	 new_graph->setPreload(new_edge,312);
	 new_graph->setEdgeName(new_edge,"channel_56_prime");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(38), new_graph->getVertexById(31));
	 new_graph->setEdgeInPhases(new_edge,{0,312,0,0,0,0,0,0});
	 new_graph->setEdgeOutPhases(new_edge,{312});
	 new_graph->setPreload(new_edge,312);
	 new_graph->setEdgeName(new_edge,"channel_57_prime");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(38), new_graph->getVertexById(32));
	 new_graph->setEdgeInPhases(new_edge,{0,0,312,0,0,0,0,0});
	 new_graph->setEdgeOutPhases(new_edge,{312});
	 new_graph->setPreload(new_edge,312);
	 new_graph->setEdgeName(new_edge,"channel_58_prime");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(38), new_graph->getVertexById(33));
	 new_graph->setEdgeInPhases(new_edge,{0,0,0,312,0,0,0,0});
	 new_graph->setEdgeOutPhases(new_edge,{312});
	 new_graph->setPreload(new_edge,312);
	 new_graph->setEdgeName(new_edge,"channel_59_prime");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(38), new_graph->getVertexById(34));
	 new_graph->setEdgeInPhases(new_edge,{0,0,0,0,312,0,0,0});
	 new_graph->setEdgeOutPhases(new_edge,{312});
	 new_graph->setPreload(new_edge,312);
	 new_graph->setEdgeName(new_edge,"channel_60_prime");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(38), new_graph->getVertexById(35));
	 new_graph->setEdgeInPhases(new_edge,{0,0,0,0,0,312,0,0});
	 new_graph->setEdgeOutPhases(new_edge,{312});
	 new_graph->setPreload(new_edge,312);
	 new_graph->setEdgeName(new_edge,"channel_61_prime");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(38), new_graph->getVertexById(36));
	 new_graph->setEdgeInPhases(new_edge,{0,0,0,0,0,0,312,0});
	 new_graph->setEdgeOutPhases(new_edge,{312});
	 new_graph->setPreload(new_edge,312);
	 new_graph->setEdgeName(new_edge,"channel_62_prime");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(38), new_graph->getVertexById(37));
	 new_graph->setEdgeInPhases(new_edge,{0,0,0,0,0,0,0,312});
	 new_graph->setEdgeOutPhases(new_edge,{312});
	 new_graph->setPreload(new_edge,312);
	 new_graph->setEdgeName(new_edge,"channel_63_prime");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(25), new_graph->getVertexById(26));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_64_prime");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(29), new_graph->getVertexById(25));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_65_prime");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(24), new_graph->getVertexById(4));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_66_prime");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(5), new_graph->getVertexById(4));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_67_prime");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(27), new_graph->getVertexById(4));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_68_prime");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(15), new_graph->getVertexById(38));
	 new_graph->setEdgeInPhases(new_edge,{2496});
	 new_graph->setEdgeOutPhases(new_edge,{312,312,312,312,312,312,312,312});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_69_prime");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(28), new_graph->getVertexById(14));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_70_prime");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(26), new_graph->getVertexById(16));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_71_prime");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(26), new_graph->getVertexById(17));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_72_prime");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(26), new_graph->getVertexById(18));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_73_prime");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(26), new_graph->getVertexById(19));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_74_prime");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(26), new_graph->getVertexById(20));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_75_prime");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(26), new_graph->getVertexById(21));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_76_prime");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(26), new_graph->getVertexById(22));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_77_prime");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(26), new_graph->getVertexById(23));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_78_prime");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(4), new_graph->getVertexById(1));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1000});
	 new_graph->setPreload(new_edge,1000);
	 new_graph->setEdgeName(new_edge,"channel_79_prime");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(26), new_graph->getVertexById(2));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1000});
	 new_graph->setPreload(new_edge,1000);
	 new_graph->setEdgeName(new_edge,"channel_80_prime");
	}
	{

	auto new_edge = new_graph->addEdge(new_graph->getVertexById(3), new_graph->getVertexById(25));
	 new_graph->setEdgeInPhases(new_edge,{1000});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1000);
	 new_graph->setEdgeName(new_edge,"channel_81_prime");
	}
	return new_graph;
}




#endif /* TESTS_HELPERS_ECHO_BUFFERED_H_ */
