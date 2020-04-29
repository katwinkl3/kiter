/*
 * UserCaseTest.cpp
 *
 *  Created on: Dec 3, 2019
 *      Author: toky
 */


#define BOOST_TEST_MODULE TemplateTest
#include "helpers/test_classes.h"
#include "helpers/echo_buffered.h"
#include <models/Dataflow.h>
#include <algorithms/schedulings.h>
#include <commons/SDF3Wrapper.h>

BOOST_FIXTURE_TEST_SUITE( use_case_test , WITH_VERBOSE)

BOOST_AUTO_TEST_CASE( echo_buffered_kperiodic_scheduling )
{
	models::Dataflow* echo =  generateEchoBuffered () ;
	models::Scheduling res = algorithms::scheduling::CSDF_KPeriodicScheduling    (echo);
    //BOOST_REQUIRE_EQUAL( res.getGraphPeriod(), commons::runSDF3Throughput(echo) );
}

BOOST_AUTO_TEST_SUITE_END()






