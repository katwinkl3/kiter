/*
 * EventGraphTest.cpp
 *
 *  Created on: Nov 6, 2019
 *      Author: toky
 */

#define BOOST_TEST_MODULE EventGraphTest
#include <boost/test/included/unit_test.hpp>

#include <models/EventGraph.h>

struct F {
    F() {
    	commons::set_verbose_mode(commons::INFO_LEVEL);
    	BOOST_TEST_MESSAGE( "Setup Done" );
    }
    ~F()                    {
    	BOOST_TEST_MESSAGE( "Teardown Done" );
    }
};



BOOST_FIXTURE_TEST_SUITE( test_suite_create_eventgraph , F)



BOOST_AUTO_TEST_CASE( test_empty_eventGraph )
{
    models::EventGraph* eg = new models::EventGraph(0);

    VERBOSE_INFO("g->getVerticesCount() == " << eg->getConstraintsCount());
    BOOST_REQUIRE_EQUAL( eg->getConstraintsCount(), 0 );

    VERBOSE_INFO("g->getEdgesCount() == " << eg->getEventCount());
    BOOST_REQUIRE_EQUAL( eg->getEventCount(), 0 );

    delete eg;

}
BOOST_AUTO_TEST_SUITE_END()





