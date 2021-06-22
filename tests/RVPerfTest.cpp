/*
 *
 *  Created on: Nov 6, 2019
 *      Author: toky
 */

#define BOOST_TEST_MODULE RVPerfTest
#include "helpers/test_classes.h"
#include <models/Dataflow.h>
#include <generators/RandomGenerator.h>
#include <algorithms/normalization.h>
#include <chrono>


#define MAX_ITER 4000
#define MAX_PHASE_COUNT 10

BOOST_FIXTURE_TEST_SUITE( rv_perf_test_suite , WITH_VERBOSE)

models::Dataflow* generate_random_graph (int num_actor,
					 int num_buffers,
					 int max_phases,
					 int max_weight,
					 int max_duration) {
	
	RandomGeneratorConfiguration config;
	config.min_vertices_count      = num_actor;
	config.max_vertices_count      = num_actor;
	config.min_edges_count         = num_buffers;
	config.max_edges_count         = num_buffers;
	config.max_weight              = max_weight;
	config.max_duration            = max_duration;
	config.max_phase_quantity      = max_phases;

	RandomGenerator generator (config);
	models::Dataflow * dataflow = generator.generate();

	return dataflow;
}

long generate_repetition_vector (models::Dataflow* dataflow){
	auto start = std::chrono::high_resolution_clock::now();
	VERBOSE_ASSERT(computeRepetitionVector(dataflow), "Could not compute Repeptition vector");
	auto elapse = std::chrono::high_resolution_clock::now() - start;

	delete dataflow;
	return elapse.count()/ 1000000;
}

long generate_normalization_vector (models::Dataflow* dataflow){
	auto start = std::chrono::high_resolution_clock::now();
	VERBOSE_ASSERT(algorithms::normalize(dataflow), "Could not compute Repeptition vector");
	auto elapse = std::chrono::high_resolution_clock::now() - start;
	delete dataflow;
	return elapse.count()/ 1000000;
}



BOOST_AUTO_TEST_CASE( generate_repetition_vector_test )
{

	std::cout << "" << "actor number"
			<< ", " << "buffer number"
			<<", " <<  "repetition vector" << std::endl;

	for (int i = MAX_ITER/10; i <= MAX_ITER; i += MAX_ITER/10) {
		int buf_num = std::rand() % (i/2) + i; 
		models::Dataflow*  g = generate_random_graph(i, buf_num, MAX_PHASE_COUNT, i, i);
		std::cout << "" << i
			<< ", " << buf_num
			<< ", " << generate_repetition_vector(g);
	}
}

BOOST_AUTO_TEST_CASE( generate_normalization_vector_test )
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




