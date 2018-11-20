Cryptoclock ESP8266
--------------------


Mode of Operation

Getting Started
- platformio (vscode/atom)
- platformio build / upload
- hw requirements
- routing
- models and configuration
- quirks of libraries

- configuration:
 - config_model.hpp
 - main.cpp -> void setupParameters() do config.h ?
 - main.cpp -> setupDisplay()

- protocol description
- OTA description


Licence
--------
Licensed under GNU GPLv2 license (see file LICENSE for details)

Used Libraries
---------------
  WiFiManager (https://github.com/tzapu/WiFiManager) - AP captive portal to configure device over WiFi
  WebSockets (https://travis-ci.org/Links2004/arduinoWebSockets)
  NtpClientLib (https://github.com/gmag11/NtpClient)
  ESP8266TrueRandom (https://github.com/marvinroger/ESP8266TrueRandom) - UUID generation
  I2Cdevlib-MPU6050 (https://github.com/jrowberg/i2cdevlib.git) - i2c library for controlling MPU6050 gyroscope/accelerometer
  Lixie-Arduino (https://github.com/connornishijima/Lixie-arduino) - driver for Lixie displays
  TM1637 (https://github.com/avishorp/TM1637) - driver for TM1637 7-segment LED displays
  Time (http://playground.arduino.cc/code/time) - dependency of NtpClientLib
  FastLED (http://fastled.io) - dependency of Lixie-Arduino
