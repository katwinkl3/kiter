/*
 * test_helper.h
 *
 *  Created on: Nov 19, 2019
 *      Author: toky
 */

#ifndef TESTS_TEST_HELPER_H_
#define TESTS_TEST_HELPER_H_

#include <boost/test/included/unit_test.hpp>
#include <commons/verbose.h>


struct WITH_VERBOSE {
	WITH_VERBOSE() {
    	commons::set_verbose_mode(commons::INFO_LEVEL);
    	BOOST_TEST_MESSAGE( "Setup Done" );
    }
    ~WITH_VERBOSE()                    {
    	BOOST_TEST_MESSAGE( "Teardown Done" );
    }
};


#endif /* TESTS_TEST_HELPER_H_ */
