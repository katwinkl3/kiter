/*
 *
 *  Created on: Nov 6, 2019
 *      Author: toky
 */

#include <printers/stdout.h>
#include <algorithms/mappings.h>
#include <algorithms/repetition_vector.h>

#define BOOST_TEST_MODULE TemplateTest
#include "helpers/test_classes.h"


BOOST_FIXTURE_TEST_SUITE( mapping_test_suite, WITH_SAMPLE)

BOOST_AUTO_TEST_CASE( RandomMapping )
{
	algorithms::mapping::randomMapping(sample, parameters_list_t());
}

BOOST_AUTO_TEST_CASE( BufferlessNoCMapAndRouteMapping )
{
	algorithms::mapping::BufferlessNoCMapAndRoute(sample, parameters_list_t());

}

BOOST_AUTO_TEST_SUITE_END()




