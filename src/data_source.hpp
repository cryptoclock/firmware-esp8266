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

/*
  Interfaces between websocket and Protocol class
*/

#pragma once
#include "config_common.hpp"
#include "parameter_store.hpp"
#include "protocol.hpp"
#include <WebSocketsClient.h>
#include <queue>
#include "ArduinoJson.h"
#include "Ticker.h"

class DataSource
{
public:
  DataSource()
    : m_connected(false), m_last_connected_at(0),
    m_last_data_received_at(0), m_text_last_sent_at(0), m_num_connection_tries(0),
    m_protocol(nullptr)
  {
    m_websocket.onEvent(DataSource::s_callback);
    m_timeout_ticker.attach(20.0, DataSource::s_timeoutCheckCallback);
  }

  void connect();
  void disconnect();
  void reconnect();
  void loop();

  void setProtocol(Protocol *protocol) {m_protocol = protocol;}

  static void s_callback(WStype_t type, uint8_t * payload, size_t length);
private:
  void sendText(const String& text);

  // gets called by WebSocketsClient
  void callback(WStype_t type, uint8_t * payload, size_t length);

  void timeoutCheckCallback(void);
  static void s_timeoutCheckCallback(void);

  bool m_connected;
  long m_last_connected_at;
  long m_last_data_received_at;
  long m_text_last_sent_at;

  static const int c_heartbeat_interval = 30 * 1000;
  static const int c_force_reconnect_interval = 120 * 1000;
  static const int c_no_data_reconnect_interval = 300 * 1000;
  static const int c_no_data_restart_interval = 600 * 1000;

  WebSocketsClient m_websocket;
  std::queue<String> m_send_queue;
  int m_num_connection_tries;
  Protocol *m_protocol;

  Ticker m_timeout_ticker;
};
