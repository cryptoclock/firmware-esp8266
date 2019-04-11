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

#include "utils.hpp"

extern DataSource *g_data_source;
extern ParameterStore g_parameters;

const int json_doc_max_out_size = 512;
const int json_doc_max_in_size = 2048;

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

  // send heartbeat
  if (m_connected && millis() - m_last_heartbeat_sent_at > c_heartbeat_interval) {
    queueText("{\"type\":\"heartbeat\"}");
    m_last_heartbeat_sent_at = millis();
  }

  m_websocket.loop();

  if (m_connected && !m_send_queue.empty() && millis() - m_text_last_sent_at > 150) {
    sendText(m_send_queue.front());
    m_send_queue.pop();
    m_text_last_sent_at = millis();
  }
}

void DataSource::s_callback(WStype_t type, uint8_t * payload, size_t length)
{ g_data_source->callback(type, payload, length); }

void DataSource::sendText(const String& text)
{
  DEBUG_SERIAL.printf_P(PSTR("[WSc] Sending text: '%s'\n"), text.c_str());
  m_websocket.sendTXT(text.c_str(), text.length());
}

void DataSource::queueText(const String& text)
{
  m_send_queue.push(text);
}

void DataSource::queueJSON(const JsonDocument& doc)
{
  String text;
  serializeJson(doc,text);
  queueText(text);
}

void DataSource::sendHello()
{
  StaticJsonDocument<json_doc_max_out_size> doc;
  doc["type"] = "hello";
  doc["model"] = X_MODEL_NUMBER;
  doc["uuid"] = g_parameters["__device_uuid"];
  doc["version"] = FIRMWARE_VERSION;
  doc["firmwareChecksum"] = ESP.getSketchMD5();
  queueJSON(doc);
}

void DataSource::sendDiagnostics()
{
  StaticJsonDocument<json_doc_max_out_size> doc;
  doc["type"] = "diagnostics";
  JsonObject reason = doc.createNestedObject("lastResetReason");
  reason["cpu0"] = ESP.getResetReason();
  queueJSON(doc);
//  queueText(";DIAG last_reset_info " + ESP.getResetInfo());
}

void DataSource::sendParameter(const ParameterItem *item)
{
  if (item->name.startsWith("__")) return;
  StaticJsonDocument<json_doc_max_out_size> doc;
  doc["type"] = "parameter";
  doc["name"] = item->name;
  doc["value"] = item->value;
  queueJSON(doc);
}

void DataSource::sendAllParameters()
{
  g_parameters.iterateAllParameters([this](const ParameterItem* item) { sendParameter(item); /* delay(20); */ });
}

bool DataSource::sendOTPRequest()
{
  if (m_connected) {
    queueText("{\"type\": \"OTPRequest\"}");
    return true;
  }
  return false;
}

