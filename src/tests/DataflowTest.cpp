/*
 * DataflowTest.cpp
 *
 *  Created on: Nov 6, 2019
 *      Author: toky
 */

#define BOOST_TEST_MODULE DataflowTest
#include <boost/test/included/unit_test.hpp>

#include <models/Dataflow.h>
#include <models/Dataflow.h>

struct F {
    F() {
    	BOOST_TEST_MESSAGE( "Setup Done" );
    }
    ~F()                    {
    	BOOST_TEST_MESSAGE( "Teardown Done" );
    }
};



BOOST_FIXTURE_TEST_SUITE( test_suite_create_dataflow , F)



BOOST_AUTO_TEST_CASE( test_simgle_task_dataflow )
{
    //models::Dataflow* g = new models::Dataflow(1);
    //BOOST_REQUIRE_EQUAL( g->getVerticesCount(), 1 );
    //BOOST_REQUIRE_EQUAL( g->getEdgesCount(), 0 );
}

BOOST_AUTO_TEST_SUITE_END()




BOOST_FIXTURE_TEST_SUITE( test_suite_modify_dataflow , F)

BOOST_AUTO_TEST_CASE( test_remove_vertex )
{
    BOOST_CHECK( true );
}

BOOST_AUTO_TEST_CASE( test_remove_edge )
{
    BOOST_CHECK( true );
}

BOOST_AUTO_TEST_SUITE_END()


