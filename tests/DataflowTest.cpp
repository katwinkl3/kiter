/*
 * DataflowTest.cpp
 *
 *  Created on: Nov 6, 2019
 *      Author: toky
 */

#define BOOST_TEST_MODULE DataflowTest
#include <boost/test/included/unit_test.hpp>

#include <commons/verbose.h>
#include <models/Dataflow.h>
#include <printers/stdout.h>

struct F {
    F() {
    	commons::set_verbose_mode(commons::INFO_LEVEL);
    	BOOST_TEST_MESSAGE( "Setup Done" );
    }
    ~F()                    {
    	BOOST_TEST_MESSAGE( "Teardown Done" );
    }
};



BOOST_FIXTURE_TEST_SUITE( test_suite_create_dataflow , F)



BOOST_AUTO_TEST_CASE( test_empty_dataflow )
{
    models::Dataflow* g = new models::Dataflow(0);

    VERBOSE_INFO("g->getVerticesCount() == " << g->getVerticesCount());
    BOOST_REQUIRE_EQUAL( g->getVerticesCount(), 0 );

    VERBOSE_INFO("g->getEdgesCount() == " << g->getEdgesCount());
    BOOST_REQUIRE_EQUAL( g->getEdgesCount(), 0 );

    delete g;

}


BOOST_AUTO_TEST_CASE( test_one_buffer_dataflow )
{
    models::Dataflow* g = new models::Dataflow(0);
    auto v1 = g->addVertex();
    auto v2 = g->addVertex();
    g->addEdge(v1,v2);

    VERBOSE_INFO("g->getVerticesCount() == " << g->getVerticesCount());
    BOOST_REQUIRE_EQUAL( g->getVerticesCount(), 2 );

    VERBOSE_INFO("g->getEdgesCount() == " << g->getEdgesCount());
    BOOST_REQUIRE_EQUAL( g->getEdgesCount(), 1 );

    delete g;

}

BOOST_AUTO_TEST_SUITE_END()




BOOST_FIXTURE_TEST_SUITE( test_suite_modify_dataflow , F)

BOOST_AUTO_TEST_CASE( test_remove_vertex )
{
	 models::Dataflow* g = new models::Dataflow(0);
	 auto v1 = g->addVertex();
	 auto v2 = g->addVertex();

	 VERBOSE_INFO("g->getVerticesCount() == " << g->getVerticesCount());
	 BOOST_REQUIRE_EQUAL( g->getVerticesCount(), 2 );

	 g->removeVertex(v1);

	 VERBOSE_INFO("g->getVerticesCount() == " << g->getVerticesCount());
	 BOOST_REQUIRE_EQUAL( g->getVerticesCount(), 1 );

	 g->removeVertex(v2);

	 VERBOSE_INFO("g->getVerticesCount() == " << g->getVerticesCount());
	 BOOST_REQUIRE_EQUAL( g->getVerticesCount(), 0 );

	 delete g;

}

BOOST_AUTO_TEST_CASE( test_remove_edge )
{
	    models::Dataflow* g = new models::Dataflow(0);
	    auto v1 = g->addVertex();
	    auto v2 = g->addVertex();
	    auto e1 = g->addEdge(v1,v2);

	    VERBOSE_INFO("g->getVerticesCount() == " << g->getVerticesCount());
	    BOOST_REQUIRE_EQUAL( g->getVerticesCount(), 2 );

	    VERBOSE_INFO("g->getEdgesCount() == " << g->getEdgesCount());
	    BOOST_REQUIRE_EQUAL( g->getEdgesCount(), 1 );

	    g->removeEdge(e1);


	    VERBOSE_INFO("g->getVerticesCount() == " << g->getVerticesCount());
	    BOOST_REQUIRE_EQUAL( g->getVerticesCount(), 2 );

	    VERBOSE_INFO("g->getEdgesCount() == " << g->getEdgesCount());
	    BOOST_REQUIRE_EQUAL( g->getEdgesCount(), 0 );


	    delete g;
}

BOOST_AUTO_TEST_CASE( test_remove_vertex_connected_once )
{
	    models::Dataflow* g = new models::Dataflow(0);
	    auto v1 = g->addVertex();
	    auto v2 = g->addVertex();
	    auto e1 = g->addEdge(v1,v2);

	    VERBOSE_INFO("g->getVerticesCount() == " << g->getVerticesCount());
	    BOOST_REQUIRE_EQUAL( g->getVerticesCount(), 2 );

	    VERBOSE_INFO("g->getEdgesCount() == " << g->getEdgesCount());
	    BOOST_REQUIRE_EQUAL( g->getEdgesCount(), 1 );

	    g->removeVertex(v1);

	    VERBOSE_INFO("g->getVerticesCount() == " << g->getVerticesCount());
	    BOOST_REQUIRE_EQUAL( g->getVerticesCount(), 1 );

	    VERBOSE_INFO("g->getEdgesCount() == " << g->getEdgesCount());
	    BOOST_REQUIRE_EQUAL( g->getEdgesCount(), 0 );


	    delete g;
}


BOOST_AUTO_TEST_CASE( test_remove_vertex_connected_more )
{
	    models::Dataflow* g = new models::Dataflow(0);
	    auto v1 = g->addVertex();
	    auto v2 = g->addVertex();
	    auto v3 = g->addVertex();
	    auto v4 = g->addVertex();
	    auto v5 = g->addVertex();
	    auto e1 = g->addEdge(v1,v2);
	    auto e2 = g->addEdge(v2,v3);
	    auto e3 = g->addEdge(v3,v4);
	    auto e4 = g->addEdge(v2,v4);
	    auto e5 = g->addEdge(v4,v1);
	    auto e6 = g->addEdge(v4,v5);

	    VERBOSE_INFO("g->getVerticesCount() == " << g->getVerticesCount());
	    BOOST_REQUIRE_EQUAL( g->getVerticesCount(), 5 );

	    VERBOSE_INFO("g->getEdgesCount() == " << g->getEdgesCount());
	    BOOST_REQUIRE_EQUAL( g->getEdgesCount(), 6 );

	    g->removeVertex(v4);

	    VERBOSE_INFO("g->getVerticesCount() == " << g->getVerticesCount());
	    BOOST_REQUIRE_EQUAL( g->getVerticesCount(), 4 );

	    VERBOSE_INFO("g->getEdgesCount() == " << g->getEdgesCount());
	    BOOST_REQUIRE_EQUAL( g->getEdgesCount(), 2 );


	    delete g;
}

BOOST_AUTO_TEST_SUITE_END()


