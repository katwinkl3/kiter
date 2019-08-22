cmake_minimum_required(VERSION 2.6)
 
FIND_PACKAGE( GLPK   REQUIRED )
FIND_PACKAGE( GUROBI )

#
#  ######### COMPILE FLAGS ##########
#
# -Wno-float-equal -Wno-switch-default ==> temp fix for Coin headers
SET(CMAKE_CXX_FLAGS_DEBUG "${CMAKE_CXX_FLAGS_DEBUG} -fPIC -Wconversion -Wno-float-equal -Wno-switch-default")
SET(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -fPIC -Wconversion -Wno-float-equal -Wno-switch-default")

#
#  ######### INCLUDE PART ##########
#
INCLUDE_DIRECTORIES(${KITER_INCLUDE_DIR})
INCLUDE_DIRECTORIES (${LIBXML2_INCLUDE_DIR})

#
#  ######### GENERAL SOURCES ##########
#

MESSAGE(STATUS "Generate libkiter source files")

FILE(GLOB_RECURSE commons_cpp commons/*.cpp)
FILE(GLOB_RECURSE commons_h   commons/*.h )

FILE(GLOB_RECURSE models_cpp models/*.cpp)
FILE(GLOB_RECURSE models_h   models/*.h )

FILE(GLOB_RECURSE printers_cpp printers/*.cpp)
FILE(GLOB_RECURSE printers_h   printers/*.h )

FILE(GLOB_RECURSE algorithms_cpp algorithms/*.cpp algorithms/*/*.cpp)
FILE(GLOB_RECURSE algorithms_h   algorithms/*.h )

SET(commons_sources      ${commons_cpp}   ${commons_h})
SET(models_sources      ${models_cpp}   ${models_h})
SET(printers_sources      ${printers_cpp}   ${printers_h})
SET(algorithms_sources      ${algorithms_cpp}   ${algorithms_h})


#
#  ######### TARGETS PART ##########
#


#          ######### GENERAL LIBS ##########

SET(LIBKITER_FLAGS "-Wconversion -Wfloat-equal  ${CMAKE_CXX_FLAGS_DEBUG} ")

### TODO Warning: Only build the static archive with -fPIC on AMD64. On other architectures this is unneeded and will have a performance impact at execution time. 
# Link about this : http://www.gentoo.org/proj/en/base/amd64/howtos/index.xml?part=1&chap=3

#          ######### API         ##########
ADD_LIBRARY			   (libkiter STATIC  ${commons_sources} ${models_sources} ${printers_sources} ${algorithms_sources})
TARGET_LINK_LIBRARIES  (libkiter rt  ${LIBXML2_LIBRARIES} ${Boost_FILESYSTEM_LIBRARY} ${Boost_SYSTEM_LIBRARY} ${GLPK_LIBRARIES}  )
set_property(TARGET libkiter PROPERTY FRAMEWORK ON)

if(GUROBI_FOUND)
    message(STATUS "GUROBI FOUND")
    target_include_directories(libkiter PRIVATE ${GUROBI_INCLUDE_DIRS} )
    TARGET_LINK_LIBRARIES( libkiter ${GUROBI_LIBRARIES} )
    target_compile_definitions(libkiter PRIVATE __GUROBILIB__=1)
endif(GUROBI_FOUND)

 


INSTALL(TARGETS libkiter
    RUNTIME DESTINATION bin
    LIBRARY DESTINATION lib
    ARCHIVE DESTINATION lib
    FRAMEWORK DESTINATION "Frameworks"
    PUBLIC_HEADER DESTINATION include/
    )
