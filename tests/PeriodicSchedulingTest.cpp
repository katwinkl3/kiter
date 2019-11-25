/*
 * PeriodicSchedulingTest.cpp
 *
 *  Created on: Nov 6, 2019
 *      Author: toky
 */

#define BOOST_TEST_MODULE PeriodicSchedulingTest
#include "test_helper.h"

#include <algorithms/schedulings.h>
#include <algorithms/repetition_vector.h>

// TODO : find a way to parametrize TEST_SUITE to try different scheduling techniques with the same tests.

BOOST_FIXTURE_TEST_SUITE( test_suite_one_periodic , WITH_VERBOSE)



BOOST_AUTO_TEST_CASE( test_scheduling_simple_dataflow )
{
    models::Dataflow* g = new models::Dataflow(0);

    auto v1 = g->addVertex();
    g->setPhasesQuantity(v1,1);
    g->setVertexDuration(v1,{1});

    auto v2 = g->addVertex();
    g->setPhasesQuantity(v2,1);
    g->setVertexDuration(v2,{1});

    auto e1 = g->addEdge(v1,v2);
    g->setEdgeInPhases(e1,{1});
    g->setEdgeOutPhases(e1,{1});


    // Case of scheduling without Reentrancy
    // ==============================================

	VERBOSE_ASSERT(computeRepetitionVector(g),"Cannot generate repetition vector.");
	models::Scheduling res1 = algorithms::scheduling::CSDF_KPeriodicScheduling_LP    (g, algorithms::scheduling::generate1PeriodicVector(g));
    BOOST_REQUIRE_EQUAL( res1.getGraphPeriod(), 0 );


    // Case of scheduling with Reentrancy
    // ==============================================

    g->reset_computation();
    g->setReentrancyFactor(v1,1);

	VERBOSE_ASSERT(computeRepetitionVector(g),"Cannot generate repetition vector.");
	models::Scheduling res2 = algorithms::scheduling::CSDF_KPeriodicScheduling_LP    (g, algorithms::scheduling::generate1PeriodicVector(g));
    BOOST_REQUIRE_EQUAL( res2.getGraphPeriod(), 1 );

    delete g;

}

BOOST_AUTO_TEST_SUITE_END()


