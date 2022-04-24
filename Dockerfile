FROM ubuntu:20.04

ENV DEBIAN_FRONTEND=noninteractive

RUN apt-get update && \
    apt-get install -y make git python3 python3-pip cmake libusb-1.0.0-dev && \
    apt-get autoclean -y && \
	ln -s /usr/bin/python3 /usr/bin/python

WORKDIR /srv

COPY ./ ./

RUN cp config_T5-4.7 config && \
    make

