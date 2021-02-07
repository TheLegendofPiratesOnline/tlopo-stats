# For building locally
# Do not use this on production!

FROM alpine:3.8
MAINTAINER Nacib Neme

# N.B. Using 3.8 because it installs an old version of boost (1.66.0-r0)

RUN apk update && apk add \
  make \
  boost-dev=1.66.0-r0 \
  build-base \
  clang \
  clang-dev \
  cmake \
  git \
  hiredis-dev \
  jansson-dev \
  libev-dev \
  openssh-client \
  perl

RUN rm /usr/bin/g++ && rm /usr/bin/gcc && ln -s $(which clang++) /usr/bin/g++ && ln -s $(which clang++) /usr/bin/gcc

RUN mkdir -p /build

WORKDIR /build

RUN git clone https://github.com/hmartiro/redox \
    && cd redox \
    && mkdir build \
    && cd build \
    && cmake -Dlib=off -Dstatic_lib=on .. \
    && make -j \
    && make install

COPY ./build/get-pip-27.py /tmp
RUN python /tmp/get-pip-27.py && python -m pip install redis

COPY . /app
WORKDIR /app/build
RUN cmake -DCMAKE_BUILD_TYPE=Debug .. && make -j
