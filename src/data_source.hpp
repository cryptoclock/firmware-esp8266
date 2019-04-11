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
  This defines datasource (websocket) and callbacks on protocol commands received from server
*/

#pragma once
#include "config_common.hpp"
#include "parameter_store.hpp"
#include "layout.hpp"
#include "protocol.hpp"
#include <WebSocketsClient.h>
#include <queue>
#include "ArduinoJson.h"
#include "Ticker.h"


// typedef std::function<void(const String&, int)> on_price_change_t;
// typedef std::function<void(const String&, int)> on_price_ath_t;
// typedef std::function<void(void)> on_update_request_t;
// typedef std::function<void(const String&, const bool, const int)> on_announcement_t;
// typedef std::function<void(const String&)> on_otp_t;
// typedef std::function<void(const String&)> on_countdown_t;
// typedef std::function<void(const String&)> on_sound_t;
// typedef std::function<void(void)> on_otp_ack_t;
// typedef std::function<void(const String&)> on_price_timeout_set_t;
// typedef std::function<void(void)> on_new_settings_t;

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

  // void setOnPriceChange(on_price_change_t func) { m_on_price_change = func; }
  // void setOnPriceATH(on_price_ath_t func) { m_on_price_ath = func; }
  // void setOnUpdateRequest(on_update_request_t func) { m_on_update_request = func; }
  // void setOnAnnouncement(on_announcement_t func) { m_on_announcement = func; }
  // void setOnCountdown(on_countdown_t func) { m_on_countdown = func; }
  // void setOnSound(on_sound_t func) { m_on_sound = func; }
  // void setOnOTP(on_otp_t func) { m_on_otp = func; }
  // void setOnOTPack(on_otp_ack_t func) { m_on_otp_ack = func; }
  // void setOnPriceTimeoutSet(on_price_timeout_set_t func) { m_on_price_timeout_set = func; }
  // void setOnNewSettings(on_new_settings_t func) { m_on_new_settings = func; }
  // bool sendOTPRequest();

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

  // on_price_change_t m_on_price_change;
  // on_price_ath_t m_on_price_ath;
  // on_update_request_t m_on_update_request;
  // on_announcement_t m_on_announcement;
  // on_countdown_t m_on_countdown;
  // on_sound_t m_on_sound;
  // on_otp_t m_on_otp;
  // on_otp_ack_t m_on_otp_ack;
  // on_price_timeout_set_t m_on_price_timeout_set;
  // on_new_settings_t m_on_new_settings;
  WebSocketsClient m_websocket;
  std::queue<String> m_send_queue;
  int m_num_connection_tries;
  Protocol *m_protocol;
//  Layout m_layout;

  Ticker m_timeout_ticker;
};
