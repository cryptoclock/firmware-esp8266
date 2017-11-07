FROM base/archlinux

RUN mkdir -p /build /sketch
RUN pacman -Sy --noconfirm git make wget python python-pip
RUN git clone --depth=1 https://github.com/esp8266/Arduino /build/esp8266
WORKDIR /build/esp8266/tools
RUN python get.py
RUN pip install esptool

RUN git clone --depth=1 https://github.com/plerup/makeEspArduino/ /build/makeEspArduino

COPY ./8x8wifimanager /sketch

ENV BUILD_DIR /build
ENV ESP_ROOT /build/esp8266
ENV MAKE_ESP /build/makeEspArduino/makeEspArduino.mk

WORKDIR /sketch
RUN make
