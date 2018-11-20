/* 
  Cryptoclock ESP8266
  Copyright (C) 2018 www.cryptoclock.net <info@cryptoclock.net>

  This program is free software; you can redistribute it and/or modify
  it under the terms of the GNU General Public License as published by
  the Free Software Foundation; either version 2 of the License, or
  (at your option) any later version.

  This program is distributed in the hope that it will be useful,
  but WITHOUT ANY WARRANTY; without even the implied warranty of
  MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
  GNU General Public License for more details.

  You should have received a copy of the GNU General Public License along
  with this program; if not, write to the Free Software Foundation, Inc.,
  51 Franklin Street, Fifth Floor, Boston, MA 02110-1301 USA.
*/

#include "gyro.hpp"
#include "config_common.hpp"
#include <deque>

#include <Arduino.h>
#include "parameter_store.hpp"
#include "display.hpp"
#include "I2Cdev.h"
#include "MPU6050_6Axis_MotionApps20.h"
//#include "MPU6050.h" // not necessary if using MotionApps include file

// Arduino Wire library is required if I2Cdev I2CDEV_ARDUINO_WIRE implementation
// is used in I2Cdev.h
#if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    #include "Wire.h"
#endif

extern ParameterStore g_parameters;
extern DisplayT *g_display;

MPU6050 g_mpu;
bool g_mpu_available = false;
uint16_t g_mpu_packet_size; // expected DMP packet size (default is 42 bytes)

std::deque<int8_t> g_rotation_buffer;

void MPUsetup()
{
  // join I2C bus (I2Cdev library doesn't do this automatically)
  #if I2CDEV_IMPLEMENTATION == I2CDEV_ARDUINO_WIRE
    Wire.begin();
    Wire.setClock(400000); // 400kHz I2C clock. Comment this line if having compilation difficulties
  #elif I2CDEV_IMPLEMENTATION == I2CDEV_BUILTIN_FASTWIRE
    Fastwire::setup(400, true);
  #endif

  DEBUG_SERIAL.println(F("Initializing I2C devices..."));
  g_mpu.initialize();

  // verify connection
  DEBUG_SERIAL.println(F("Testing MPU6050 connection..."));
  bool success = g_mpu.testConnection();
  if (!success) {
    DEBUG_SERIAL.println(F("MPU6050 connection failed"));
    return;
  }
  // TODO: reinit and try again
  DEBUG_SERIAL.println(F("MPU6050 connection successfull"));
  DEBUG_SERIAL.println(F("Initializing MPU6050 DMP..."));
  uint8_t devStatus = g_mpu.dmpInitialize();

  // supply your own gyro offsets here, scaled for min sensitivity
  g_mpu.setXGyroOffset(220);
  g_mpu.setYGyroOffset(76);
  g_mpu.setZGyroOffset(-85);
  g_mpu.setZAccelOffset(1788); // 1688 factory default for my test chip

  if (devStatus != 0) {
    // 1 = initial memory load failed
    // 2 = DMP configuration updates failed
    // (if it's going to break, usually the code will be 1)
    DEBUG_SERIAL.printf_P(PSTR("MPU6050 DMP Initialization failed (code %i)\n"),devStatus);
    return;
  }
  // turn on the DMP, now that it's ready
  DEBUG_SERIAL.println(F("Enabling MPU6050 DMP..."));
  g_mpu.setDMPEnabled(true);
  g_mpu_packet_size = g_mpu.dmpGetFIFOPacketSize();
  g_mpu_available = true;

  for (int i=0;i<16;++i)
    g_rotation_buffer.push_back(-1);
}

bool closeTo(const VectorFloat& a, const VectorFloat& b, const float eps)
{
  return (std::fabs(b.x-a.x) < eps && 
          std::fabs(b.y-a.y) < eps);
}

int8_t convertGravityToDisplayRotation(const VectorFloat& gravity)
{
  const float eps = 0.66f;
  if (closeTo(gravity, { 1.0,  0.0, 0.5}, eps)) return 0; // up
  if (closeTo(gravity, {-1.0,  0.0, 0.5}, eps)) return 1; // down
  return -1;
}


int8_t getFilteredDisplayRotation()//const string& user_setting)
{
  static int filtered_rotation = 0;

  switch (g_parameters["rotate_display"].toInt()) {
    case 0: return 0;
    case 1: return 1;
    case 2: break; // auto
    default:
      return -1;
  }

  int8_t last = g_rotation_buffer.front();

  // returns display orientation if stabilized, otherwise -1
  int8_t res = std::all_of(g_rotation_buffer.begin(), g_rotation_buffer.end(), [last](int8_t i){return i == last;}) ? last : -2;

  if (res==0 || res==1)
    filtered_rotation = res;

  return filtered_rotation;
}

void MPUtick(void) 
{
  if (!g_mpu_available)
    return;

  uint16_t fifoCount = g_mpu.getFIFOCount();

  if (fifoCount < g_mpu_packet_size)
    return;

  // check for overflow
  if (fifoCount >= 1024) {
    // reset so we can continue cleanly
    g_mpu.resetFIFO();
    fifoCount = g_mpu.getFIFOCount();
//    DEBUG_SERIAL.println(F("FIFO overflow"));
  } else {
    // wait for correct available data length, should be a VERY short wait
    while (fifoCount < g_mpu_packet_size) 
      fifoCount = g_mpu.getFIFOCount();

    // read a packet from FIFO
    uint8_t fifoBuffer[64];
    g_mpu.getFIFOBytes(fifoBuffer, g_mpu_packet_size);

    // track FIFO count here in case there is > 1 packet available
    // (this lets us immediately read more without waiting for an interrupt)
    fifoCount -= g_mpu_packet_size;

    Quaternion q;           // [w, x, y, z]         quaternion container
    VectorFloat gravity;    // [x, y, z]            gravity vector

    g_mpu.dmpGetQuaternion(&q, fifoBuffer);
    g_mpu.dmpGetGravity(&gravity, &q);
    g_rotation_buffer.push_back(convertGravityToDisplayRotation(gravity));
    g_rotation_buffer.pop_front();

    int8_t rotation = getFilteredDisplayRotation();
    if (g_display)
      g_display->setRotation(rotation);
  }
}
