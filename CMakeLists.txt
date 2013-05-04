cmake_minimum_required(VERSION 2.6)

project(nash)

set(SOURCES src/statistics.cc src/car.cc src/track.cc src/car_factory/simple_car_factory.cc src/car_factory/csv_car_factory.cc src/config.cc src/cell.h)
set(TESTS tests/config_test.cc tests/track_test.cc)

add_executable(bin/nash src/main.cc ${SOURCES})
target_link_libraries(bin/nash)

set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")

find_package(GAlib)

IF(GALIB_FOUND)
  include_directories(${GALIB_INCLUDE_DIRS})
  add_executable(bin/search_params src/search_params.cc ${SOURCES})
  target_link_libraries(bin/search_params ${GALIB_LIBRARIES})
  set_target_properties(bin/search_params PROPERTIES COMPILE_DEFINITIONS "DEBUG=1")
ENDIF(GALIB_FOUND)

find_package(allegro)

IF(ALLEGRO_FOUND)
    link_directories(${LINK_DIRECTORIES})
    include_directories(${ALLEGRO_INCLUDE_DIRS})
    add_executable(bin/gui-nash src/main.cc ${SOURCES})
    target_link_libraries(bin/gui-nash ${ALLEGRO_LIBRARIES})
    set_target_properties(bin/gui-nash PROPERTIES COMPILE_DEFINITIONS "GUI=1")
ENDIF(ALLEGRO_FOUND)

find_package(GoogleTest)

IF(GTEST_FOUND AND GALIB_FOUND)
    include_directories(${GTEST_INCLUDE_DIRS} ${GALIB_INCLUDE_DIRS})
    add_executable(bin/run_tests tests/tests.cc ${SOURCES} ${TESTS})
    target_link_libraries(bin/run_tests ${GTEST_LIBRARIES} pthread ${GALIB_LIBRARIES})
ENDIF(GTEST_FOUND AND GALIB_FOUND)

add_executable(bin/analyzer src/analyzer.cc)
