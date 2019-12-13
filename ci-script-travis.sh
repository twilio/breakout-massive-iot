#!/bin/bash

set -e

SOURCE_DIR=$(pwd)

mkdir -p cmake
cd cmake
cmake ..
make

${SOURCE_DIR}/tests/scripts/hedwig-dispatcher.py --baserepo https://github.com/twilio/breakout-massive-iot --prrepo https://github.com/twilio/breakout-massive-iot --baseref ${TRAVIS_BRANCH} --prref ${TRAVIS_PULL_REQUEST_BRANCH} --extra-args ${CODECOV_TOKEN} ${SECRET_HEDWIG_WORKERS}
