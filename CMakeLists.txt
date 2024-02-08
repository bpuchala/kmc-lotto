cmake_minimum_required(VERSION 3.20)
project(kmc_lotto VERSION 1.0.0 LANGUAGES CXX)

# set CMAKE_INSTALL_X variables
include(GNUInstallDirs)

# specify the C++ standard
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED True)


##############################################
## Create kmc-lotto header-only library

set(
  kmc_lotto_HEADERS
  ${PROJECT_SOURCE_DIR}/include/lotto/event_rate_tree.hpp
  ${PROJECT_SOURCE_DIR}/include/lotto/event_rate_tree_impl.hpp
  ${PROJECT_SOURCE_DIR}/include/lotto/event_selector.hpp
  ${PROJECT_SOURCE_DIR}/include/lotto/random.hpp
  ${PROJECT_SOURCE_DIR}/include/lotto/rejection.hpp
  ${PROJECT_SOURCE_DIR}/include/lotto/rejection_free.hpp
  ${PROJECT_SOURCE_DIR}/include/lotto/sum_tree.hpp
  ${PROJECT_SOURCE_DIR}/include/lotto/sum_tree_impl.hpp
)
add_library(kmc_lotto INTERFACE)
target_include_directories(kmc_lotto
  INTERFACE
    $<INSTALL_INTERFACE:${CMAKE_INSTALL_INCLUDEDIR}>
    $<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/include>
)
target_compile_features(kmc_lotto INTERFACE cxx_std_17)


##############################################
## Install kmc-lotto

# install header files in <prefix>/include/lotto/,
# while preserving directory structure
foreach ( filevar ${kmc_lotto_HEADERS} )
  file(RELATIVE_PATH relfile ${PROJECT_SOURCE_DIR}/include/ ${filevar})
  get_filename_component( reldir ${relfile} DIRECTORY )
  install(
    FILES ${filevar}
    DESTINATION ${CMAKE_INSTALL_INCLUDEDIR}/${reldir}
  )
endforeach()

# install artifacts
install(
  TARGETS kmc_lotto
  EXPORT kmc_lottoTargets
  DESTINATION lib)

# Install the CMake package config and target files
set(LOTTO_CMAKE_PACKAGE_INSTALL_SUBDIR "share/kmc_lotto/cmake")

install(
  EXPORT kmc_lottoTargets
  NAMESPACE LOTTO::
  DESTINATION ${LOTTO_CMAKE_PACKAGE_INSTALL_SUBDIR})

include(CMakePackageConfigHelpers)

write_basic_package_version_file(
  kmc_lottoConfigVersion.cmake
  VERSION ${PROJECT_VERSION}
  COMPATIBILITY SameMinorVersion)

configure_package_config_file(
  "${PROJECT_SOURCE_DIR}/cmake/kmc_lottoConfig.cmake.in" kmc_lottoConfig.cmake
  INSTALL_DESTINATION ${LOTTO_CMAKE_PACKAGE_INSTALL_SUBDIR})

install(FILES "${PROJECT_BINARY_DIR}/kmc_lottoConfig.cmake"
              "${PROJECT_BINARY_DIR}/kmc_lottoConfigVersion.cmake"
        DESTINATION ${LOTTO_CMAKE_PACKAGE_INSTALL_SUBDIR})

