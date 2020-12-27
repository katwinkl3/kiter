/*
 * UserCaseTest.cpp
 *
 *  Created on: Dec 3, 2019
 *      Author: toky
 */


#define BOOST_TEST_MODULE UseCaseTest
#include "helpers/test_classes.h"
#include "helpers/echo_buffered.h"
#include <models/Dataflow.h>
#include <algorithms/schedulings.h>
#include <printers/SDF3Wrapper.h>

BOOST_FIXTURE_TEST_SUITE( use_case_test , WITH_VERBOSE)

BOOST_AUTO_TEST_CASE( generate_echo )
{
	models::Dataflow* echo =  generateEchoBuffered () ;
	// models::Scheduling res = algorithms::scheduling::CSDF_KPeriodicScheduling    (echo);
	delete echo;
}

BOOST_AUTO_TEST_CASE( one_periodic_schedule_echo )
{
	models::Dataflow* echo =  generateEchoBuffered () ;
	//models::Scheduling res = algorithms::scheduling::CSDF_KPeriodicScheduling    (echo);
	delete echo;

}

BOOST_AUTO_TEST_SUITE_END()






