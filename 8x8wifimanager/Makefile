BUILD_DIR ?= ./build
BOARD ?= nodemcuv2
ESP_ROOT ?= /usr/share/arduino/hardware/esp8266com/esp8266
ESP_ROOT ?= ./Arduino
MAKE_ESP ?= ./makeEspArduino.mk

F_CPU = 160000000L

.PHONY: all
all:
	mv ./build/8x8wifimanager.bin ./export
	make clean

include $(MAKE_ESP)
