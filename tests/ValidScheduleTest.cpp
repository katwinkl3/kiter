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

BOOST_FIXTURE_TEST_SUITE( valid_schedule_test , WITH_VERBOSE)


BOOST_AUTO_TEST_CASE( test_name1 )
{
    models::Dataflow* g = generate_random_graph(5, 6, 3, 5, 3);

    // models::Scheduling sched = compute_CSDF_SPeriodic(g);
    // sched.check_valid_schedule();

    delete g;
}


BOOST_AUTO_TEST_SUITE_END()




