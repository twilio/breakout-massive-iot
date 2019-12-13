#!/bin/bash

set -e

mkdir -p cmake
cd cmake
cmake -DBUILD_HIL_TESTS=ON -DENABLE_COVERAGE=ON ..
make

# Allow overriding modems.csv locally to test this script
#   modems.csv.local is not used in the CI process

if [ -f ../tests/scripts/modems.csv.local ]; then
	MODEMS_CSV_FILE=../tests/scripts/modems.csv.local
else
	MODEMS_CSV_FILE=../tests/scripts/modems.csv
fi

../tests/scripts/test_runner.py ${HEDWIG_LOCATION} ${MODEMS_CSV_FILE} ./native/hiltests/test_modem
./tests/test_owlmodemat
make twilio_massive_sdk-gcov

export CODECOV_TOKEN="${1}"
bash <(curl -s https://codecov.io/bash)

