#!/bin/sh

# What this does is that it prints each command that is going to be executed with a little plus.
set -x

SOURCE_DIR=`pwd`
BUILD_DIR=${BUILD_DIR:-./build}

BUILD_TYPE=${BUILD_TYPE:-debug}
INSTALL_DIR=${INSTALL_DIR:-../${BUILD_TYPE}-install}
CXX=${CXX:-g++}

ln -sf $BUILD_DIR/$BUILD_TYPE/compile_commands.json

          # CMAKE_BUILD_TYPE
          # Specifies the build type on single-configuration generators.
          # Possible values are empty, Debug, Release
mkdir -p $BUILD_DIR/$BUILD_TYPE \
  && cd $BUILD_DIR/$BUILD_TYPE \
  && cmake \
            -DCMAKE_BUILD_TYPE=$BUILD_TYPE \
            -DCMAKE_INSTALL_PREFIX=$INSTALL_DIR \
            -DCMAKE_EXPORT_COMPILE_COMMANDS=ON \
            $SOURCE_DIR \
  && make $*

# Use the following command to run all the unit tests
# at the dir $BUILD_DIR/$BUILD_TYPE :
# CTEST_OUTPUT_ON_FAILURE=TRUE make test

# cd $SOURCE_DIR && doxygen


