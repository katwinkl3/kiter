/*
 *
 *  Created on: Nov 6, 2019
 *      Author: toky
 */

#define BOOST_TEST_MODULE TemplateTest
#include "helpers/test_classes.h"
#include "helpers/sample.h"

BOOST_FIXTURE_TEST_SUITE( repetition_vector_test , WITH_SAMPLE)

BOOST_AUTO_TEST_CASE( sample_test )
{

	BOOST_REQUIRE(pipeline_sample);
	Vertex a = pipeline_sample->getVertexByName("a");
	Vertex b = pipeline_sample->getVertexByName("b");
	Vertex c = pipeline_sample->getVertexByName("c");
	BOOST_REQUIRE(computeRepetitionVector(pipeline_sample));
	BOOST_REQUIRE_EQUAL(pipeline_sample->getNi(a), 3);
	BOOST_REQUIRE_EQUAL(pipeline_sample->getNi(b), 2);
	BOOST_REQUIRE_EQUAL(pipeline_sample->getNi(c), 1);

}

BOOST_AUTO_TEST_SUITE_END()




