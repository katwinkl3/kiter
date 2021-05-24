/*
 *
 *      Author: toky
 */

#define BOOST_TEST_MODULE StrictlyPeriodicTest
#include "helpers/test_classes.h"
#include <algorithms/throughput/csdf_strictly_periodic.h>
#include <algorithms/throughput/kperiodic.h>
#include <algorithms/schedulings.h>
#include <models/EventGraph.h>



BOOST_FIXTURE_TEST_SUITE( event_graph_test_suite , WITH_SAMPLE)

BOOST_AUTO_TEST_CASE( test_pipeline_generation )
{

	computeRepetitionVector(pipeline_sample);

	models::EventGraph* eg = algorithms::generate_csdf_strictly_periodic_event_graph(pipeline_sample);
	std::cout << eg->printXML() << std::endl;
	delete eg;

}

BOOST_AUTO_TEST_CASE( test_cycle_generation )
{

	computeRepetitionVector(cycle_sample);

	models::EventGraph* eg = algorithms::generate_csdf_strictly_periodic_event_graph(cycle_sample);
	std::cout << eg->printXML() << std::endl;
	delete eg;

}

BOOST_AUTO_TEST_CASE( test_MinCycleRatio )
{

	computeRepetitionVector(cycle_sample);

	models::EventGraph* eg = algorithms::generate_csdf_strictly_periodic_event_graph(cycle_sample);
	std::cout << eg->printXML() << std::endl;
	std::cout << "Looking for (Period:24) or (Thr:0.0416667)" << std::endl;
	BOOST_CHECK_CLOSE ( 1 / eg->MinCycleRatio().first , 24, 0.0001 );
	delete eg;

}


BOOST_AUTO_TEST_SUITE_END()




