#include <time.h>
#include <sys/time.h>
#include <sntp.h>
#include "ntp.hpp"

void NTP::init() 
{
  if (m_initialized)
    sntp_stop();

  connectToServer();
  sntp_init();
  m_initialized = true;
}

void NTP::connectToServer()
{
  DEBUG_SERIAL.printf("Setting SNTP to (%s)\n", m_server.c_str());
  sntp_setservername(0, (char *) m_server.c_str());
}

void NTP::setServer(const String& server)
{
  m_server = server;
  if (m_initialized)
    connectToServer();
}

void NTP::setTimezone(const int offset)
{
  sntp_set_timezone(offset);
  DEBUG_SERIAL.printf("Timezone set to '%i'\n",offset);
}

String NTP::getTime()
{
  uint32 now = sntp_get_current_timestamp();
  if (now<1514764800L) // < 2018-01-01 = system time not yet set
    return String("");

  const char *stime = sntp_get_real_time(now);
  return String(stime);
}
