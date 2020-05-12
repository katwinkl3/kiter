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
#include <commons/SDF3Wrapper.h>

BOOST_FIXTURE_TEST_SUITE( sdf3_writer_test , WITH_VERBOSE)

BOOST_AUTO_TEST_CASE( test_echo )
{
	models::Dataflow* echo =  generateEchoBuffered () ;
	std::string echoStr = commons::generateSDF3XML(echo);
	models::Dataflow* echobis = commons::parseSDF3XML(echoStr);
	std::string echoStrBis = commons::generateSDF3XML(echobis);



	BOOST_REQUIRE_EQUAL( echoStr, echoStrBis );
	// models::Scheduling res = algorithms::scheduling::CSDF_KPeriodicScheduling    (echo);
	delete echo;
}



BOOST_AUTO_TEST_SUITE_END()






