cmake_minimum_required(VERSION 2.6)

#
#  ######### SOURCES ##########
#

SET ( tests  testBufferLessScheduling.cpp )
INCLUDE_DIRECTORIES(${KITER_INCLUDE_DIR})
INCLUDE_DIRECTORIES(${Boost_INCLUDE_DIR})

#          ######### SCDUMP EXEC ##########

ADD_EXECUTABLE         (testBufferLessScheduling  testBufferLessScheduling.cpp)
TARGET_LINK_LIBRARIES  (testBufferLessScheduling libkiter -lz)


#
#  ######### INSTALL PART ##########
#

INSTALL(TARGETS testBufferLessScheduling DESTINATION bin)
    
