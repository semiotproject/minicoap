cmake_minimum_required(VERSION 3.1 FATAL_ERROR)
project(minicoap)
add_definitions(
    -x c++
)

set(CMAKE_CXX_STANDARD 11)

# //////
# TODO: cmake for esp8266 arduino
find_path(WiringPi wiringPi.h)
IF(WiringPi)
  message(STATUS "WiringPi include was found in ${WiringPi}...")
  find_library(WIRINGPI_LIBRARY NAMES libwiringPi.so)
  IF(WIRINGPI_LIBRARY)
    message(STATUS "WiringPi library was found in ${WIRINGPI_LIBRARY}...")
    add_definitions(-DWITH_GPIO)
  else()
    message(STATUS "==== WiringPi library not found. GPIO support disabled.")
  message(STATUS "==== (Please follow http://wiringpi.com/download-and-install/ if you want to use GPIO.)")
  ENDIF()
else()
  message(STATUS "==== WiringPi include file (wiringPi.h) not found. GPIO support disabled.")
  message(STATUS "==== (Please follow http://wiringpi.com/download-and-install/ if you want to use GPIO.)")
ENDIF(WiringPi)

# //////

INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR})
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/microcoap)
INCLUDE_DIRECTORIES(${PROJECT_SOURCE_DIR}/examples/minicoap-light)
SET(SRC_LIST
    ${PROJECT_SOURCE_DIR}/examples/minicoap-light/definitions.h
    ${PROJECT_SOURCE_DIR}/minicoapdefinitions.h
    ${PROJECT_SOURCE_DIR}/minicoap.h
    ${PROJECT_SOURCE_DIR}/minicoap.cpp
    ${PROJECT_SOURCE_DIR}/coapresource.h
    ${PROJECT_SOURCE_DIR}/coapresource.cpp
    ${PROJECT_SOURCE_DIR}/wellknowncoreresource.h
    ${PROJECT_SOURCE_DIR}/wellknowncoreresource.cpp
    ${PROJECT_SOURCE_DIR}/configresource.h
    ${PROJECT_SOURCE_DIR}/configresource.cpp
    ${PROJECT_SOURCE_DIR}/examples/minicoap-light/lightresource.h
    ${PROJECT_SOURCE_DIR}/examples/minicoap-light/lightresource.cpp
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
target_compile_features(minicoap PRIVATE cxx_range_for)
target_link_libraries(minicoap ${WIRINGPI_LIBRARY})

