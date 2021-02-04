/*
 * SDF3WriterTest.cpp
 *
 *  Created on: Dec 3, 2019
 *      Author: toky
 */


#define BOOST_TEST_MODULE SDF3WriterTest
#include "helpers/test_classes.h"
#include "helpers/echo_buffered.h"
#include <models/Dataflow.h>
#include <algorithms/schedulings.h>
#include <printers/SDF3Wrapper.h>


inline models::Dataflow * generateReetrancySample () { // B and C have no reentrancy. A is.
	// Auto-generate by Kiter for Kiter
	//
	models::Dataflow* new_graph = new models::Dataflow();

	{
		auto new_vertex = new_graph->addVertex(1);
		new_graph->setVertexName(new_vertex,"a");
		new_graph->setPhasesQuantity(new_vertex,1);
		new_graph->setVertexDuration(new_vertex,{1});
	}
	{
		auto new_vertex = new_graph->addVertex(2);
		new_graph->setVertexName(new_vertex,"b");
		new_graph->setPhasesQuantity(new_vertex,1);
		new_graph->setReentrancyFactor(new_vertex,1);
		new_graph->setVertexDuration(new_vertex,{2});
	}
	{
		auto new_vertex = new_graph->addVertex(3);
		new_graph->setVertexName(new_vertex,"c");
		new_graph->setPhasesQuantity(new_vertex,1);
		new_graph->setReentrancyFactor(new_vertex,1);
		new_graph->setVertexDuration(new_vertex,{2});
	}
	{
		auto new_edge = new_graph->addEdge(new_graph->getVertexById(1), new_graph->getVertexById(2));
		new_graph->setEdgeInPhases(new_edge,{2});
		new_graph->setEdgeOutPhases(new_edge,{3});
		new_graph->setPreload(new_edge,0);
		new_graph->setEdgeName(new_edge,"alpha");
	}
	{
		auto new_edge = new_graph->addEdge(new_graph->getVertexById(2), new_graph->getVertexById(3));
		new_graph->setEdgeInPhases(new_edge,{1});
		new_graph->setEdgeOutPhases(new_edge,{2});
		new_graph->setPreload(new_edge,0);
		new_graph->setEdgeName(new_edge,"beta");
	}

	return new_graph;
}



BOOST_FIXTURE_TEST_SUITE( sdf3_writer_test , WITH_VERBOSE)

BOOST_AUTO_TEST_CASE( test_echo )
{
	models::Dataflow* echo =  generateEchoBuffered () ;
	std::string echoStr = printers::generateSDF3XML(echo);
	models::Dataflow* echobis = printers::parseSDF3XML(echoStr);
	std::string echoStrBis = printers::generateSDF3XML(echobis);



	BOOST_REQUIRE_EQUAL( echoStr, echoStrBis );
	delete echo;
	delete echobis;
}
BOOST_AUTO_TEST_CASE( test_reentrancy )
{
	models::Dataflow* sample =  generateReetrancySample () ;
	std::string sampleStr = printers::generateSDF3XML(sample);
	models::Dataflow* samplebis = printers::parseSDF3XML(sampleStr);
	std::string sampleStrBis = printers::generateSDF3XML(samplebis);



	BOOST_REQUIRE_EQUAL( sampleStr, sampleStrBis );
	delete sample;
	delete samplebis;
}


BOOST_AUTO_TEST_SUITE_END()






