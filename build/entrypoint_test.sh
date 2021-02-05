#!/bin/sh

cd /app/build
python get-pip-27.py
python -m pip install redis
cmake -DCMAKE_BUILD_TYPE=Debug ..
make -j4

cd ../test
ln -fs /app/build/tlopostats .
python test_$1.py $2
