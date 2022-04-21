ARG ARCH=${ARCH:-x86}

FROM --platform=linux/amd64 ubuntu:20.04 AS x86
FROM --platform=linux/arm64 nvcr.io/nvidia/l4t-base:r32.6.1 AS jetson

FROM ${ARCH} AS install-dependency
ARG ARCH
ENV ENV=${ARCH}
RUN apt-get update && apt-get install -y \
	make \
	g++ \
	libssl-dev \
	libboost-chrono-dev \
	libboost-random-dev \
	libboost-system-dev \
	&& rm -rf /var/lib/apt/lists/*

FROM install-dependency AS build-stage
ARG ARCH
ENV ENV=${ARCH}
COPY . /app
WORKDIR /app/linux-plugin-sdk
RUN /usr/bin/make 
WORKDIR /app
RUN /usr/bin/make
RUN /usr/bin/make package

FROM scratch AS output-stage
ARG ARCH
ENV ENV=${ARCH}
COPY --from=build-stage /app/${ARCH}/output/plugIN-tmate.tar.gz / 