/*
 * VerboseTest.cpp
 *
 *  Created on: Jul 3, 2021
 *      Author: SamKouteili
 */

#define BOOST_TEST_MODULE VerboseTest
#include "helpers/test_classes.h"
#include <models/Dataflow.h>
#include <commons/Verbose.h>


BOOST_FIXTURE_TEST_SUITE( verbose_test_suite , WITH_VERBOSE)

BOOST_AUTO_TEST_CASE(calling_verbose_functions )
{
    Verbose::Info() << "Verbose::Info() test, output " << 1 << " & output" << 2 << "functional";
    Verbose::Debug() << "Verbose::Debug() test, output " << 1 << " & output" << 2 << "functional";
    Verbose::Assert(true) << "This should not output!";
    // try
    // {
    //     Verbose::Assert(false) << "Verbose::Assert() test,";
    // }
    // catch(exit())
    // {
    //     std::cerr << "Verbose::Assert() Successfuly failing"<< '\n';
    // }
    Verbose::AssertGreater(2, 1) << "This should not output!";
}

BOOST_AUTO_TEST_SUITE_END()




