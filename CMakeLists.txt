cmake_minimum_required(VERSION 3.26)
project(microjit_experiment)

set(CMAKE_CXX_STANDARD 17)

add_definitions(-DDEBUG_ENABLED)
add_definitions(-DASMJIT_EMBED)

set(MICROJIT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/microjit)
set(GTEST_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/googletest)

set(ASMJIT_SOURCE_DIR ${MICROJIT_SOURCE_DIR}/asmjit)

add_subdirectory(${GTEST_SOURCE_DIR})
add_subdirectory(${MICROJIT_SOURCE_DIR})
add_subdirectory(${ASMJIT_SOURCE_DIR})

include_directories(${GTEST_SOURCE_DIR}/googletest/include)
include_directories(${GTEST_SOURCE_DIR}/googlemock/include)
include_directories(${ASMJIT_SOURCE_DIR}/src)
include_directories(${MICROJIT_SOURCE_DIR}/src)

#STRING(REPLACE "-O2" "-Od" CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE})

add_executable(microjit_experiment main.cpp
        general_experiments.h
        tests/general_functionalities.cpp
        tests/test_thread_pool.cpp
        tests/conversion_test.gen.cpp
        tests/combined_test.cpp
        tests/combined_test_builder.h
)
target_link_libraries(microjit_experiment asmjit microjit gtest gtest_main)
#target_link_libraries(microjit_experiment -L${CMAKE_CURRENT_SOURCE_DIR}/../cmake-build-debug -lmicrojit)
#-L${CMAKE_CURRENT_SOURCE_DIR}/../cmake-build-debug/asmjit -lasmjit