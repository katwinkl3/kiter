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
	std::cout << "generate a dataflow graph..." << std::endl;
	models::Dataflow* new_graph = new models::Dataflow();

	{
	std::cout << "generate a task ..." << std::endl;
	auto new_vertex = new_graph->addVertex(1);
	 new_graph->setVertexName(new_vertex,"audio_in_1");
	 new_graph->setPhasesQuantity(new_vertex,1);
	 new_graph->setReentrancyFactor(new_vertex,1);
	 new_graph->setVertexDuration(new_vertex,{1.44738e+07});
	}
	{
	std::cout << "generate a task ..." << std::endl;
	auto new_vertex = new_graph->addVertex(2);
	 new_graph->setVertexName(new_vertex,"audio_in_2");
	 new_graph->setPhasesQuantity(new_vertex,1);
	 new_graph->setReentrancyFactor(new_vertex,1);
	 new_graph->setVertexDuration(new_vertex,{1.78487e+07});
	}
	{
	std::cout << "generate a task ..." << std::endl;
	auto new_vertex = new_graph->addVertex(3);
	 new_graph->setVertexName(new_vertex,"audio_out_3");
	 new_graph->setPhasesQuantity(new_vertex,1);
	 new_graph->setReentrancyFactor(new_vertex,1);
	 new_graph->setVertexDuration(new_vertex,{1.49562e+07});
	}
	{
	std::cout << "generate a task ..." << std::endl;
	auto new_vertex = new_graph->addVertex(4);
	 new_graph->setVertexName(new_vertex,"Dup_5");
	 new_graph->setPhasesQuantity(new_vertex,1);
	 new_graph->setReentrancyFactor(new_vertex,1);
	 new_graph->setVertexDuration(new_vertex,{255779});
	}
	{
	std::cout << "generate a task ..." << std::endl;
	auto new_vertex = new_graph->addVertex(5);
	 new_graph->setVertexName(new_vertex,"Dup_7");
	 new_graph->setPhasesQuantity(new_vertex,1);
	 new_graph->setReentrancyFactor(new_vertex,1);
	 new_graph->setVertexDuration(new_vertex,{3.84457e+06});
	}
	{
	std::cout << "generate a task ..." << std::endl;
	auto new_vertex = new_graph->addVertex(6);
	 new_graph->setVertexName(new_vertex,"Norm_factor_elem_optim_8");
	 new_graph->setPhasesQuantity(new_vertex,1);
	 new_graph->setReentrancyFactor(new_vertex,1);
	 new_graph->setVertexDuration(new_vertex,{1.66006e+06});
	}
	{
	std::cout << "generate a task ..." << std::endl;
	auto new_vertex = new_graph->addVertex(7);
	 new_graph->setVertexName(new_vertex,"Norm_factor_elem_optim_9");
	 new_graph->setPhasesQuantity(new_vertex,1);
	 new_graph->setReentrancyFactor(new_vertex,1);
	 new_graph->setVertexDuration(new_vertex,{1.45096e+06});
	}
	{
	std::cout << "generate a task ..." << std::endl;
	auto new_vertex = new_graph->addVertex(8);
	 new_graph->setVertexName(new_vertex,"Norm_factor_elem_optim_10");
	 new_graph->setPhasesQuantity(new_vertex,1);
	 new_graph->setReentrancyFactor(new_vertex,1);
	 new_graph->setVertexDuration(new_vertex,{1.98053e+06});
	}
	{
	std::cout << "generate a task ..." << std::endl;
	auto new_vertex = new_graph->addVertex(9);
	 new_graph->setVertexName(new_vertex,"Norm_factor_elem_optim_11");
	 new_graph->setPhasesQuantity(new_vertex,1);
	 new_graph->setReentrancyFactor(new_vertex,1);
	 new_graph->setVertexDuration(new_vertex,{1.95366e+06});
	}
	{
	std::cout << "generate a task ..." << std::endl;
	auto new_vertex = new_graph->addVertex(10);
	 new_graph->setVertexName(new_vertex,"Norm_factor_elem_optim_12");
	 new_graph->setPhasesQuantity(new_vertex,1);
	 new_graph->setReentrancyFactor(new_vertex,1);
	 new_graph->setVertexDuration(new_vertex,{1.47265e+06});
	}
	{
	std::cout << "generate a task ..." << std::endl;
	auto new_vertex = new_graph->addVertex(11);
	 new_graph->setVertexName(new_vertex,"Norm_factor_elem_optim_13");
	 new_graph->setPhasesQuantity(new_vertex,1);
	 new_graph->setReentrancyFactor(new_vertex,1);
	 new_graph->setVertexDuration(new_vertex,{1.96723e+06});
	}
	{
	std::cout << "generate a task ..." << std::endl;
	auto new_vertex = new_graph->addVertex(12);
	 new_graph->setVertexName(new_vertex,"Norm_factor_elem_optim_14");
	 new_graph->setPhasesQuantity(new_vertex,1);
	 new_graph->setReentrancyFactor(new_vertex,1);
	 new_graph->setVertexDuration(new_vertex,{1.3753e+06});
	}
	{
	std::cout << "generate a task ..." << std::endl;
	auto new_vertex = new_graph->addVertex(13);
	 new_graph->setVertexName(new_vertex,"Norm_factor_elem_optim_15");
	 new_graph->setPhasesQuantity(new_vertex,1);
	 new_graph->setReentrancyFactor(new_vertex,1);
	 new_graph->setVertexDuration(new_vertex,{2.14934e+06});
	}
	{
	std::cout << "generate a task ..." << std::endl;
	auto new_vertex = new_graph->addVertex(14);
	 new_graph->setVertexName(new_vertex,"Sum_Invert_16");
	 new_graph->setPhasesQuantity(new_vertex,1);
	 new_graph->setReentrancyFactor(new_vertex,1);
	 new_graph->setVertexDuration(new_vertex,{384939});
	}
	{
	std::cout << "generate a task ..." << std::endl;
	auto new_vertex = new_graph->addVertex(15);
	 new_graph->setVertexName(new_vertex,"Dup_18");
	 new_graph->setPhasesQuantity(new_vertex,1);
	 new_graph->setReentrancyFactor(new_vertex,1);
	 new_graph->setVertexDuration(new_vertex,{620475});
	}
	{
	std::cout << "generate a task ..." << std::endl;
	auto new_vertex = new_graph->addVertex(16);
	 new_graph->setVertexName(new_vertex,"Wfilter_elem_19");
	 new_graph->setPhasesQuantity(new_vertex,1);
	 new_graph->setReentrancyFactor(new_vertex,1);
	 new_graph->setVertexDuration(new_vertex,{457890});
	}
	{
	std::cout << "generate a task ..." << std::endl;
	auto new_vertex = new_graph->addVertex(17);
	 new_graph->setVertexName(new_vertex,"Wfilter_elem_20");
	 new_graph->setPhasesQuantity(new_vertex,1);
	 new_graph->setReentrancyFactor(new_vertex,1);
	 new_graph->setVertexDuration(new_vertex,{421128});
	}
	{
	std::cout << "generate a task ..." << std::endl;
	auto new_vertex = new_graph->addVertex(18);
	 new_graph->setVertexName(new_vertex,"Wfilter_elem_21");
	 new_graph->setPhasesQuantity(new_vertex,1);
	 new_graph->setReentrancyFactor(new_vertex,1);
	 new_graph->setVertexDuration(new_vertex,{247453});
	}
	{
	std::cout << "generate a task ..." << std::endl;
	auto new_vertex = new_graph->addVertex(19);
	 new_graph->setVertexName(new_vertex,"Wfilter_elem_22");
	 new_graph->setPhasesQuantity(new_vertex,1);
	 new_graph->setReentrancyFactor(new_vertex,1);
	 new_graph->setVertexDuration(new_vertex,{435181});
	}
	{
	std::cout << "generate a task ..." << std::endl;
	auto new_vertex = new_graph->addVertex(20);
	 new_graph->setVertexName(new_vertex,"Wfilter_elem_23");
	 new_graph->setPhasesQuantity(new_vertex,1);
	 new_graph->setReentrancyFactor(new_vertex,1);
	 new_graph->setVertexDuration(new_vertex,{422413});
	}
	{
	std::cout << "generate a task ..." << std::endl;
	auto new_vertex = new_graph->addVertex(21);
	 new_graph->setVertexName(new_vertex,"Wfilter_elem_24");
	 new_graph->setPhasesQuantity(new_vertex,1);
	 new_graph->setReentrancyFactor(new_vertex,1);
	 new_graph->setVertexDuration(new_vertex,{185566});
	}
	{
	std::cout << "generate a task ..." << std::endl;
	auto new_vertex = new_graph->addVertex(22);
	 new_graph->setVertexName(new_vertex,"Wfilter_elem_25");
	 new_graph->setPhasesQuantity(new_vertex,1);
	 new_graph->setReentrancyFactor(new_vertex,1);
	 new_graph->setVertexDuration(new_vertex,{413265});
	}
	{
	std::cout << "generate a task ..." << std::endl;
	auto new_vertex = new_graph->addVertex(23);
	 new_graph->setVertexName(new_vertex,"Wfilter_elem_26");
	 new_graph->setPhasesQuantity(new_vertex,1);
	 new_graph->setReentrancyFactor(new_vertex,1);
	 new_graph->setVertexDuration(new_vertex,{622419});
	}
	{
	std::cout << "generate a task ..." << std::endl;
	auto new_vertex = new_graph->addVertex(24);
	 new_graph->setVertexName(new_vertex,"Dup_27");
	 new_graph->setPhasesQuantity(new_vertex,1);
	 new_graph->setReentrancyFactor(new_vertex,1);
	 new_graph->setVertexDuration(new_vertex,{323679});
	}
	{
	std::cout << "generate a task ..." << std::endl;
	auto new_vertex = new_graph->addVertex(25);
	 new_graph->setVertexName(new_vertex,"Dup_29");
	 new_graph->setPhasesQuantity(new_vertex,1);
	 new_graph->setReentrancyFactor(new_vertex,1);
	 new_graph->setVertexDuration(new_vertex,{306939});
	}
	{
	std::cout << "generate a task ..." << std::endl;
	auto new_vertex = new_graph->addVertex(26);
	 new_graph->setVertexName(new_vertex,"error_calculation_30");
	 new_graph->setPhasesQuantity(new_vertex,1);
	 new_graph->setReentrancyFactor(new_vertex,1);
	 new_graph->setVertexDuration(new_vertex,{419920});
	}
	{
	std::cout << "generate a task ..." << std::endl;
	auto new_vertex = new_graph->addVertex(27);
	 new_graph->setVertexName(new_vertex,"Dup_32");
	 new_graph->setPhasesQuantity(new_vertex,1);
	 new_graph->setReentrancyFactor(new_vertex,1);
	 new_graph->setVertexDuration(new_vertex,{583601});
	}
	{
	std::cout << "generate a task ..." << std::endl;
	auto new_vertex = new_graph->addVertex(28);
	 new_graph->setVertexName(new_vertex,"Dup_33");
	 new_graph->setPhasesQuantity(new_vertex,1);
	 new_graph->setReentrancyFactor(new_vertex,1);
	 new_graph->setVertexDuration(new_vertex,{1.15223e+06});
	}
	{
	std::cout << "generate a task ..." << std::endl;
	auto new_vertex = new_graph->addVertex(29);
	 new_graph->setVertexName(new_vertex,"Dup_34");
	 new_graph->setPhasesQuantity(new_vertex,1);
	 new_graph->setReentrancyFactor(new_vertex,1);
	 new_graph->setVertexDuration(new_vertex,{501488});
	}
	{
	std::cout << "generate a task ..." << std::endl;
	auto new_vertex = new_graph->addVertex(30);
	 new_graph->setVertexName(new_vertex,"Wupdate_elem_35");
	 new_graph->setPhasesQuantity(new_vertex,1);
	 new_graph->setReentrancyFactor(new_vertex,1);
	 new_graph->setVertexDuration(new_vertex,{508300});
	}
	{
	std::cout << "generate a task ..." << std::endl;
	auto new_vertex = new_graph->addVertex(31);
	 new_graph->setVertexName(new_vertex,"Wupdate_elem_36");
	 new_graph->setPhasesQuantity(new_vertex,1);
	 new_graph->setReentrancyFactor(new_vertex,1);
	 new_graph->setVertexDuration(new_vertex,{255688});
	}
	{
	std::cout << "generate a task ..." << std::endl;
	auto new_vertex = new_graph->addVertex(32);
	 new_graph->setVertexName(new_vertex,"Wupdate_elem_37");
	 new_graph->setPhasesQuantity(new_vertex,1);
	 new_graph->setReentrancyFactor(new_vertex,1);
	 new_graph->setVertexDuration(new_vertex,{335501});
	}
	{
	std::cout << "generate a task ..." << std::endl;
	auto new_vertex = new_graph->addVertex(33);
	 new_graph->setVertexName(new_vertex,"Wupdate_elem_38");
	 new_graph->setPhasesQuantity(new_vertex,1);
	 new_graph->setReentrancyFactor(new_vertex,1);
	 new_graph->setVertexDuration(new_vertex,{337028});
	}
	{
	std::cout << "generate a task ..." << std::endl;
	auto new_vertex = new_graph->addVertex(34);
	 new_graph->setVertexName(new_vertex,"Wupdate_elem_39");
	 new_graph->setPhasesQuantity(new_vertex,1);
	 new_graph->setReentrancyFactor(new_vertex,1);
	 new_graph->setVertexDuration(new_vertex,{429331});
	}
	{
	std::cout << "generate a task ..." << std::endl;
	auto new_vertex = new_graph->addVertex(35);
	 new_graph->setVertexName(new_vertex,"Wupdate_elem_40");
	 new_graph->setPhasesQuantity(new_vertex,1);
	 new_graph->setReentrancyFactor(new_vertex,1);
	 new_graph->setVertexDuration(new_vertex,{385595});
	}
	{
	std::cout << "generate a task ..." << std::endl;
	auto new_vertex = new_graph->addVertex(36);
	 new_graph->setVertexName(new_vertex,"Wupdate_elem_41");
	 new_graph->setPhasesQuantity(new_vertex,1);
	 new_graph->setReentrancyFactor(new_vertex,1);
	 new_graph->setVertexDuration(new_vertex,{414125});
	}
	{
	std::cout << "generate a task ..." << std::endl;
	auto new_vertex = new_graph->addVertex(37);
	 new_graph->setVertexName(new_vertex,"Wupdate_elem_42");
	 new_graph->setPhasesQuantity(new_vertex,1);
	 new_graph->setReentrancyFactor(new_vertex,1);
	 new_graph->setVertexDuration(new_vertex,{354902});
	}
	{
	std::cout << "generate a task ..." << std::endl;
	auto new_vertex = new_graph->addVertex(38);
	 new_graph->setVertexName(new_vertex,"Join_43");
	 new_graph->setPhasesQuantity(new_vertex,8);
	 new_graph->setReentrancyFactor(new_vertex,1);
	 new_graph->setVertexDuration(new_vertex,{361639,222689,362437,380756,268630,228866,186859,102795});
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(5), new_graph->getVertexById(6));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_0");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(6), new_graph->getVertexById(14));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_1");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(5), new_graph->getVertexById(7));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_2");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(7), new_graph->getVertexById(14));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_3");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(5), new_graph->getVertexById(8));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_4");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(8), new_graph->getVertexById(14));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_5");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(5), new_graph->getVertexById(9));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_6");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(9), new_graph->getVertexById(14));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_7");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(5), new_graph->getVertexById(10));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_8");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(10), new_graph->getVertexById(14));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_9");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(5), new_graph->getVertexById(11));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_10");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(11), new_graph->getVertexById(14));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_11");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(5), new_graph->getVertexById(12));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_12");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(12), new_graph->getVertexById(14));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_13");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(5), new_graph->getVertexById(13));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_14");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(13), new_graph->getVertexById(14));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_15");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(24), new_graph->getVertexById(16));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_16");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(24), new_graph->getVertexById(17));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_17");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(24), new_graph->getVertexById(18));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_18");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(24), new_graph->getVertexById(19));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_19");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(24), new_graph->getVertexById(20));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_20");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(24), new_graph->getVertexById(21));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_21");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(24), new_graph->getVertexById(22));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_22");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(24), new_graph->getVertexById(23));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_23");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(15), new_graph->getVertexById(16));
	 new_graph->setEdgeInPhases(new_edge,{2496});
	 new_graph->setEdgeOutPhases(new_edge,{2496});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_24");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(15), new_graph->getVertexById(17));
	 new_graph->setEdgeInPhases(new_edge,{2496});
	 new_graph->setEdgeOutPhases(new_edge,{2496});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_25");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(15), new_graph->getVertexById(18));
	 new_graph->setEdgeInPhases(new_edge,{2496});
	 new_graph->setEdgeOutPhases(new_edge,{2496});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_26");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(15), new_graph->getVertexById(19));
	 new_graph->setEdgeInPhases(new_edge,{2496});
	 new_graph->setEdgeOutPhases(new_edge,{2496});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_27");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(15), new_graph->getVertexById(20));
	 new_graph->setEdgeInPhases(new_edge,{2496});
	 new_graph->setEdgeOutPhases(new_edge,{2496});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_28");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(15), new_graph->getVertexById(21));
	 new_graph->setEdgeInPhases(new_edge,{2496});
	 new_graph->setEdgeOutPhases(new_edge,{2496});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_29");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(15), new_graph->getVertexById(22));
	 new_graph->setEdgeInPhases(new_edge,{2496});
	 new_graph->setEdgeOutPhases(new_edge,{2496});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_30");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(15), new_graph->getVertexById(23));
	 new_graph->setEdgeInPhases(new_edge,{2496});
	 new_graph->setEdgeOutPhases(new_edge,{2496});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_31");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(27), new_graph->getVertexById(30));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_32");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(27), new_graph->getVertexById(31));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_33");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(27), new_graph->getVertexById(32));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_34");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(27), new_graph->getVertexById(33));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_35");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(27), new_graph->getVertexById(34));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_36");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(27), new_graph->getVertexById(35));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_37");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(27), new_graph->getVertexById(36));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_38");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(27), new_graph->getVertexById(37));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_39");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(28), new_graph->getVertexById(30));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_40");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(28), new_graph->getVertexById(31));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_41");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(28), new_graph->getVertexById(32));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_42");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(28), new_graph->getVertexById(33));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_43");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(28), new_graph->getVertexById(34));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_44");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(28), new_graph->getVertexById(35));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_45");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(28), new_graph->getVertexById(36));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_46");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(28), new_graph->getVertexById(37));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_47");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(29), new_graph->getVertexById(30));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_48");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(29), new_graph->getVertexById(31));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_49");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(29), new_graph->getVertexById(32));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_50");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(29), new_graph->getVertexById(33));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_51");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(29), new_graph->getVertexById(34));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_52");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(29), new_graph->getVertexById(35));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_53");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(29), new_graph->getVertexById(36));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_54");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(29), new_graph->getVertexById(37));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_55");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(30), new_graph->getVertexById(38));
	 new_graph->setEdgeInPhases(new_edge,{312});
	 new_graph->setEdgeOutPhases(new_edge,{312,0,0,0,0,0,0,0});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_56");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(31), new_graph->getVertexById(38));
	 new_graph->setEdgeInPhases(new_edge,{312});
	 new_graph->setEdgeOutPhases(new_edge,{0,312,0,0,0,0,0,0});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_57");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(32), new_graph->getVertexById(38));
	 new_graph->setEdgeInPhases(new_edge,{312});
	 new_graph->setEdgeOutPhases(new_edge,{0,0,312,0,0,0,0,0});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_58");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(33), new_graph->getVertexById(38));
	 new_graph->setEdgeInPhases(new_edge,{312});
	 new_graph->setEdgeOutPhases(new_edge,{0,0,0,312,0,0,0,0});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_59");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(34), new_graph->getVertexById(38));
	 new_graph->setEdgeInPhases(new_edge,{312});
	 new_graph->setEdgeOutPhases(new_edge,{0,0,0,0,312,0,0,0});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_60");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(35), new_graph->getVertexById(38));
	 new_graph->setEdgeInPhases(new_edge,{312});
	 new_graph->setEdgeOutPhases(new_edge,{0,0,0,0,0,312,0,0});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_61");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(36), new_graph->getVertexById(38));
	 new_graph->setEdgeInPhases(new_edge,{312});
	 new_graph->setEdgeOutPhases(new_edge,{0,0,0,0,0,0,312,0});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_62");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(37), new_graph->getVertexById(38));
	 new_graph->setEdgeInPhases(new_edge,{312});
	 new_graph->setEdgeOutPhases(new_edge,{0,0,0,0,0,0,0,312});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_63");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(26), new_graph->getVertexById(25));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_64");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(25), new_graph->getVertexById(29));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_65");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(4), new_graph->getVertexById(24));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_66");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(4), new_graph->getVertexById(5));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_67");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(4), new_graph->getVertexById(27));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_68");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(38), new_graph->getVertexById(15));
	 new_graph->setEdgeInPhases(new_edge,{312,312,312,312,312,312,312,312});
	 new_graph->setEdgeOutPhases(new_edge,{2496});
	 new_graph->setPreload(new_edge,2496);
	 new_graph->setEdgeName(new_edge,"channel_69");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(14), new_graph->getVertexById(28));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_70");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(16), new_graph->getVertexById(26));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_71");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(17), new_graph->getVertexById(26));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_72");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(18), new_graph->getVertexById(26));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_73");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(19), new_graph->getVertexById(26));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_74");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(20), new_graph->getVertexById(26));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_75");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(21), new_graph->getVertexById(26));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_76");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(22), new_graph->getVertexById(26));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_77");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(23), new_graph->getVertexById(26));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_78");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(1), new_graph->getVertexById(4));
	 new_graph->setEdgeInPhases(new_edge,{1000});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_79");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(2), new_graph->getVertexById(26));
	 new_graph->setEdgeInPhases(new_edge,{1000});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_80");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(25), new_graph->getVertexById(3));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1000});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_81");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(6), new_graph->getVertexById(5));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_0_prime");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(14), new_graph->getVertexById(6));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_1_prime");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(7), new_graph->getVertexById(5));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_2_prime");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(14), new_graph->getVertexById(7));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_3_prime");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(8), new_graph->getVertexById(5));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_4_prime");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(14), new_graph->getVertexById(8));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_5_prime");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(9), new_graph->getVertexById(5));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_6_prime");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(14), new_graph->getVertexById(9));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_7_prime");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(10), new_graph->getVertexById(5));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_8_prime");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(14), new_graph->getVertexById(10));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_9_prime");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(11), new_graph->getVertexById(5));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_10_prime");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(14), new_graph->getVertexById(11));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_11_prime");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(12), new_graph->getVertexById(5));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_12_prime");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(14), new_graph->getVertexById(12));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_13_prime");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(13), new_graph->getVertexById(5));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_14_prime");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(14), new_graph->getVertexById(13));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_15_prime");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(16), new_graph->getVertexById(24));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_16_prime");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(17), new_graph->getVertexById(24));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_17_prime");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(18), new_graph->getVertexById(24));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_18_prime");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(19), new_graph->getVertexById(24));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_19_prime");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(20), new_graph->getVertexById(24));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_20_prime");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(21), new_graph->getVertexById(24));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_21_prime");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(22), new_graph->getVertexById(24));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_22_prime");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(23), new_graph->getVertexById(24));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_23_prime");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(16), new_graph->getVertexById(15));
	 new_graph->setEdgeInPhases(new_edge,{2496});
	 new_graph->setEdgeOutPhases(new_edge,{2496});
	 new_graph->setPreload(new_edge,2496);
	 new_graph->setEdgeName(new_edge,"channel_24_prime");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(17), new_graph->getVertexById(15));
	 new_graph->setEdgeInPhases(new_edge,{2496});
	 new_graph->setEdgeOutPhases(new_edge,{2496});
	 new_graph->setPreload(new_edge,2496);
	 new_graph->setEdgeName(new_edge,"channel_25_prime");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(18), new_graph->getVertexById(15));
	 new_graph->setEdgeInPhases(new_edge,{2496});
	 new_graph->setEdgeOutPhases(new_edge,{2496});
	 new_graph->setPreload(new_edge,2496);
	 new_graph->setEdgeName(new_edge,"channel_26_prime");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(19), new_graph->getVertexById(15));
	 new_graph->setEdgeInPhases(new_edge,{2496});
	 new_graph->setEdgeOutPhases(new_edge,{2496});
	 new_graph->setPreload(new_edge,2496);
	 new_graph->setEdgeName(new_edge,"channel_27_prime");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(20), new_graph->getVertexById(15));
	 new_graph->setEdgeInPhases(new_edge,{2496});
	 new_graph->setEdgeOutPhases(new_edge,{2496});
	 new_graph->setPreload(new_edge,2496);
	 new_graph->setEdgeName(new_edge,"channel_28_prime");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(21), new_graph->getVertexById(15));
	 new_graph->setEdgeInPhases(new_edge,{2496});
	 new_graph->setEdgeOutPhases(new_edge,{2496});
	 new_graph->setPreload(new_edge,2496);
	 new_graph->setEdgeName(new_edge,"channel_29_prime");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(22), new_graph->getVertexById(15));
	 new_graph->setEdgeInPhases(new_edge,{2496});
	 new_graph->setEdgeOutPhases(new_edge,{2496});
	 new_graph->setPreload(new_edge,2496);
	 new_graph->setEdgeName(new_edge,"channel_30_prime");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(23), new_graph->getVertexById(15));
	 new_graph->setEdgeInPhases(new_edge,{2496});
	 new_graph->setEdgeOutPhases(new_edge,{2496});
	 new_graph->setPreload(new_edge,2496);
	 new_graph->setEdgeName(new_edge,"channel_31_prime");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(30), new_graph->getVertexById(27));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_32_prime");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(31), new_graph->getVertexById(27));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_33_prime");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(32), new_graph->getVertexById(27));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_34_prime");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(33), new_graph->getVertexById(27));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_35_prime");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(34), new_graph->getVertexById(27));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_36_prime");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(35), new_graph->getVertexById(27));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_37_prime");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(36), new_graph->getVertexById(27));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_38_prime");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(37), new_graph->getVertexById(27));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_39_prime");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(30), new_graph->getVertexById(28));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_40_prime");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(31), new_graph->getVertexById(28));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_41_prime");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(32), new_graph->getVertexById(28));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_42_prime");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(33), new_graph->getVertexById(28));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_43_prime");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(34), new_graph->getVertexById(28));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_44_prime");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(35), new_graph->getVertexById(28));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_45_prime");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(36), new_graph->getVertexById(28));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_46_prime");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(37), new_graph->getVertexById(28));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_47_prime");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(30), new_graph->getVertexById(29));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_48_prime");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(31), new_graph->getVertexById(29));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_49_prime");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(32), new_graph->getVertexById(29));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_50_prime");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(33), new_graph->getVertexById(29));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_51_prime");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(34), new_graph->getVertexById(29));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_52_prime");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(35), new_graph->getVertexById(29));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_53_prime");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(36), new_graph->getVertexById(29));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_54_prime");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(37), new_graph->getVertexById(29));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_55_prime");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(38), new_graph->getVertexById(30));
	 new_graph->setEdgeInPhases(new_edge,{312,0,0,0,0,0,0,0});
	 new_graph->setEdgeOutPhases(new_edge,{312});
	 new_graph->setPreload(new_edge,312);
	 new_graph->setEdgeName(new_edge,"channel_56_prime");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(38), new_graph->getVertexById(31));
	 new_graph->setEdgeInPhases(new_edge,{0,312,0,0,0,0,0,0});
	 new_graph->setEdgeOutPhases(new_edge,{312});
	 new_graph->setPreload(new_edge,312);
	 new_graph->setEdgeName(new_edge,"channel_57_prime");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(38), new_graph->getVertexById(32));
	 new_graph->setEdgeInPhases(new_edge,{0,0,312,0,0,0,0,0});
	 new_graph->setEdgeOutPhases(new_edge,{312});
	 new_graph->setPreload(new_edge,312);
	 new_graph->setEdgeName(new_edge,"channel_58_prime");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(38), new_graph->getVertexById(33));
	 new_graph->setEdgeInPhases(new_edge,{0,0,0,312,0,0,0,0});
	 new_graph->setEdgeOutPhases(new_edge,{312});
	 new_graph->setPreload(new_edge,312);
	 new_graph->setEdgeName(new_edge,"channel_59_prime");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(38), new_graph->getVertexById(34));
	 new_graph->setEdgeInPhases(new_edge,{0,0,0,0,312,0,0,0});
	 new_graph->setEdgeOutPhases(new_edge,{312});
	 new_graph->setPreload(new_edge,312);
	 new_graph->setEdgeName(new_edge,"channel_60_prime");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(38), new_graph->getVertexById(35));
	 new_graph->setEdgeInPhases(new_edge,{0,0,0,0,0,312,0,0});
	 new_graph->setEdgeOutPhases(new_edge,{312});
	 new_graph->setPreload(new_edge,312);
	 new_graph->setEdgeName(new_edge,"channel_61_prime");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(38), new_graph->getVertexById(36));
	 new_graph->setEdgeInPhases(new_edge,{0,0,0,0,0,0,312,0});
	 new_graph->setEdgeOutPhases(new_edge,{312});
	 new_graph->setPreload(new_edge,312);
	 new_graph->setEdgeName(new_edge,"channel_62_prime");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(38), new_graph->getVertexById(37));
	 new_graph->setEdgeInPhases(new_edge,{0,0,0,0,0,0,0,312});
	 new_graph->setEdgeOutPhases(new_edge,{312});
	 new_graph->setPreload(new_edge,312);
	 new_graph->setEdgeName(new_edge,"channel_63_prime");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(25), new_graph->getVertexById(26));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_64_prime");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(29), new_graph->getVertexById(25));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_65_prime");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(24), new_graph->getVertexById(4));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_66_prime");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(5), new_graph->getVertexById(4));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_67_prime");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(27), new_graph->getVertexById(4));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_68_prime");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(15), new_graph->getVertexById(38));
	 new_graph->setEdgeInPhases(new_edge,{2496});
	 new_graph->setEdgeOutPhases(new_edge,{312,312,312,312,312,312,312,312});
	 new_graph->setPreload(new_edge,0);
	 new_graph->setEdgeName(new_edge,"channel_69_prime");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(28), new_graph->getVertexById(14));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_70_prime");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(26), new_graph->getVertexById(16));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_71_prime");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(26), new_graph->getVertexById(17));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_72_prime");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(26), new_graph->getVertexById(18));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_73_prime");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(26), new_graph->getVertexById(19));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_74_prime");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(26), new_graph->getVertexById(20));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_75_prime");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(26), new_graph->getVertexById(21));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_76_prime");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(26), new_graph->getVertexById(22));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_77_prime");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(26), new_graph->getVertexById(23));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1);
	 new_graph->setEdgeName(new_edge,"channel_78_prime");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(4), new_graph->getVertexById(1));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1000});
	 new_graph->setPreload(new_edge,1000);
	 new_graph->setEdgeName(new_edge,"channel_79_prime");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(26), new_graph->getVertexById(2));
	 new_graph->setEdgeInPhases(new_edge,{1});
	 new_graph->setEdgeOutPhases(new_edge,{1000});
	 new_graph->setPreload(new_edge,1000);
	 new_graph->setEdgeName(new_edge,"channel_80_prime");
	}
	{
	std::cout << "generate a buffer ..." << std::endl;
	auto new_edge = new_graph->addEdge(new_graph->getVertexById(3), new_graph->getVertexById(25));
	 new_graph->setEdgeInPhases(new_edge,{1000});
	 new_graph->setEdgeOutPhases(new_edge,{1});
	 new_graph->setPreload(new_edge,1000);
	 new_graph->setEdgeName(new_edge,"channel_81_prime");
	}
	return new_graph;
}




#endif /* TESTS_HELPERS_ECHO_BUFFERED_H_ */
