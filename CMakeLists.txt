cmake_minimum_required(VERSION 3.7)
project(twilio-massive-sdk)

set(CMAKE_CXX_STANDARD 14)

option(BUILD_HIL_TESTS "Build HIL tests" OFF)
option(BUILD_SAMPLE_BG96 "Build BG96 MQTT sample" OFF)
option(BUILD_SAMPLE_PAHO "Build Paho MQTT sample" OFF)

set(CMAKE_MODULE_PATH "${CMAKE_SOURCE_DIR}/cmake-modules" ${CMAKE_MODULE_PATH})

if(ENABLE_COVERAGE)
	find_package(codecov)
endif()

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

add_definitions(-DBUILD_FOR_TEST)

set(SDK_SOURCES
	src/modem/enums.cpp
	src/modem/OwlModemAT.cpp
	src/modem/OwlModemBG96.cpp
	src/modem/OwlModemCall.cpp
	src/modem/OwlModemGNSS.cpp
	src/modem/OwlModemInformation.cpp
	src/modem/OwlModemMQTTBG96.cpp
	src/modem/OwlModemNetwork.cpp
	src/modem/OwlModemNetworkRN4.cpp
	src/modem/OwlModemPDN.cpp
	src/modem/OwlModemRN4.cpp
	src/modem/OwlModemSIM.cpp
	src/modem/OwlModemSocketRN4.cpp
	src/modem/OwlModemSSLBG96.cpp
	src/modem/OwlModemSSLRN4.cpp
	src/utils/str.cpp
	src/utils/md5.cpp
	src/utils/base64.cpp
	)

add_library(twilio_massive_sdk ${SDK_SOURCES})

if(ENABLE_COVERAGE)
	add_coverage(twilio_massive_sdk)
endif()

add_subdirectory(tests)

if (BUILD_SAMPLE_BG96 OR BUILD_SAMPLE_PAHO)
  add_subdirectory(native/sample)
endif()

if (BUILD_HIL_TESTS)
  add_subdirectory(native/hiltests)
endif()