void DataSource::JSONCallback(const JsonDocument& doc)
{
  const char* cmd = doc["type"];
  if (cmd==nullptr) { // error
    DEBUG_SERIAL.printf_P(PSTR("[DS] Invalid JSON message received, skipping\n"));
    return;
  }

  // TODO: prepsat jako pole funktoru, podle esp32 
  if (!strcmp(cmd,"heartbeat")) {
    DEBUG_SERIAL.printf_P(PSTR("[DS] Heartbeat received\n"));
  } else if (!strcmp(cmd,"triggerReset")) {
    ESP.restart();
  } else if (!strcmp(cmd,"welcome")) {
    DEBUG_SERIAL.printf_P(PSTR("[DS] Welcome message received\n"));
  } else if (!strcmp(cmd,"tick")) {
    //[{"id":"(UUID)","value":"6456.9"},...]}
    auto data = doc["data"];
    if (data.isNull())
      return;

    for (JsonObjectConst tick: data.as<JsonArrayConst>()) {
      const String value = tick["value"];
      const String id = tick["id"];
      if (value=="null")
        continue;
      int screen_idx = m_layout.findScreenIndexByUUID(id);
      if (m_on_price_change)
        m_on_price_change(value, screen_idx);
    }
  } else if (!strcmp(cmd,"allTimeHigh")) {
   // {"type": "allTimeHigh", data: [{id: "(UUID)", "value": 99999.9},...] }
    auto data = doc["data"];
    if (data.isNull())
      return;

    for (JsonObjectConst tick: data.as<JsonArrayConst>()) {
      const String value = tick["value"];
      const String id = tick["id"];
      if (value=="null")
        continue;
      int screen_idx = m_layout.findScreenIndexByUUID(id);
      if (m_on_price_ath)
        m_on_price_ath(value, screen_idx);
    }
  } else if (!strcmp(cmd,"message")) {
    // {"type": "message", "text": "Hello there!", "target": "(UUID)"} // FIXME: use target
    const String text = doc["text"];
    if (m_on_announcement)
      m_on_announcement(text, false, 0);
  } else if (!strcmp(cmd,"staticMessage")) {
    // {"type": "staticMessage", "text": "Hello there!", "durationSecs": 60.0, "target": "(UUID)"}
    const String text = doc["text"];
    int duration = doc["durationSecs"];
    if (m_on_announcement)
      m_on_announcement(text, true, duration);
  } else if (!strcmp(cmd,"imageBegin")) { // not supported
  } else if (!strcmp(cmd,"imageEnd")) { // not supported
  } else if (!strcmp(cmd,"imageChunk")) { // not supported
  } else if (!strcmp(cmd,"setTemplate")) { // not supported
  } else if (!strcmp(cmd,"layout")) {
    auto data = doc["data"];
    if (data.isNull())
      return;

    JsonArrayConst array = data.as<JsonArrayConst>();
    m_layout.fromJSON(array);

    // FIXME: reset all screens (or only those where layout changed)
    if (m_on_price_change)
        m_on_price_change("", 0);

  } else if (!strcmp(cmd,"parameter")) {
    // {"type": "parameter", "name": "timezone", "value": 3}
    const String name = doc["name"];
    const String value = doc["value"];
    if (name == "null" || value == "null") {
      DEBUG_SERIAL.printf_P(PSTR("[DS] Error, parameter or value missing\n"));
    } else {
      DEBUG_SERIAL.printf_P(PSTR("[DS] Parameter '%s' updated to '%s'\n"),name.c_str(), value.c_str());
      parameterCallback(name, value);
    }
  } else if (!strcmp(cmd,"requestParameters")) {
    DEBUG_SERIAL.printf_P(PSTR("[DS] Parameters requested, sending\n"));
    sendAllParameters();
  } else if (!strcmp(cmd,"setTimeout")) {
    // {"type": "setTimeout", "dataReceivedTimeoutSecs": 180.0 }
    const String timeout = doc["dataReceivedTimeoutSecs"];
    if (timeout=="null")
      return;

    DEBUG_SERIAL.printf_P(PSTR("[DS] Data timeout set to '%s' secs\n"),timeout.c_str());
    if (m_on_price_timeout_set)
      m_on_price_timeout_set(timeout);
  } else if (!strcmp(cmd,"OTP")) {
    // {"type": "OTP", "password": 123456}
    const String password = doc["password"];
    if (password=="null")
      return;

    if (m_on_otp)
      m_on_otp(password);
  } else if (!strcmp(cmd,"OTP_ACK")) {
    if (m_on_otp_ack)
      m_on_otp_ack();
  } else if (!strcmp(cmd,"triggerUpdate")) {
    if (m_on_update_request) {
      disconnect();
      m_on_update_request();
    }
  } else if (!strcmp(cmd,"newSettingsLoaded")) {
    DEBUG_SERIAL.printf_P(PSTR("[DS] New settings loaded\n"));
    if (m_on_new_settings)
      m_on_new_settings();
  } else if (!strcmp(cmd,"countdown")) {
    // {"type": "countdown", "timeSecs": 60.0 }
    const String duration = doc["timeSecs"];
    if (duration=="null")
      return;
    if (m_on_countdown)
      m_on_countdown(duration);
  } else if (!strcmp(cmd,"sound")) {
    // {"type": "sound", "melodyRTTL": "StarwarsI:d=16,o=5,b=100:4e,4e,4e,8c,p,g,4e,8c,p,g,4e,4p,4b,4b,4b,8c6,p,g,4d#,8c,p,g,4e,8p"}
    const String melody = doc["melodyRTTL"];
    if (melody=="null")
      return;
    if (m_on_sound)
      m_on_sound(melody);
  } else if (!strcmp(cmd,"noath")) {
  // if (str.startsWith(";NOATH")) {
  //   if (m_on_price_ath)
  //     m_on_price_ath("off");
  } else {
    DEBUG_SERIAL.printf_P(PSTR("[DS] Unsupported command!\n"));
  }
}

void DataSource::textCallback(const String& str)
{
  if (str=="") return;
  if (str[0]=='{') {
    DynamicJsonDocument doc(json_doc_max_in_size);
    DeserializationError err = deserializeJson(doc, str);
    if (err) {
      DEBUG_SERIAL.printf_P(PSTR("[DS] Error parsing JSON message received: %s\n"),err.c_str());
      return;
    }
    JSONCallback(doc);
    return;
  }

  if (str.startsWith(";")){
    DEBUG_SERIAL.printf_P(PSTR("[WSc] Ignoring comment '%s'\n"),str.c_str());
  } else {
    if (isdigit(str.charAt(0)) ||
      (str.charAt(0)=='-' && isdigit(str.charAt(1)))
    ) {
      if (m_on_price_change)
        m_on_price_change(str, 0);
    } else {
      DEBUG_SERIAL.printf_P(PSTR("[WSc] Unknown text '%s'\n"),str.c_str());
    }
  }
}

void DataSource::parameterCallback(const String& param_name, const String& param_value)
{
  if (param_name=="" || param_name.startsWith("_"))
    return;

  if (param_name=="ticker_path") // legacy
  {
    String value = Utils::urlChangePath(g_parameters["ticker_url"],param_value);
    g_parameters.setIfExistsAndTriggerCallback("ticker_url", value, true);
  } else {
    g_parameters.setIfExistsAndTriggerCallback(param_name, param_value, true);
  }
  g_parameters.storeToEEPROM();
}

void DataSource::callback(WStype_t type, uint8_t * payload, size_t length)
{
  switch(type) {
  case WStype_DISCONNECTED:
    if (m_connected) {
      m_last_connected_at = millis();
      m_last_data_received_at = millis();
      m_connected = false;
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
    m_hello_sent = false;
    break;
  case WStype_TEXT:
    m_last_data_received_at = millis();
    m_connected = true;
    if (!m_hello_sent) {
      m_hello_sent = true;
      sendHello();
      sendAllParameters();
      sendDiagnostics();
    }

    if (payload==nullptr) {
      DEBUG_SERIAL.println(F("[WSc] got empty text!"));
    } else {
      DEBUG_SERIAL.printf_P(PSTR("[WSc] got text: %s\n"), payload);
      textCallback(String((char*)payload));
    }
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