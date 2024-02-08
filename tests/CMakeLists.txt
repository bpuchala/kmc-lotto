cmake_minimum_required(VERSION 3.20)
set(CMAKE_OSX_DEPLOYMENT_TARGET "10.15" CACHE STRING "Minimum OS X deployment version")

project(kmc_lotto_tests VERSION 1.0.0 LANGUAGES CXX)


# set CMAKE_INSTALL_X variables
include(GNUInstallDirs)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)

# try to use ccache
find_program(CCACHE_PROGRAM ccache)
if(CCACHE_PROGRAM)
    set(CMAKE_CXX_COMPILER_LAUNCHER "${CCACHE_PROGRAM}")
endif()

### googletest ###
include(FetchContent)

FetchContent_Declare(
  googletest
  GIT_REPOSITORY https://github.com/google/googletest.git
  GIT_TAG        v1.13.0
)
FetchContent_MakeAvailable(googletest)
message(STATUS "googletest_SOURCE_DIR: ${googletest_SOURCE_DIR}")

# compile Google Test as an object library
add_library(gtest_all OBJECT "${googletest_SOURCE_DIR}/googletest/src/gtest-all.cc")
set_property(TARGET gtest_all PROPERTY INCLUDE_DIRECTORIES
  "${googletest_SOURCE_DIR}/googletest/include"
  "${googletest_SOURCE_DIR}/googletest")
set_property(TARGET gtest_all PROPERTY POSITION_INDEPENDENT_CODE ON)
target_compile_options(gtest_all
  PUBLIC
    "-DGTEST_HAS_PTHREAD=0"
    "-DGTEST_LINKED_AS_SHARED_LIBRARY=1"
)

### kmc-lotto ###
find_package(kmc_lotto)
if(NOT kmc_lotto_FOUND)
  message(FATAL_ERROR "CMake failed to find kmc-lotto")
endif()
# if successful, we have LOTTO::kmc_lotto

enable_testing()


################################################################
# kmc_lotto_unit_tests
add_executable(kmc_lotto_unit_tests
  ${PROJECT_SOURCE_DIR}/unit/gtest_main_run_all.cpp
  ${PROJECT_SOURCE_DIR}/unit/lotto/event_rate_tree.cpp
  ${PROJECT_SOURCE_DIR}/unit/lotto/random.cpp
  ${PROJECT_SOURCE_DIR}/unit/lotto/rejection.cpp
  ${PROJECT_SOURCE_DIR}/unit/lotto/rejection_free.cpp
)
target_link_libraries(kmc_lotto_unit_tests
  gtest_all
  LOTTO::kmc_lotto
)
target_include_directories(kmc_lotto_unit_tests
  PUBLIC
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/unit>
    $<BUILD_INTERFACE:${googletest_SOURCE_DIR}/googletest/include>
)

add_test(NAME kmc_lotto_unit_tests COMMAND kmc_lotto_unit_tests)

