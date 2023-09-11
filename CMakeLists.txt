cmake_minimum_required(VERSION 3.26)
project(microjit_experiment)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -g")
set(CMAKE_C_FLAGS_RELEASE "${CMAKE_C_FLAGS_RELEASE} -g")

#set(CMAKE_BUILD_TYPE RelWithDebInfo)
#set(CMAKE_CXX_FLAGS_RELEASE "${CMAKE_CXX_FLAGS_RELEASE} -O0")


add_definitions(-DDEBUG_ENABLED)
add_definitions(-DASMJIT_EMBED)

set(MICROJIT_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/microjit)
set(GTEST_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/googletest)
set(GBM_SOURCE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/benchmark)

set(ASMJIT_SOURCE_DIR ${MICROJIT_SOURCE_DIR}/asmjit)

add_subdirectory(${GTEST_SOURCE_DIR})
add_subdirectory(${GBM_SOURCE_DIR})
add_subdirectory(${MICROJIT_SOURCE_DIR})
add_subdirectory(${ASMJIT_SOURCE_DIR})

include_directories(${GTEST_SOURCE_DIR}/googletest/include)
include_directories(${GTEST_SOURCE_DIR}/googlemock/include)
include_directories(${GBM_SOURCE_DIR}/include)
include_directories(${ASMJIT_SOURCE_DIR}/src)
include_directories(${MICROJIT_SOURCE_DIR}/src)

#STRING(REPLACE "-O2" "-Od" CMAKE_CXX_FLAGS_RELEASE ${CMAKE_CXX_FLAGS_RELEASE})

add_executable(gtest_run gtest_main.cpp
        tests/general_functionalities.cpp
        tests/conversion_test.gen.cpp
        tests/combined_test.cpp
        general_functionalities.h
        tests/native_invocation_test.cpp
)

add_executable(gbenchmark_run gbenchmark_main.cpp
        bm/combined_benchmark.cpp
        general_functionalities.h
)

add_executable(microjit_experiments experiments_main.cpp
        general_experiments.h
        general_functionalities.h
)

target_link_libraries(gtest_run asmjit microjit gtest gtest_main)
target_link_libraries(gbenchmark_run asmjit microjit benchmark::benchmark)
target_link_libraries(microjit_experiments asmjit microjit)
#target_link_libraries(microjit_experiment -L${CMAKE_CURRENT_SOURCE_DIR}/../cmake-build-debug -lmicrojit)
#-L${CMAKE_CURRENT_SOURCE_DIR}/../cmake-build-debug/asmjit -lasmjit