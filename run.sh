#!/bin/zsh

cmake -DCMAKE_POLICY_VERSION_MINIMUM=3.5 -DCMAKE_CXX_STANDARD=20 -DCMAKE_CXX_STANDARD_REQUIRED=ON -DCMAKE_EXPORT_COMPILE_COMMANDS=ON -B build

cmake --build build -j 4

ctest --test-dir build --output-on-failure

./build/orm
