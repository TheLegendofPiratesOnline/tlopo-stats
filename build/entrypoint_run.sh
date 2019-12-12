#!/bin/sh

cd /app/build
cmake -DCMAKE_BUILD_TYPE=Debug ..
make -j4
./tlopostats
