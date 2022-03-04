FROM ubuntu:18.04
RUN apt-get update && apt-get install -y \
	make \
	g++ \
	libcurl4-openssl-dev \
	libboost1.65-dev \
	libssl-dev \
	git \
	&& rm -rf /var/lib/apt/lists/*
COPY . /build/source
WORKDIR /build/source

RUN /bin/bash buildscript.sh