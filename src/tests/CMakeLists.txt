cmake_minimum_required(VERSION 2.6)

add_executable(DataflowTest DataflowTest.cpp)

INCLUDE_DIRECTORIES(${KITER_INCLUDE_DIR})
target_link_libraries(DataflowTest
					  libkiter
                      ${Boost_UNIT_TEST_FRAMEWORK_LIBRARY}
                      )
    
add_test(NAME DataflowTest COMMAND DataflowTest)
