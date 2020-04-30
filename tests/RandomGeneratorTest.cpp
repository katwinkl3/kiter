/*
 * RandomgeneratorTest.cpp
 *
 *  Created on: Apr 30, 2020
 *      Author: toky
 */


#define BOOST_TEST_MODULE TemplateTest
#include "helpers/test_classes.h"
#include <models/Dataflow.h>
#include <algorithms/repetition_vector.h>
#include <algorithms/symbolic_execution.h>
#include <generators/RandomGenerator.h>

static void verify_dataflow (models::Dataflow * dataflow, RandomGeneratorConfiguration config) {

	BOOST_REQUIRE_LE(dataflow->getVerticesCount() , config.max_vertices_count );
	BOOST_REQUIRE_LE(dataflow->getEdgesCount() , config.max_edges_count );

	BOOST_REQUIRE(computeRepetitionVector(dataflow));

}

BOOST_FIXTURE_TEST_SUITE( generator_test , WITH_VERBOSE)

BOOST_AUTO_TEST_CASE( generator_test_two_tasks ) {

	RandomGeneratorConfiguration config;
	config.max_vertices_count      = 2;
	config.max_edges_count         = 1;
	config.max_weight              = 1;
	config.max_duration            = 1;
	config.max_phase_quantity      = 1;

	RandomGenerator generator (config);
	models::Dataflow * dataflow = generator.generate();
	verify_dataflow(dataflow, config);


	delete dataflow;
}

BOOST_AUTO_TEST_CASE( generator_test_ten_tasks_HSDF ) {

	RandomGeneratorConfiguration config;
	config.max_vertices_count       = 10;
	config.max_edges_count          = 15;
	config.max_weight               = 1;
	config.max_duration             = 1;
	config.max_phase_quantity       = 1;
	RandomGenerator generator (config);
	models::Dataflow * dataflow = generator.generate();

	verify_dataflow(dataflow, config);

	delete dataflow;
}


BOOST_AUTO_TEST_CASE( generator_test_ten_tasks_SDF ) {

	RandomGeneratorConfiguration config;
	config.max_vertices_count       = 10;
	config.max_edges_count          = 15;
	config.max_weight               = 10;
	config.max_duration             = 10;
	config.max_phase_quantity       = 1;
	RandomGenerator generator (config);
	models::Dataflow * dataflow = generator.generate();

	verify_dataflow(dataflow, config);


	delete dataflow;
}

BOOST_AUTO_TEST_SUITE_END()








