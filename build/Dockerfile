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
  jansson-dev \
  openssh-client \
  perl \
  scons

RUN rm /usr/bin/g++ && rm /usr/bin/gcc && ln -s $(which clang++) /usr/bin/g++ && ln -s $(which clang++) /usr/bin/gcc

RUN mkdir -p /build

WORKDIR /build

RUN wget https://github.com/mongodb/mongo-c-driver/releases/download/1.1.10/mongo-c-driver-1.1.10.tar.gz \
    && tar xzf mongo-c-driver-1.1.10.tar.gz \
    && rm mongo-c-driver-1.1.10.tar.gz \
    && cd mongo-c-driver-1.1.10 \
    && ./configure \
    && make -j4 \
    && make install

RUN git clone https://github.com/mongodb/mongo-cxx-driver -b legacy \
    && cd mongo-cxx-driver \
    && scons --sharedclient --disable-warnings-as-errors install \
    && cp -r build/install/* /usr/local

COPY . /app
WORKDIR /app/build
