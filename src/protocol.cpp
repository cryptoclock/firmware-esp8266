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

#include "protocol.hpp"
#include "parameter_store.hpp"
#include "utils.hpp"
//#include "fonts.hpp"

const int json_doc_max_out_size = 2048;
const int json_doc_max_in_size = 2048;

extern ParameterStore g_parameters;
extern WiFiCore *g_wifi;

// extern Fonts g_Fonts;

#include "log.hpp"
static const char* LOGTAG = "Protocol";

static const char *g_allowed_commands[] = {
  "welcome", "parameter", "heartbeat", "OTP", "OTP_ACK", 
  "message", "staticMessage", "triggerUpdate","triggerReset", "requestParameters", 
  "countdown", "sound",  "setTimeout", "tick", "layout", 
  "allTimeHigh", "newSettingsLoaded", "imageBegin", "imageChunk", "imageEnd", 
  "setTemplate", 
  "getDeviceInfo","addWiFiAP","removeWiFiAP","getWiFiAPs"};

// subset of g_allowed_commands, local-only (eg. over serial) commands
static const char *g_local_commands[] = {
  "getDeviceInfo","addWiFiAP","removeWiFiAP","getWiFiAPs"
};

// TODO: local commands 
// getDeviceInfo (include screen IDs and info), 
// eraseEEPROM, 
// getdiagnostic info (wifi signal strenght etc.)

constexpr int c_n_commands = sizeof(g_allowed_commands)/sizeof(char *);
constexpr int c_n_local_commands = sizeof(g_local_commands)/sizeof(char *);

bool isLocalOnlyCommand(const String& command)
{
  for(int i=0;i<c_n_local_commands;++i)
    if (command==g_local_commands[i])
      return true;

  return false;
}

void CC_Protocol::setCommandCallback(const String& name, on_command_callback_t cb)
{
  for(int i=0;i<c_n_commands;++i) {
    if (name==g_allowed_commands[i]) {
      m_commands[i] = cb;
      return;
    }
  }
  CCLOGE("Callback for command '%s' not allowed", name.c_str());
  return;
}

void CC_Protocol::dataReceived(const char* data, const int data_size)
{
  m_connected = true;
  if (!m_hello_sent) {
    m_hello_sent = true;
    sendHello();
    sendAllParameters();
    sendDiagnostics();
//    sendAvailableFonts();
  }
  if (data_size <2 || data[0] =='\0') {
    CCLOGW("Got empty text!");
    return;
  }
  CCLOGI("Received data: %.*s", std::min(1024, data_size), data);

  String str(data);

  if (str=="") return;
  if (str.startsWith("{")) {
    DynamicJsonDocument doc(json_doc_max_in_size);
    DeserializationError err = deserializeJson(doc, str);
    if (err) {
      CCLOGE("Error parsing JSON message received: %s",err.c_str());
      return;
    }
    commandCallback(doc);
    return;
  } else if (str.startsWith(";")){
    CCLOGI("Ignoring comment '%s'",str.c_str());
  } else {
    if (isdigit(str.charAt(0)) ||
      (str.charAt(0)=='-' && isdigit(str.charAt(1)))
    ) {
      legacyCallback(str);
    } else {
      CCLOGE("Unknown text '%s'",str.c_str());
    }
  }
}

// legacy callback
void CC_Protocol::legacyCallback(const String& text)
{
  DynamicJsonDocument j(json_doc_max_out_size);

  j["type"] = "tick";
  JsonArray data = j.createNestedArray("data");
  data[0]["id"] = "12345";
  data[0]["value"] = text;

  commandCallback(j);
}
  
void CC_Protocol::commandCallback(const JsonDocument& j)
{
  const String command = j["type"];
//  command.toLowerCase();
  for (int i=0;i<c_n_commands;++i) {
    if (command == g_allowed_commands[i]) {
      auto func = m_commands[i];
      if (func==nullptr) {
        CCLOGW("Command '%s' has no defined callback function",command.c_str());
      } else if(m_is_remote && isLocalOnlyCommand(command)) {
        CCLOGE("Command '%s' is local only, can't be executed over remote connection",command.c_str());
      } else {
        func(j);
      }
      return;
    }
  }

  CCLOGW("Unrecognized command '%s'",command.c_str());
  return;
}

String CC_Protocol::readyToSend()
{
  if (m_send_queue.empty())
    return "";
  
  String str = m_send_queue.front();
  m_send_queue.pop();
  return str;
}

void CC_Protocol::connected()
{
  m_connected = true;
  m_hello_sent = false;
  m_last_heartbeat_sent_at = millis();
  CCLOGI("Connected");
}

