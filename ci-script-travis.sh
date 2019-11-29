#!/bin/bash

mkdir -p cmake
cd cmake
cmake -DENABLE_COVERAGE=ON ..
make
./tests/test_owlmodemat
make twilio_massive_sdk-gcov
