#!/bin/bash

mkdir -p cmake
cd cmake
cmake ..
make
./tests/test_owlmodemat