void CC_Protocol::disconnected()
{
  if (m_connected)
    m_connected = false;

  CCLOGI("Disconnected");
}

CC_Protocol::CC_Protocol(const String& protocol_name, bool is_remote) : 
  m_connected(false), m_should_send_hello(false), m_hello_sent(true),
  m_protocol_name(protocol_name), m_is_remote(is_remote), m_last_heartbeat_sent_at(0)
{
  m_commands = new on_command_callback_t[c_n_commands];
  for(int i=0;i<c_n_commands;++i)
     m_commands[i] = nullptr;

  setDefaultCallbacks();
}

void CC_Protocol::setDefaultCallbacks()
{
  setCommandCallback("heartbeat", [this](const JsonDocument& j) { CCLOGI("Received heartbeat"); });
  setCommandCallback("requestParameters", [this](const JsonDocument& j) { 
    CCLOGI("ParaImeters requested, sending"); 
    sendAllParameters();
  });

  setCommandCallback("parameter", [this](const JsonDocument& j) { 
    const String name = j["name"];
    const String value = j["value"];
    if (name == "null" || value == "null") {
      CCLOGE("Error, parameter or value missing");
    } else {
      if (name=="" || name.startsWith("_"))
        return;

      if (name=="ticker_path") // legacy
      {
        String v = Utils::urlChangePath(g_parameters["ticker_url"],value);
        g_parameters.setIfExistsAndTriggerCallback("ticker_url", v, true);
      } else {
        g_parameters.setIfExistsAndTriggerCallback(name, value, true);
      }
      g_parameters.storeToEEPROM();
      CCLOGI("Parameter '%s' updated to '%s'",name.c_str(), value.c_str());
    }
  });

  if (m_is_remote == false) {
    setCommandCallback("getDeviceInfo", [this](const JsonDocument& j) {
      sendHello();
    });
  }
}

void CC_Protocol::importCallbacks(CC_Protocol* source)
{
  for (int i=0;i<c_n_commands;++i)
    if (m_commands[i]==nullptr) // don't replace default callbacks
      m_commands[i] = source->m_commands[i];
}

void CC_Protocol::queueText(const String& text)
{
  if (m_connected)
    m_send_queue.push(text);
}

void CC_Protocol::queueJSON(const JsonDocument& doc)
{
  String text;
  serializeJson(doc,text);
  queueText(text);
}

void CC_Protocol::sendHello()
{
  StaticJsonDocument<json_doc_max_out_size> doc;
  doc["type"] = "hello";
  doc["model"] = X_MODEL_NUMBER;
  doc["uuid"] = g_parameters["__device_uuid"];
  doc["version"] = FIRMWARE_VERSION;
  doc["firmwareChecksum"] = ESP.getSketchMD5();
  queueJSON(doc);
}

void CC_Protocol::sendDiagnostics()
{
  StaticJsonDocument<json_doc_max_out_size> doc;
  doc["type"] = "diagnostics";
  JsonObject reason = doc.createNestedObject("lastResetReason");
  reason["cpu0"] = ESP.getResetReason();
  queueJSON(doc);
}

void CC_Protocol::sendAllParameters()
{
  StaticJsonDocument<json_doc_max_out_size> doc;
  doc["type"] = "parametersStart";
  queueJSON(doc);

  g_parameters.iterateAllParameters([this](const ParameterItem* item) { sendParameter(item); });

  doc["type"] = "parametersEnd";
  queueJSON(doc);
}

void CC_Protocol::sendParameter(const ParameterItem *item)
{
  if (item->name.startsWith("__")) return;
  StaticJsonDocument<json_doc_max_out_size> doc;
  doc["type"] = "parameter";
  doc["name"] = item->name;
  doc["value"] = item->value;
  queueJSON(doc);
}

void CC_Protocol::sendAPlist()
{
  DynamicJsonDocument doc(json_doc_max_out_size);

  doc["type"] = "WiFiAPs";
  JsonArray data = doc.createNestedArray("data");
  auto aps = g_wifi->getAPs();
  for (auto& ap : aps) {
    JsonObject obj = data.createNestedObject();
    obj["SSID"] = ap.first;
    obj["password"] = ap.second;
    
  }
  queueJSON(doc);
}

bool CC_Protocol::sendOTPRequest()
{
  if (m_connected) {
    queueText("{\"type\": \"OTPRequest\"}");
    return true;
  }
  return false;
}

void CC_Protocol::poll() 
{
  if (m_is_remote && m_connected && millis() - m_last_heartbeat_sent_at > c_heartbeat_interval) {
    queueText("{\"type\":\"heartbeat\"}");
    m_last_heartbeat_sent_at = millis();
  }
  return;
}

Protocol::~Protocol() {}
