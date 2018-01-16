#include "data_source.hpp"

extern DataSource *g_data_source;
extern ParameterStore g_parameters;

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
  // force reconnect
  if (!m_connected && (millis() - m_last_connected_at > c_force_reconnect_interval)) {
    m_last_connected_at = millis();
    disconnect();
    connect();
  }

  if (m_connected && m_should_send_hello) {
    m_hello_sent = true;
    m_should_send_hello = false;
    sendHello();
    sendAllParameters();
  }

  if (m_connected && millis() - m_last_heartbeat_sent_at > c_heartbeat_interval) {
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
  String text = ";HELLO " + String(X_MODEL_NUMBER) + " " +
    g_parameters["__device_uuid"] + " " + FIRMWARE_VERSION + " " + ESP.getSketchMD5();
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
  g_parameters.iterateAllParameters([this](const ParameterItem* item) { sendParameter(item); delay(20); });
}

bool DataSource::sendOTPRequest()
{
  if (m_connected) {
    sendText(";OTP_REQ");
    return true;
  }
  return false;
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
  } else if (str.startsWith(";PARAM ")) { // parameter update
    String pair = str.substring(7);
    int index = pair.indexOf(" ");
    String param_name = pair.substring(0,index);
    String param_value = pair.substring(index);
    DEBUG_SERIAL.printf_P(PSTR("[WSc] Parameter '%s' updated to '%s'\n"),param_name.c_str(), param_value.c_str());
    parameterCallback(param_name, param_value);
  } else if (str.startsWith(";OTP ")||str.startsWith(";OTP=")) { // OTP
    if (m_on_otp)
      m_on_otp(str.substring(5));
  } else if (str.startsWith(";OTP_ACK")) { // OTP acknowledge
    if (m_on_otp_ack)
      m_on_otp_ack();
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

void DataSource::parameterCallback(const String& param_name, const String& param_value)
{
  if (param_name=="" || param_name.startsWith("_"))
    return;
  g_parameters.setValue(param_name, param_value);
}

void DataSource::callback(WStype_t type, uint8_t * payload, size_t length)
{
  switch(type) {
  case WStype_DISCONNECTED:
    if (m_connected) {
      m_last_connected_at = millis();
      m_connected = false;
    }

    DEBUG_SERIAL.printf_P(PSTR("[WSc] Disconnected!\n"));
    hexdump(payload, length);
    break;
  case WStype_CONNECTED:
    m_connected = true;
    m_last_connected_at = millis();
    if (payload==nullptr)
      DEBUG_SERIAL.printf_P(PSTR("[WSc] Connected to url: <nullptr>\n"));
    else
      DEBUG_SERIAL.printf_P(PSTR("[WSc] Connected to url: %s\n"),payload);
    m_hello_sent = false;
    break;
  case WStype_TEXT:
    m_connected = true;
    if (!m_hello_sent)
      m_should_send_hello = true;

    if (payload==nullptr) {
      DEBUG_SERIAL.println(F("[WSc] got empty text!"));
    } else {
      DEBUG_SERIAL.printf_P(PSTR("[WSc] got text: %s\n"), payload);
      textCallback(String((char*)payload));
    }
    break;
  case WStype_BIN:
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
