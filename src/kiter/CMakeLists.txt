cmake_minimum_required(VERSION 2.6)

#
#  ######### SOURCES ##########
#

SET ( kiter_sources  main.cpp )
INCLUDE_DIRECTORIES(${KITER_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})

#          ######### SCDUMP EXEC ##########

ADD_EXECUTABLE         (kiter  ${kiter_sources})
TARGET_LINK_LIBRARIES  (kiter libkiter -lz)


#
#  ######### INSTALL PART ##########
#

INSTALL(TARGETS kiter DESTINATION bin)
    
