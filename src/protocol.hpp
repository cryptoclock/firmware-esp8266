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

#pragma once
#include <map>
#include <memory>
#include <functional>
#include <queue>
#include <Arduino.h>
#include "ArduinoJson.h"
#include "utils.hpp"
#include "parameter_store.hpp"

using std::shared_ptr;
using std::function;
using std::queue;

class Protocol 
{
public:
  virtual void dataReceived(const char* data, const int data_size) = 0;
  virtual String readyToSend() = 0;
  virtual void poll() = 0;
  virtual void connected() = 0;
  virtual void disconnected() = 0;
  virtual void queueText(const String& text) = 0;
  virtual void queueJSON(const JsonDocument& doc) = 0;
  virtual bool sendOTPRequest() = 0;
  virtual void sendParameter(const ParameterItem *item) = 0;
  virtual ~Protocol();
};

typedef function<void(const JsonDocument& doc)> on_command_callback_t;

class CC_Protocol : public Protocol
{
public:
  CC_Protocol(const String& protocol_name, bool is_remote);

  void poll();

  void setCommandCallback(const String& name, on_command_callback_t cb);

  bool sendOTPRequest() override;

  void sendParameter(const ParameterItem *item) override;
  void queueText(const String& text) override;
  void queueJSON(const JsonDocument& doc) override;

  void dataReceived(const char* data, const int data_size) override;
  String readyToSend() override;
  void connected() override;
  void disconnected() override;

  void importCallbacks(CC_Protocol* source);
private:
  CC_Protocol();
  void setDefaultCallbacks();

  void sendHello();
  void sendDiagnostics();
  void sendAllParameters();

  void legacyCallback(const String& text);

  void commandCallback(const JsonDocument& j);

  bool m_connected;
  bool m_should_send_hello;
  bool m_hello_sent;
 
  static constexpr int c_heartbeat_interval = 30 * 1000;
  static constexpr int c_no_data_reconnect_interval = 300 * 1000;

  queue<String> m_send_queue;
  std::map<String, on_command_callback_t> m_commands;

  String m_protocol_name;
  bool m_is_remote; // remote (network) or local (serial) communication
  long m_last_heartbeat_sent_at;
};
