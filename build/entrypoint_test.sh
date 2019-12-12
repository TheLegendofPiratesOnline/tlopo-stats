#!/bin/sh

cd /tmp
wget https://bootstrap.pypa.io/get-pip.py
python get-pip.py
python -m pip install pymongo

cd /app/build
cmake -DCMAKE_BUILD_TYPE=Debug ..
make -j4

cd ../test
ln -fs /app/build/tlopostats .
python test_$1.py
