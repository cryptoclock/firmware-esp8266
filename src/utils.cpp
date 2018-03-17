#include "utils.hpp"

const int EEPROM_SIZE = 2048;

namespace Utils {
void eeprom_BEGIN()
{
  EEPROM.begin(EEPROM_SIZE);
}

void eeprom_END()
{
  EEPROM.end();
}

void eeprom_WIPE()
{
  eeprom_BEGIN();
  eeprom_Erase(0, EEPROM_SIZE);
  eeprom_END();
}

void eeprom_WriteString(int& offset, const String &s)
{
  int length = s.length();
  const char *c_str = s.c_str();
  for (int i=0;i<=length;++i,++offset)
    EEPROM.write(offset, c_str[i]);
}

String eeprom_ReadString(int& offset, const int max_chars)
{
  String s="";
  int start_offset = offset;
  while(true) {
    char c = (char) EEPROM.read(offset++);
    if (c=='\0' || c==255) break; // NULL char or uninitialized memory
    s += c;
    if (offset - start_offset >= max_chars)
      break;
  }
  return s;
}

void eeprom_Erase(const int offset, const int size) {
    for (int i=0;i<size;++i)
      EEPROM.write(offset+i, 0x0);
}

void parseHostname(const String& hostname, String& server, int& port)
{
  int colon_pos = hostname.indexOf(':');
  if (colon_pos==-1) {
    server = hostname;
  } else {
    server = hostname.substring(0,colon_pos);
    int p = hostname.substring(colon_pos+1).toInt();
    if (p>0)
      port = p;
  }
}
void parseURL(String url, String &server, int &port, String& path, String& protocol)
{
  if (url.startsWith("http://") || url.startsWith("ws://")) {
    protocol = "ws";
    port = 80;
  } else {
    protocol = "wss";
    port = 443;
  }

  if (url.startsWith("https://") || url.startsWith("http://") || url.startsWith("wss://") || url.startsWith("ws://")) {

    url = url.substring(url.indexOf('/')+2);
  }

  int slash_pos = url.indexOf('/');
  if (slash_pos==-1) {
    parseHostname(url, server, port);
  } else {
    parseHostname(url.substring(0,slash_pos), server, port);
    path = url.substring(slash_pos);
  }

  if (path == "") path = "/";
  if (port <= 0) port = 443;
  return;
}

String urlChangePath(String url, const String& new_path)
{
  String host, path, protocol;
  int port=0;
  parseURL(url, host, port, path, protocol);
  return String(protocol + "://" + host + ":" + port + new_path);
}

String shortenText(const String& text, const int lead)
{
  return (text.substring(0,lead) + ".." + text.substring(text.length()-lead-1,text.length()-1));
}

}
