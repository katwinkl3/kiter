/*
 *
 *  Created on: 2021
 *      Author: toky
 */

#include <commons/verbose.h>
#include <commons/commons.h>

#define BOOST_TEST_MODULE CommonTest
#include "helpers/test_classes.h"



BOOST_FIXTURE_TEST_SUITE( stdout_printer_test_suite, WITH_VERBOSE)


BOOST_AUTO_TEST_CASE( euclide_algorithm )
{
	int a =  630;
	int b = 1088;
	int c =   20;

	auto res = commons::extended_euclide (a, b, c) ;

	VERBOSE_ASSERT_EQUALS(res.first , 190);
	VERBOSE_ASSERT_EQUALS(res.second , -110);


	auto res2 = commons::extended_euclide(8, 19, 1) ;


	VERBOSE_ASSERT_EQUALS( res2.first  , -7);
	VERBOSE_ASSERT_EQUALS( res2.second ,  3);

}


BOOST_AUTO_TEST_SUITE_END()




