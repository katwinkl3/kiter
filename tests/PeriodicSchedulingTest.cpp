/*
 * PeriodicSchedulingTest.cpp
 *
 *  Created on: Nov 6, 2019
 *      Author: toky
 */

#define BOOST_TEST_MODULE PeriodicSchedulingTest
#include "helpers/test_classes.h"


#include <commons/basic_types.h>
#include <printers/SDF3Wrapper.h>
#include <algorithms/schedulings.h>

// TODO : find a way to parametrize TEST_SUITE to try different scheduling techniques with the same tests.

BOOST_FIXTURE_TEST_SUITE( test_suite_one_periodic , WITH_VERBOSE)



BOOST_AUTO_TEST_CASE( test_scheduling_single_task_SDF )
{
    models::Dataflow* g = new models::Dataflow(0);

    auto v1 = g->addVertex("single");
    g->setPhasesQuantity(v1,1);
    g->setVertexDuration(v1,{1});

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
    //BOOST_REQUIRE_EQUAL( res2.getGraphThroughput(), commons::runSDF3Throughput(g) );


    delete g;

}



BOOST_AUTO_TEST_CASE( test_scheduling_single_task_CSDF )
{
    models::Dataflow* g = new models::Dataflow(0);

    auto v1 = g->addVertex("single");
    g->setPhasesQuantity(v1,2);
    g->setVertexDuration(v1,{1,1});

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
    //BOOST_REQUIRE_EQUAL( res2.getGraphThroughput(), commons::runSDF3Throughput(g) );

    delete g;

}

BOOST_AUTO_TEST_CASE( test_scheduling_simple_buffer_dataflow )
{
    models::Dataflow* g = new models::Dataflow(0);

    auto v1 = g->addVertex("one");
    g->setPhasesQuantity(v1,1);
    g->setVertexDuration(v1,{1});

    auto v2 = g->addVertex("two");
    g->setPhasesQuantity(v2,1);
    g->setVertexDuration(v2,{1});

    auto e1 = g->addEdge(v1,v2,"a");
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
    //BOOST_REQUIRE_EQUAL( res2.getGraphPeriod(), commons::runSDF3Throughput(g) );

    delete g;

}

BOOST_AUTO_TEST_SUITE_END()


