/*
 * NormalizationTest.cpp
 *
 *  Created on: Jun 21, 2021
 *      Author: SamKouteili
 */

#define BOOST_TEST_MODULE NormalizationTest
#include "helpers/test_classes.h"
#include <models/Dataflow.h>
#include "helpers/random_generator.h"
#include <algorithms/normalization.h>
#include <chrono>


#define MAX_ITER 30
#define MAX_PHASE_COUNT 5

BOOST_FIXTURE_TEST_SUITE( normalization_test_suite , WITH_VERBOSE)

long generate_normalization_vector (models::Dataflow* dataflow){
	auto start = std::chrono::high_resolution_clock::now();
	VERBOSE_ASSERT(algorithms::normalize(dataflow), "Could not compute Repeptition vector");
	auto elapse = std::chrono::high_resolution_clock::now() - start;
	delete dataflow;
	return elapse.count()/ 1000000;
}

BOOST_AUTO_TEST_CASE( generate_normalization_vector__performance_test )
{

	std::cout << "" << "actor number"
			<< ", " << "buffer number"
			<<", " <<  "repetition vector" << std::endl;

	for (int i = MAX_ITER/10; i <= MAX_ITER; i += MAX_ITER/10) {
		int buf_num = std::rand() % (i/2) + i; 
		models::Dataflow*  g = generate_random_graph(i, buf_num,  MAX_PHASE_COUNT, i, i);
		std::cout << "" << i
			<< ", " << buf_num
			<< ", " << generate_normalization_vector(g);
	}
}

BOOST_AUTO_TEST_SUITE_END()




