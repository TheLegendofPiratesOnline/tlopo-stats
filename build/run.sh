#!/bin/sh

rm -f ./tlopostats
docker image build -t tlopostats -f Dockerfile ..
docker run --rm -it --net host -entrypoint=/bin/sh tlopostats /app/build/entrypoint_run.sh "$@"
