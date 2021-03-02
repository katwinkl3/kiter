cmake_minimum_required(VERSION 2.6)


#  ######### BOOST ##########

FIND_PACKAGE( Boost 1.69.0 COMPONENTS  graph  system  filesystem ) # remove REQUIRED to check by hand !


IF (EXISTS "/usr/include/boost141/boost/")
	SET(Boost_INCLUDE_DIR "/usr/include/boost141/")
ENDIF()

IF (NOT Boost_INCLUDE_DIR) 
	MESSAGE( FATAL_ERROR "Boost includes not found !!") 
ENDIF()
	


MESSAGE ( STATUS "Boost INCLUDES           = ${Boost_INCLUDE_DIR}" )
MESSAGE ( STATUS "Boost VERSION            = ${Boost_VERSION}" )




#  ######### LibXml2 PART ##########
FIND_PACKAGE (LibXml2 REQUIRED)



ADD_SUBDIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/libkiter libkiter)
ADD_SUBDIRECTORY(${CMAKE_CURRENT_SOURCE_DIR}/kiter kiter)

