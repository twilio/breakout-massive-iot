cmake_minimum_required(VERSION 3.7)
project(twilio-massive-sdk)

set(CMAKE_CXX_STANDARD 14)

option(BUILD_SAMPLE "Build MQTT sample" OFF)

include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../src)

add_definitions(-DBUILD_FOR_TEST)

set(SDK_SOURCES
	src/modem/enums.cpp
	src/modem/OwlModemAT.cpp
	src/modem/OwlModemBG96.cpp
	src/modem/OwlModemGNSS.cpp
	src/modem/OwlModemInformation.cpp
	src/modem/OwlModemMQTTBG96.cpp
	src/modem/OwlModemNetwork.cpp
	src/modem/OwlModemPDN.cpp
	src/modem/OwlModemRN4.cpp
	src/modem/OwlModemSIM.cpp
	src/modem/OwlModemSocket.cpp
	src/modem/OwlModemSSLBG96.cpp

	src/utils/str.c
	src/utils/md5.c
	src/utils/base64.c
	)

add_library(twilio_massive_sdk ${SDK_SOURCES})

add_subdirectory(tests)

if (BUILD_SAMPLE)
  add_subdirectory(native/sample)
endif(BUILD_SAMPLE)
