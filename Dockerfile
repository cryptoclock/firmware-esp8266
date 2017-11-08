# esp8266 base image, repo here: https://github.com/resin-io-library/dependent-base-images/tree/master/esp8266
# See more about resin base images here: http://docs.resin.io/runtime/resin-base-images/
FROM resin/esp8266 AS buildstep

# Set the working directory
WORKDIR /usr/src/app

# Set the board
ENV BOARD nodemcuv2

# Copy config files to the working directory
COPY platformio.ini ./

# Copy source files to the working directory
#COPY src/ ./src

# Copy library files to the working directory
#COPY lib/ ./lib

RUN apt-get -y update
RUN apt-get -y install git-core
RUN platformio lib install WebSockets
RUN platformio lib install U8g2
RUN platformio lib install the-real-orca/WifiManager#WiFiMulti
RUN platformio lib install TM1637

# Compile the firmware
CMD platformio run --environment $BOARD && mv .pioenvs/$BOARD/firmware.bin /assets/
