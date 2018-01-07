#include "data_source.hpp"

extern DataSource *g_data_source;

void DataSource::connect()
{
  DEBUG_SERIAL.printf_P(PSTR("[Wsc] Connecting to host %s port %i url '%s'\n"),m_host.c_str(), m_port, m_url.c_str());
  m_websocket.beginSSL(m_host, m_port, m_url);
}

void DataSource::disconnect()
{
  m_websocket.disconnect();
}

void DataSource::loop()
{
  if (m_should_send_hello) {
    m_hello_sent = true;
    m_should_send_hello = false;
    sendHello();
    sendAllParameters();
  }

  if (millis() - m_last_heartbeat_sent_at > 30000) {
    sendText(";HB");
    m_last_heartbeat_sent_at = millis();
  }

  m_websocket.loop();
}

void DataSource::s_callback(WStype_t type, uint8_t * payload, size_t length)
{ g_data_source->callback(type, payload, length); }



void DataSource::sendText(const String& text)
{
  DEBUG_SERIAL.printf_P(PSTR("[WSc] Sending text: '%s'\n"), text.c_str());
  m_websocket.sendTXT(text.c_str(), text.length());
}

void DataSource::sendHello()
{
  String text = ";HELLO " + String(X_MODEL_NUMBER) + " " + g_parameters["__device_uuid"];
  sendText(text);
}

void DataSource::sendParameter(const ParameterItem *item)
{
  if (item->name.startsWith("__")) return;
  String text = ";PARAM " + item->name + " " + item->value;
  sendText(text);
}

void DataSource::sendAllParameters()
{
  g_parameters.iterateAllParameters([this](const ParameterItem* item) { sendParameter(item); delay(10); });
}

void DataSource::textCallback(const String& str)
{
  if (str=="") return;
  if (str==";UPDATE") {
    if (m_on_update_request) {
      disconnect();
      m_on_update_request();
    }
  } else if (str.startsWith(";ATH=")) { // All-Time-High
    if (m_on_price_ath)
      m_on_price_ath(str.substring(5));
  } else if (str.startsWith(";MSG ") || str.startsWith(";MSG=")) { // Announcement
    if (m_on_announcement)
      m_on_announcement(str.substring(5));
  } else if (str.startsWith(";")){
    DEBUG_SERIAL.printf_P(PSTR("[WSc] Unknown message '%s'\n"),str.c_str());
  } else {
    if (isdigit(str.charAt(0)) ||
      (str.charAt(0)=='-' && isdigit(str.charAt(1)))
    ) {
      if (m_on_price_change)
        m_on_price_change(str);
    } else {
      DEBUG_SERIAL.printf_P(PSTR("[WSc] Unknown text '%s'\n"),str.c_str());
    }
  }
}

void DataSource::callback(WStype_t type, uint8_t * payload, size_t length)
{
  switch(type) {
  case WStype_DISCONNECTED:
    DEBUG_SERIAL.printf_P(PSTR("[WSc] Disconnected!\n"));
    hexdump(payload, length);
    break;
  case WStype_CONNECTED:
    DEBUG_SERIAL.printf_P(PSTR("[WSc] Connected to url: %s\n"),  payload);
    m_hello_sent = false;
    break;
  case WStype_TEXT:
    DEBUG_SERIAL.printf_P(PSTR("[WSc] get text: %s\n"), payload);
    if (!m_hello_sent)
      m_should_send_hello = true;
    textCallback(String((char*)payload));
    break;
  case WStype_BIN:
    DEBUG_SERIAL.printf_P(PSTR("[WSc] get binary length: %u\n"), length);
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
