cmake_minimum_required(VERSION 2.6)

project(nasch)

set(CMAKE_CXX_FLAGS "-g -Wall")

set(SOURCES src/statistics.cc src/car.cc src/track.cc src/car_factory/simple_car_factory.cc src/car_factory/csv_car_factory.cc src/config.cc src/cell.h)
set(TESTS tests/config_test.cc tests/track_test.cc)

add_executable(bin/nasch src/main.cc ${SOURCES})
target_link_libraries(bin/nasch)

add_executable(bin/ascii-nasch src/main.cc ${SOURCES})
set_target_properties(bin/ascii-nasch PROPERTIES COMPILE_DEFINITIONS "ASCII=1")

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
    add_executable(bin/gui-nasch src/main.cc ${SOURCES})
    target_link_libraries(bin/gui-nasch ${ALLEGRO_LIBRARIES})
    set_target_properties(bin/gui-nasch PROPERTIES COMPILE_DEFINITIONS "GUI=1")
ENDIF(ALLEGRO_FOUND)

find_package(GoogleTest)

IF(GTEST_FOUND AND GALIB_FOUND)
    include_directories(${GTEST_INCLUDE_DIRS} ${GALIB_INCLUDE_DIRS})
    add_executable(bin/run_tests tests/tests.cc ${SOURCES} ${TESTS})
    target_link_libraries(bin/run_tests ${GTEST_LIBRARIES} pthread ${GALIB_LIBRARIES})
ENDIF(GTEST_FOUND AND GALIB_FOUND)

add_executable(bin/tools src/tools.cc ${SOURCES})
