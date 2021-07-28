/*
 *
 *  Created on: Jul 23, 2021
 *      Author: SamKouteili
 */

#define BOOST_TEST_MODULE TemplateTest
#include "helpers/test_classes.h"
#include <models/Dataflow.h>
#include <models/Scheduling.h>
#include "helpers/random_generator.h"
#include <algorithms/throughput/csdf_strictly_periodic.h>
#include <algorithms/schedulings.h>


#include <map> 

BOOST_FIXTURE_TEST_SUITE( valid_schedule_test , WITH_VERBOSE)

struct task_track {
	ARRAY_INDEX id;
	TIME_UNIT exec_time;
};

struct task_catalog {
	EXEC_COUNT cur_phase;
	EXEC_COUNT task_Ni;
	std::vector<TIME_UNIT> phase_durations;
	std::vector<TIME_UNIT> schedule;
};

BOOST_AUTO_TEST_CASE( small_graph_test_valid_schedule)
{
    models::Dataflow* gg = generate_random_graph(4, 5, 3, 3, 3);

    models::Scheduling sched = algorithms::scheduling::CSDF_SPeriodicScheduling(gg);
    sched.verbose_print();
    VERBOSE_ASSERT(sched.check_valid_schedule(), "Invalid Schedule");

    delete gg;

}


BOOST_AUTO_TEST_SUITE_END()




