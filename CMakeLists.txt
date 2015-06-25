# - Top level CMakeLists.txt for root_plot_utilities program

cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(root_plot_utilities)

# Use C++11
set(CMAKE_CXX_FLAGS "-W -Wall -std=c++11")

# - ROOT
find_package(ROOT 5.26.00 REQUIRED)

# - Bayeux
find_package(Bayeux 1.0.0 REQUIRED)

include_directories(${PROJECT_SOURCE_DIR} ${ROOT_INCLUDE_DIRS} ${Bayeux_INCLUDE_DIRS})

add_library(root_plot_utilities SHARED
  manager.h
  manager.cc
  base_container.h
  base_container.cc)

install(FILES
  ${PROJECT_BINARY_DIR}/libroot_plot_utilities${CMAKE_SHARED_LIBRARY_SUFFIX}
  DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
  PERMISSIONS OWNER_READ OWNER_EXECUTE
  GROUP_READ GROUP_EXECUTE
  WORLD_READ WORLD_EXECUTE
  )

add_executable(plot_histogram plot_histogram.cxx)

target_link_libraries(plot_histogram
  root_plot_utilities
  ${ROOT_LIBRARIES}
  ${Bayeux_LIBRARIES}
  )

install(FILES
  ${PROJECT_BINARY_DIR}/plot_histogram
  DESTINATION ${CMAKE_INSTALL_PREFIX}
  PERMISSIONS OWNER_READ OWNER_EXECUTE
  GROUP_READ GROUP_EXECUTE
  WORLD_READ WORLD_EXECUTE
  )

# end of CMakeLists.txt