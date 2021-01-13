/*
 *
 *  Created on: Nov 6, 2019
 *      Author: toky
 */

#include <commons/commons.h>

#define BOOST_TEST_MODULE TemplateTest
#include "helpers/test_classes.h"


BOOST_FIXTURE_TEST_SUITE( stdout_printer_test_suite, WITH_VERBOSE)

BOOST_AUTO_TEST_CASE( init_fract )
{
	TOKEN_FRACT zero(0);
	TOKEN_FRACT one(1);
	TOKEN_FRACT two(2);
}

BOOST_AUTO_TEST_CASE( op_fract )
{
	TOKEN_FRACT zero(0);
	TOKEN_FRACT one(1);
	TOKEN_FRACT two(2);
	TOKEN_FRACT four(4);
	TOKEN_FRACT half(1,2);
	BOOST_REQUIRE_EQUAL(one * one, one);
	BOOST_REQUIRE_EQUAL(two * two, four);
	BOOST_REQUIRE_EQUAL(two / four, half);
	BOOST_REQUIRE_EQUAL(two * half, one);

	BOOST_REQUIRE_GT(one , zero);
	BOOST_REQUIRE_GT(one , half);
	BOOST_REQUIRE_GT(half , zero);
	BOOST_REQUIRE_GT(two , one);
	BOOST_REQUIRE_GT(four , two);

	BOOST_REQUIRE_LT(zero , one);
	BOOST_REQUIRE_LT(half , one);
	BOOST_REQUIRE_LT(zero , half);
	BOOST_REQUIRE_LT(one  , two);
	BOOST_REQUIRE_LT(two  , four);

	BOOST_REQUIRE_NE(zero , one);
	BOOST_REQUIRE_NE(half , one);
	BOOST_REQUIRE_NE(zero , half);
	BOOST_REQUIRE_NE(one  , two);
	BOOST_REQUIRE_NE(two  , four);



}

BOOST_AUTO_TEST_SUITE_END()




