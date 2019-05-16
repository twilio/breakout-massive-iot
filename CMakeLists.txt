cmake_minimum_required(VERSION 3.7)
project(twilio-massive-sdk)

set(CMAKE_CXX_STANDARD 14)

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
	
	src/DTLS/enums.cpp
	src/DTLS/OwlDTLSClient.cpp

	src/CoAP/CoAPMessage.cpp
	src/CoAP/CoAPOption.cpp
	src/CoAP/CoAPPeer.cpp
	src/CoAP/enums.cpp

	src/Breakout/Breakout.cpp
	)

add_library(twilio_massive_sdk ${SDK_SOURCES})

