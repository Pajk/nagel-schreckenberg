cmake_minimum_required(VERSION 2.6)

set(PROJECT_NAME nash)

project(${PROJECT_NAME})

set(SOURCES src/statistics.cc src/car.cc src/track.cc src/car_factory/simple_car_factory.cc src/car_factory/csv_car_factory.cc src/config.cc src/cell.h)
set(TESTS tests/config_test.cc tests/track_test.cc)

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})

add_executable(bin/run_tests tests/tests.cc ${SOURCES} ${TESTS})
target_link_libraries(bin/run_tests ${GTEST_LIBRARIES} pthread)

add_executable(bin/${PROJECT_NAME} src/main.cc ${SOURCES})
target_link_libraries(bin/${PROJECT_NAME})

add_executable(bin/search_params src/search_params.cc ${SOURCES})
target_link_libraries(bin/search_params ga)

# pokud je k dispozici knihovna allegro, prida se target gui-nash
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
find_package(allegro REQUIRED)

IF(ALLEGRO_FOUND)
    LINK_DIRECTORIES(${LINK_DIRECTORIES})
    INCLUDE_DIRECTORIES(${ALLEGRO_INCLUDE_DIRS} ${ALLEGRO_INCLUDE_DIR})

    add_executable(bin/gui-${PROJECT_NAME} src/main.cc ${SOURCES})
    target_link_libraries(bin/gui-${PROJECT_NAME} ${ALLEGRO_LIBRARIES})
    set_target_properties(bin/gui-${PROJECT_NAME} PROPERTIES COMPILE_DEFINITIONS "GUI=1")
ENDIF(ALLEGRO_FOUND)
