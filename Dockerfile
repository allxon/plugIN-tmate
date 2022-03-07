ARG ENV=${ENV:-x86}

FROM ubuntu:18.04 AS x86
RUN apt-get update && apt-get install -y \
	make \
	g++ \
	libcurl4-openssl-dev \
	libboost1.65-dev \
	libssl-dev \
	git \
	&& rm -rf /var/lib/apt/lists/*
ARG ENV
WORKDIR /build/source
COPY . /build/source
WORKDIR /build/source/linux-plugin-sdk
RUN /usr/bin/make
RUN /bin/bash -c 'cp ${ENV}/release_static/libadmplugin.a ../${ENV}/lib/'
WORKDIR /build/source
RUN /usr/bin/make

FROM 480737503464.dkr.ecr.ap-northeast-1.amazonaws.com/allxon/toolchain-nvidia:latest AS jetson
ARG ENV
WORKDIR /build/source
COPY . /build/source
WORKDIR /build/source/linux-plugin-sdk
RUN /usr/bin/make toolchainbuild 
RUN /bin/bash -c 'cp ${ENV}/release_static/libadmplugin.a ../${ENV}/lib/'
WORKDIR /build/source
RUN /usr/bin/make toolchainbuild 

FROM ${ENV} AS deploy-stage
ARG ENV
WORKDIR /build/source
RUN /usr/bin/make package

FROM scratch AS output-stage
ARG ENV
COPY --from=deploy-stage /build/source/${ENV}/output/* / 	