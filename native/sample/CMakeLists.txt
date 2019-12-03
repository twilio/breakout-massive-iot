# Don't try to build as a standalone project, build as a part of the main one by setting BUILD_SAMPLE=ON

find_package (Threads)

set(PLATFORM_SOURCES
	../platform/log.cpp
	../platform/time.cpp
	../platform/power.cpp
	)

if(BUILD_SAMPLE_BG96)
	add_executable(bg96_mqtt_sample bg96-mqtt-sample.cpp ${PLATFORM_SOURCES})

	target_include_directories(bg96_mqtt_sample PUBLIC ../platform ../../src)
	target_link_libraries(bg96_mqtt_sample twilio_massive_sdk)
	target_link_libraries(bg96_mqtt_sample edit)
	target_link_libraries(bg96_mqtt_sample ${CMAKE_THREAD_LIBS_INIT})
endif(BUILD_SAMPLE_BG96)

if(BUILD_SAMPLE_PAHO)
	if("${PAHO_PROJECT_PATH}" STREQUAL "")
		message(FATAL_ERROR "PAHO_PROJECT_PATH is not set. Please clone https://github.com/monstrenyatko/ArduinoMqtt, and point this variable to where it is located")
	endif()

	set(MQTT_SOURCES "${PAHO_PROJECT_PATH}/src/MQTTPacket/MQTTConnectClient.c"
			 "${PAHO_PROJECT_PATH}/src/MQTTPacket/MQTTConnectServer.c"
			 "${PAHO_PROJECT_PATH}/src/MQTTPacket/MQTTDeserializePublish.c"
			 "${PAHO_PROJECT_PATH}/src/MQTTPacket/MQTTFormat.c"
			 "${PAHO_PROJECT_PATH}/src/MQTTPacket/MQTTPacket.c"
			 "${PAHO_PROJECT_PATH}/src/MQTTPacket/MQTTSerializePublish.c"
			 "${PAHO_PROJECT_PATH}/src/MQTTPacket/MQTTSubscribeClient.c"
			 "${PAHO_PROJECT_PATH}/src/MQTTPacket/MQTTSubscribeServer.c"
			 "${PAHO_PROJECT_PATH}/src/MQTTPacket/MQTTUnsubscribeClient.c"
			 "${PAHO_PROJECT_PATH}/src/MQTTPacket/MQTTUnsubscribeServer.c"
	)
	add_executable(rn4_paho_mqtt_sample rn4-paho-mqtt-sample.cpp ../../src/shims/paho-mqtt/RN4PahoIPStack.cpp ${PLATFORM_SOURCES} ${MQTT_SOURCES})

	target_include_directories(rn4_paho_mqtt_sample PUBLIC ../platform ../../src "${PAHO_PROJECT_PATH}/src")
	target_link_libraries(rn4_paho_mqtt_sample twilio_massive_sdk)
	target_link_libraries(rn4_paho_mqtt_sample ${CMAKE_THREAD_LIBS_INIT})
endif(BUILD_SAMPLE_PAHO)
