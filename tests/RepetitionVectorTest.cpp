/*
 *
 *  Created on: Nov 6, 2019
 *      Author: toky
 */

#define BOOST_TEST_MODULE TemplateTest
#include "helpers/test_classes.h"
#include "helpers/sample.h"
#include <algorithms/repetition_vector.h>

BOOST_FIXTURE_TEST_SUITE( repetition_vector_test , WITH_SAMPLE)

BOOST_AUTO_TEST_CASE( sample_test )
{

	BOOST_REQUIRE(sample);
	Vertex a = sample->getVertexByName("a");
	Vertex b = sample->getVertexByName("b");
	Vertex c = sample->getVertexByName("c");
	BOOST_REQUIRE(computeRepetitionVector(sample));
	BOOST_REQUIRE_EQUAL(sample->getNi(a), 3);
	BOOST_REQUIRE_EQUAL(sample->getNi(b), 2);
	BOOST_REQUIRE_EQUAL(sample->getNi(c), 1);

}

BOOST_AUTO_TEST_SUITE_END()




