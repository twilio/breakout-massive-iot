cmake_minimum_required(VERSION 3.7)
project(breakout-tob-tests)

set(CMAKE_CXX_STANDARD 14)

find_package (Threads)

link_libraries(
  twilio_massive_sdk
  ${CMAKE_THREAD_LIBS_INIT}
  )

set(PLATFORM_SOURCES
	../platform/log.c
	../platform/time.c
	../platform/power.c
	)

include_directories(../platform ../../src)

add_executable(
  test_modem
  UnitMain.cpp
  InformationRN4Test.cpp
  NetworkRN4Test.cpp
  SIMRN4Test.cpp
  ${PLATFORM_SOURCES}
  )

if(ENABLE_COVERAGE)
	add_coverage(test_modem)
endif()
