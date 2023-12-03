#!/bin/bash
set -e

mkdir -p build
cd build
cmake ../
cmake --build .
./${1}Demo
cd ../