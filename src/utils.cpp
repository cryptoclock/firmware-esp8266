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

String uint64ToString(uint64_t input) 
{
  String result = "";
  do {
    char c = input % 10;
    input /= 10;

    if (c < 10)
      c +='0';
    else
      c += 'A' - 10;
    result = c + result;
  } while (input);
  return result;
}

}
