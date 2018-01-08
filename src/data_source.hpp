#include "config_common.hpp"
#include "parameter_store.hpp"
#include <WebSocketsClient.h>
#undef NETWORK_W5100 // To fix WebSockets and NTPClientLib #define conflict
#undef NETWORK_ENC28J60
#undef NETWORK_ESP8266

typedef std::function<void(const String&)> on_price_change_t;
typedef std::function<void(const String&)> on_price_ath_t;
typedef std::function<void(void)> on_update_request_t;
typedef std::function<void(const String&)> on_announcement_t;
typedef std::function<void(const String&)> on_otp_t;
typedef std::function<void(void)> on_otp_ack_t;

class DataSource
{
public:
  DataSource(const String& host, const int port, const String& url)
    : m_host(host), m_port(port), m_url(url), m_connected(false), m_last_connected_at(0),
    m_should_send_hello(false), m_hello_sent(true), m_last_heartbeat_sent_at(0),
    m_on_price_change(nullptr), m_on_price_ath(nullptr), m_on_update_request(nullptr),  m_on_announcement(nullptr),
    m_on_otp(nullptr), m_on_otp_ack(nullptr)
  {
    m_websocket.onEvent(DataSource::s_callback);
  }

  void connect();
  void disconnect();
  void loop();

  void setOnPriceChange(on_price_change_t func) { m_on_price_change = func; }
  void setOnPriceATH(on_price_ath_t func) { m_on_price_ath = func; }
  void setOnUpdateRequest(on_update_request_t func) { m_on_update_request = func; }
  void setOnAnnouncement(on_announcement_t func) { m_on_announcement = func; }
  void setOnOTP(on_otp_t func) { m_on_otp = func; }
  void setOnOTPack(on_otp_ack_t func) { m_on_otp_ack = func; }
  bool sendOTPRequest();

  static void s_callback(WStype_t type, uint8_t * payload, size_t length);
private:
  void sendText(const String& text);
  void sendHello();
  void sendParameter(const ParameterItem *item);
  void sendAllParameters();

  void callback(WStype_t type, uint8_t * payload, size_t length);
  void textCallback(const String& text);
  void parameterCallback(const String& name, const String& value);

  const String m_host;
  const int m_port;
  const String m_url;
  bool m_connected;
  long m_last_connected_at;
  bool m_should_send_hello;
  bool m_hello_sent;
  long m_last_heartbeat_sent_at;

  static const int c_heartbeat_interval = 30 * 1000;
  static const int c_force_reconnect_interval = 120 * 1000;

  on_price_change_t m_on_price_change;
  on_price_ath_t m_on_price_ath;
  on_update_request_t m_on_update_request;
  on_announcement_t m_on_announcement;
  on_otp_t m_on_otp;
  on_otp_ack_t m_on_otp_ack;
  WebSocketsClient m_websocket;
};
