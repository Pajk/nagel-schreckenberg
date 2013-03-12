cmake_minimum_required(VERSION 2.6)

set(PROJECT_NAME nash)

project(${PROJECT_NAME})

set(SOURCES car.cc track.cc simple_car_factory.cc csv_car_factory.cc config.cc)
set(TESTS tests/config.cc)

find_package(GTest REQUIRED)
include_directories(${GTEST_INCLUDE_DIRS})
 
add_executable(run_tests tests.cc ${SOURCES} ${TESTS})
target_link_libraries(run_tests ${GTEST_LIBRARIES} pthread)

add_executable(${PROJECT_NAME} main.cc ${SOURCES})
target_link_libraries(${PROJECT_NAME} ga)

# pokud je k dispozici knihovna allegro, prida se target gui-nash
set(CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_SOURCE_DIR}/cmake/")
find_package(allegro REQUIRED)

IF(ALLEGRO_FOUND)
    LINK_DIRECTORIES(${LINK_DIRECTORIES})
    INCLUDE_DIRECTORIES(${ALLEGRO_INCLUDE_DIRS} ${ALLEGRO_INCLUDE_DIR})

    add_executable(gui-${PROJECT_NAME} main.cc ${SOURCES})
    target_link_libraries(gui-${PROJECT_NAME} ${ALLEGRO_LIBRARIES})
    set_target_properties(gui-${PROJECT_NAME} PROPERTIES COMPILE_DEFINITIONS "GUI=1")
ENDIF(ALLEGRO_FOUND)
