#!/bin/sh

BUILD_DIR=/var/up2date-cpp/

cd "$BUILD_DIR" && rm -rf build && mkdir build && cd build && cmake -DBUILD_EXAMPLES=ON ..
make && cd "$BUILD_DIR" && chmod -R 777 build