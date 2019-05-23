# Don't try to build as a standalone project, build as a part of the main one by setting BUILD_SAMPLE=ON

find_package (Threads)

set(SAMPLE_SOURCES
	mqtt-sample.cpp
	../platform/log.c
	../platform/time.c
	../platform/power.c
	)

add_executable(mqtt_sample ${SAMPLE_SOURCES})

target_include_directories(mqtt_sample PUBLIC ../platform ../../src)
target_link_libraries(mqtt_sample twilio_massive_sdk)
target_link_libraries(mqtt_sample edit)
target_link_libraries(mqtt_sample ${CMAKE_THREAD_LIBS_INIT})
