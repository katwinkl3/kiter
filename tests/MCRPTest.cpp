/*
 * MCRPTest.cpp
 *
 *  Created on: Jun 24, 2021
 *      Author: SamKouteili
 */

#define BOOST_TEST_MODULE MCRPTest
#include "helpers/test_classes.h"
#include <models/Dataflow.h>
#include "helpers/random_generator.h"
#include <algorithms/throughput/kperiodic.h>
#include <algorithms/nperiodic.h>
#include <models/EventGraph.h>
#include <chrono>


#define MAX_ITER 30
#define MAX_PHASE_COUNT 5

/* how to define bound? */

BOOST_FIXTURE_TEST_SUITE( MCRP_test_suite , WITHOUT_VERBOSE)

long event_graph_performance_tester (models::Dataflow* dataflow){


    VERBOSE_ASSERT(dataflow,TXT_NEVER_HAPPEND);
	VERBOSE_ASSERT(computeRepetitionVector(dataflow),"inconsistent graph");
	auto kvector = algorithms::get_Kvector(dataflow);

	auto start = std::chrono::high_resolution_clock::now();
	models::EventGraph* eg = algorithms::generateKPeriodicEventGraph(dataflow,&kvector);
    auto elapse = std::chrono::high_resolution_clock::now() - start;

    VERBOSE_INFO("KPeriodic EventGraph generation Done, edges = " << eg->getConstraintsCount() << " vertex = " << eg->getEventCount());

	delete dataflow;
	return elapse.count()/ 1000000;
}

long MCRP_performance_tester (models::Dataflow* dataflow){
    VERBOSE_ASSERT(dataflow,TXT_NEVER_HAPPEND);
	VERBOSE_ASSERT(computeRepetitionVector(dataflow),"inconsistent graph");
    auto kvector = algorithms::get_Kvector(dataflow);

    models::EventGraph* eg = algorithms::generateKPeriodicEventGraph(dataflow,&kvector);
	VERBOSE_INFO("KPeriodic EventGraph generation Done, edges = " << eg->getConstraintsCount() << " vertex = " << eg->getEventCount());

    auto start = std::chrono::high_resolution_clock::now();
    std::pair<TIME_UNIT,std::vector<models::EventGraphEdge> > howard_res = eg->MinCycleRatio();
	std::vector<models::EventGraphEdge> * critical_circuit = &(howard_res.second);
    auto elapse = std::chrono::high_resolution_clock::now() - start;

    delete dataflow;
	return elapse.count()/ 1000000;
}


BOOST_AUTO_TEST_CASE( generate_event_graph__performance_test )
{

	commons::set_verbose_mode(commons::WARNING_LEVEL);
	for (int i = MAX_ITER/10; i <= MAX_ITER; i += MAX_ITER/10) {
		int buf_num = std::rand() % (i/2) + i;
		std::cout << "Iteration " << i << " buf_num=" << buf_num << std::endl;
		models::Dataflow*  g = generate_random_graph(i, buf_num,  MAX_PHASE_COUNT, i, i);
		std::cout << "Graph done " << i << std::endl;
		std::cout << event_graph_performance_tester(g) << std::endl;
		std::cout << "event_graph_performance_tester done " << i << std::endl;
	};
}

BOOST_AUTO_TEST_CASE( MCRP_performance_test )
{
// 	for (int i = MAX_ITER/10; i <= MAX_ITER; i += MAX_ITER/10) {
// 		int buf_num = std::rand() % (i/2) + i; 
// 		models::Dataflow*  g = generate_random_graph(i, buf_num,  MAX_PHASE_COUNT, i, i);
// 		std::cout << MCRP_performance_tester(g);
// 	};
}

BOOST_AUTO_TEST_SUITE_END()




