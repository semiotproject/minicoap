cmake_minimum_required(VERSION 3.0 FATAL_ERROR)
project(minicoap)
add_definitions(-x c++)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR})
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/microcoap)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/examples/minicoap-light)
SET(SRC_LIST
    ${PROJECT_SOURCE_DIR}/minicoapdefinitions.h
    ${PROJECT_SOURCE_DIR}/minicoap.h
    ${PROJECT_SOURCE_DIR}/minicoap.cpp
    ${PROJECT_SOURCE_DIR}/examples/minicoap-light/minicoap-light.ino
    )
set(CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} -std=gnu++11 -Wall")
#MESSAGE(${CMAKE_CXX_FLAGS})
#MESSAGE(${SRC_LIST})
set_source_files_properties(${PROJECT_SOURCE_DIR}/examples/minicoap-light/minicoap-light.ino
    PROPERTIES LANGUAGE CXX)
add_executable(${PROJECT_NAME} ${SRC_LIST})
set_target_properties(${PROJECT_NAME}
    PROPERTIES LINKER_LANGUAGE CXX)
