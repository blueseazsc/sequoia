cmake_minimum_required( VERSION 3.25.3 )

project(sequoia
        VERSION  1.0.0
        DESCRIPTION "Sequoia project for nrsdk!"
        LANGUAGES CXX C
        )

# Define relative dir for ${CMAKE_SOURCE_DIR}
set(PROJECT_SRC_DIR "sequoia")

######################################################################################
##                              INIT						    ##
######################################################################################
find_package(spdlog REQUIRED)
find_package(doctest REQUIRED)
######################################################################################
##                              Compiler					    ##
######################################################################################
if (CMAKE_CXX_COMPILER_ID MATCHES "Clang")
    set(PROJECT_CMAKE_C_FLAGS "${PROJECT_CMAKE_C_FLAGS} -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-shorten-64-to-32")
    set(PROJECT_CMAKE_CXX_FLAGS "${PROJECT_CMAKE_CXX_FLAGS} -Wno-deprecated -Wno-register -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-unused-private-field -Wno-shorten-64-to-32 -Wno-vla-extension")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "GNU")
    set(PROJECT_CMAKE_C_FLAGS "${PROJECT_CMAKE_C_FLAGS} -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-deprecated-declarations")
    set(PROJECT_CMAKE_CXX_FLAGS "${PROJECT_CMAKE_CXX_FLAGS} -Wno-register -Wno-unused-variable -Wno-unused-parameter -Wno-unused-function -Wno-deprecated-declarations")
elseif (CMAKE_CXX_COMPILER_ID STREQUAL "MSVC")
    set(PROJECT_CMAKE_CXX_FLAGS "${PROJECT_CMAKE_CXX_FLAGS} /WX")
endif()
######################################################################################
##                              Config Project                                      ##
######################################################################################
set(EXTERNAL_PROJECT_DIR ${PROJECT_SOURCE_DIR}/external/project)
list( APPEND CMAKE_MODULE_PATH "${EXTERNAL_PROJECT_DIR}/cmake" )

# After set environment variable, we includ project cmake.
include(Project)

# Set git hook
execute_process(COMMAND git config core.hooksPath external/project/githook)
