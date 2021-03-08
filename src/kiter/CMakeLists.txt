cmake_minimum_required(VERSION 2.6)

FIND_PACKAGE(GUROBI)

#
#  ######### SOURCES ##########
#

SET ( kiter_sources  main.cpp )
INCLUDE_DIRECTORIES(${KITER_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})

#  ######### EXEC ##########

ADD_EXECUTABLE         (kiter  ${kiter_sources})
TARGET_LINK_LIBRARIES  (kiter  libkiter -lz)
INSTALL(TARGETS kiter DESTINATION bin)

if(GUROBI_FOUND)
 ADD_EXECUTABLE         (kiter_gurobi  ${kiter_sources})
 TARGET_LINK_LIBRARIES  (kiter_gurobi  libkiter_gurobi -lz)
endif(GUROBI_FOUND)

