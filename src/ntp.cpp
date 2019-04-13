#include <time.h>
#include <sys/time.h>
#include <sntp.h>
#include "ntp.hpp"
#include "log.hpp"

static const char* LOGTAG = "NTP";

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
  CCLOGI("Setting SNTP to (%s)", m_server.c_str());
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
  CCLOGI("Timezone set to '%i'",offset);
}

String NTP::getTime()
{
  if (!m_initialized)
    return "";

  auto now = getTimestamp();
  if (now==0)
    return "";

  const char *stime = sntp_get_real_time(now);
  return String(stime);
}

uint32 NTP::getTimestamp()
{
  uint32 now = sntp_get_current_timestamp();
  if (now<1514764800L) // < 2018-01-01 = system time not yet set
    return 0;

  return now;
}
