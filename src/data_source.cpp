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

#include "data_source.hpp"
#include "protocol.hpp"
#include "utils.hpp"


extern DataSource *g_data_source;
extern ParameterStore g_parameters;

void DataSource::connect()
{
  String host, path, protocol;
  int port;

  String ticker_url = g_parameters["ticker_url"];
  Utils::parseURL(ticker_url, host, port, path, protocol);
  path += "?uuid=" + g_parameters["__device_uuid"] + "&format=json";

  DEBUG_SERIAL.printf_P(PSTR("[Wsc] Connecting to protocol '%s' host '%s' port '%i' url '%s'\n"),protocol.c_str(),host.c_str(), port, path.c_str());
  if (protocol=="ws")
    m_websocket.begin(host, port, path);
  else
    m_websocket.beginSSL(host, port, path);
}

void DataSource::disconnect()
{
  m_websocket.disconnect();
}

void DataSource::reconnect()
{
  m_last_connected_at = millis();
  m_last_data_received_at = millis();
  disconnect();
  connect();
}

void DataSource::loop()
{
  // force reconnect
  if (!m_connected && (millis() - m_last_connected_at > c_force_reconnect_interval)) {
    DEBUG_SERIAL.printf_P(PSTR("[WSc] Couldn't autoconnect for %i secs, forcing restart\n"), c_force_reconnect_interval / 1000);
    ESP.restart();
  }

  if (m_connected && (millis() - m_last_data_received_at > c_no_data_reconnect_interval)) {
    if (++m_num_connection_tries < 2) {
      DEBUG_SERIAL.printf_P(PSTR("[WSc] No data received for %i secs, forcing reconnect\n"), c_no_data_reconnect_interval / 1000);
      reconnect();
    } else {
      DEBUG_SERIAL.println(F("[WSc] Max reconnection attempts, forcing restart"));
      ESP.restart();
    } 
  }

  m_websocket.loop();

  m_protocol->poll();

  if (m_protocol && m_connected && millis() - m_text_last_sent_at > 150) {
    const String& text = m_protocol->readyToSend();
    if (text!="") {
      sendText(text);
      m_text_last_sent_at = millis();
    }
  }
}

void DataSource::s_callback(WStype_t type, uint8_t * payload, size_t length)
{ g_data_source->callback(type, payload, length); }

void DataSource::sendText(const String& text)
{
  DEBUG_SERIAL.printf_P(PSTR("[WSc] Sending text: '%s'\n"), text.c_str());
  m_websocket.sendTXT(text.c_str(), text.length());
}

void DataSource::callback(WStype_t type, uint8_t * payload, size_t length)
{
  switch(type) {
  case WStype_DISCONNECTED:
    if (m_connected) {
      m_last_connected_at = millis();
      m_last_data_received_at = millis();
      m_connected = false;
      if (m_protocol)
        m_protocol->disconnected();
    }

    DEBUG_SERIAL.printf_P(PSTR("[WSc] Disconnected!\n"));
    hexdump(payload, length);
    break;
  case WStype_CONNECTED:
    m_connected = true;
    m_last_connected_at = millis();
    m_last_data_received_at = millis();
    m_num_connection_tries = 0;
    if (payload==nullptr)
      DEBUG_SERIAL.printf_P(PSTR("[WSc] Connected to url: <nullptr>\n"));
    else
      DEBUG_SERIAL.printf_P(PSTR("[WSc] Connected to url: %s\n"),payload);
    if (m_protocol)
      m_protocol->connected();
    break;
  case WStype_TEXT:
    m_last_data_received_at = millis();
    m_connected = true;

    if (m_protocol)
      m_protocol->dataReceived((const char*) payload, length);
    break;
  case WStype_BIN:
    m_last_data_received_at = millis();
    DEBUG_SERIAL.printf_P(PSTR("[WSc] got binary, length: %u\n"), length);
    hexdump(payload, length);
    break;
  case WStype_ERROR:
  case WStype_FRAGMENT:
  case WStype_FRAGMENT_BIN_START:
  case WStype_FRAGMENT_TEXT_START:
  case WStype_FRAGMENT_FIN:
  default:
    break;
  }
}

void DataSource::s_timeoutCheckCallback()
{
  g_data_source->timeoutCheckCallback();
}

void DataSource::timeoutCheckCallback()
{
  if (millis() - m_last_data_received_at > c_no_data_restart_interval) {
    DEBUG_SERIAL.printf_P(PSTR("[WSc] No data received for %i secs, forcing reset\n"), c_no_data_restart_interval / 1000);
    ESP.restart();
  }
}