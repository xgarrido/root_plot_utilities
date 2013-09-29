# - Top level CMakeLists.txt for plot_genbb_comparison program

cmake_minimum_required(VERSION 2.8 FATAL_ERROR)
project(plot_genbb_comparison)

if(${CMAKE_SOURCE_DIR} STREQUAL ${CMAKE_BINARY_DIR})
  message(STATUS "${PROJECT_NAME} requires an out-of-source build.")
  message(STATUS "Please remove these files from ${CMAKE_BINARY_DIR} first:")
  message(STATUS "  CMakeCache.txt")
  message(STATUS "  CMakeFiles")
  message(STATUS "Once these files are removed, create a separate directory")
  message(STATUS "and run CMake from there, pointing it to:")
  message(STATUS "  ${CMAKE_SOURCE_DIR}")
  message(FATAL_ERROR "in-source build detected")
endif()

# - Boost
set(Boost_COMPONENTS program_options)
find_package(Boost 1.47 QUIET COMPONENTS ${Boost_COMPONENTS} NO_MODULE)
find_package(Boost 1.47 REQUIRED ${Boost_COMPONENTS})

# - ROOT
find_package(ROOT 5.26.00 REQUIRED)

# - datatools
find_package(datatools 5.0.0 REQUIRED)

include_directories(${PROJECT_SOURCE_DIR} ${ROOT_INCLUDE_DIRS} ${datatools_INCLUDE_DIRS})

add_library(root_plot SHARED base_container.h base_container.cc)

install( FILES
         ${PROJECT_BINARY_DIR}/libroot_plot${CMAKE_SHARED_LIBRARY_SUFFIX}
         DESTINATION ${CMAKE_INSTALL_PREFIX}/lib
         PERMISSIONS OWNER_READ OWNER_EXECUTE
         GROUP_READ GROUP_EXECUTE
         WORLD_READ WORLD_EXECUTE
       )

add_executable(plot_genbb_comparison plot_genbb_comparison.cxx)

target_link_libraries(plot_genbb_comparison
  root_plot
  ${ROOT_LIBRARIES}
  ${Boost_LIBRARIES}
  ${datatools_LIBRARIES}
  )

install ( FILES
    ${PROJECT_BINARY_DIR}/plot_genbb_comparison
    DESTINATION ${CMAKE_INSTALL_PREFIX}
    PERMISSIONS OWNER_READ OWNER_EXECUTE
    GROUP_READ GROUP_EXECUTE
    WORLD_READ WORLD_EXECUTE
  )

# end of CMakeLists.txt