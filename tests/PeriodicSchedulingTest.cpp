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

BOOST_FIXTURE_TEST_SUITE( test_suite_one_periodic , WITH_VERBOSE)



BOOST_AUTO_TEST_CASE( test_scheduling_empty_dataflow )
{
    models::Dataflow* g = new models::Dataflow(0);
	VERBOSE_ASSERT(computeRepetitionVector(g),"Cannot generate repetition vector.");
	models::Scheduling res = algorithms::scheduling::CSDF_KPeriodicScheduling_LP    (g, algorithms::scheduling::generate1PeriodicVector(g));
    BOOST_REQUIRE_EQUAL( res.getGraphPeriod(), 0 );

    delete g;

}

BOOST_AUTO_TEST_SUITE_END()


