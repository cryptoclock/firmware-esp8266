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
#include <umm_malloc/umm_malloc.h>
#include "parameter_store.hpp"
extern ParameterStore g_parameters;
const int EEPROM_SIZE = 2048;

const size_t block_size = 8;

size_t getTotalAvailableMemory() {
  umm_info(0, 0);
  return ummHeapInfo.freeBlocks * block_size;
}

size_t getLargestAvailableBlock() {
  umm_info(0, 0);
  return ummHeapInfo.maxFreeContiguousBlocks * block_size;
}

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

float getMemoryFragmentation() {
  return 100 - getLargestAvailableBlock() * 100.0 / getTotalAvailableMemory();
}


String getDeviceInfo(const String& sep)
{
  String info= "v" FIRMWARE_VERSION;
  info += sep + __DATE__ " " __TIME__;
  info += sep + "MD5: " + ESP.getSketchMD5().substring(0,6);
  info += sep + "UUID: " + g_parameters["__device_uuid"].substring(0,6);
  info += sep + "SDK: " + String(ESP.getSdkVersion());
  return info;
}

String getDisconnectReason(WiFiDisconnectReason reason)
{
  switch (reason) {
    case WIFI_DISCONNECT_REASON_UNSPECIFIED:                return F("Unspecified");
    case WIFI_DISCONNECT_REASON_AUTH_EXPIRE:                return F("Auth expire");
    case WIFI_DISCONNECT_REASON_AUTH_LEAVE:                 return F("Auth leave");
    case WIFI_DISCONNECT_REASON_ASSOC_EXPIRE:               return F("Assoc expire");
    case WIFI_DISCONNECT_REASON_ASSOC_TOOMANY:              return F("Assoc toomany");
    case WIFI_DISCONNECT_REASON_NOT_AUTHED:                 return F("Not authed");
    case WIFI_DISCONNECT_REASON_NOT_ASSOCED:                return F("Not assoced");
    case WIFI_DISCONNECT_REASON_ASSOC_LEAVE:                return F("Assoc leave");
    case WIFI_DISCONNECT_REASON_ASSOC_NOT_AUTHED:           return F("Assoc not authed");
    case WIFI_DISCONNECT_REASON_DISASSOC_PWRCAP_BAD:        return F("Disassoc pwrcap bad");
    case WIFI_DISCONNECT_REASON_DISASSOC_SUPCHAN_BAD:       return F("Disassoc supchan bad");
    case WIFI_DISCONNECT_REASON_IE_INVALID:                 return F("IE invalid");
    case WIFI_DISCONNECT_REASON_MIC_FAILURE:                return F("Mic failure");
    case WIFI_DISCONNECT_REASON_4WAY_HANDSHAKE_TIMEOUT:     return F("4way handshake timeout");
    case WIFI_DISCONNECT_REASON_GROUP_KEY_UPDATE_TIMEOUT:   return F("Group key update timeout");
    case WIFI_DISCONNECT_REASON_IE_IN_4WAY_DIFFERS:         return F("IE in 4way differs");
    case WIFI_DISCONNECT_REASON_GROUP_CIPHER_INVALID:       return F("Group cipher invalid");
    case WIFI_DISCONNECT_REASON_PAIRWISE_CIPHER_INVALID:    return F("Pairwise cipher invalid");
    case WIFI_DISCONNECT_REASON_AKMP_INVALID:               return F("AKMP invalid");
    case WIFI_DISCONNECT_REASON_UNSUPP_RSN_IE_VERSION:      return F("Unsupp RSN IE version");
    case WIFI_DISCONNECT_REASON_INVALID_RSN_IE_CAP:         return F("Invalid RSN IE cap");
    case WIFI_DISCONNECT_REASON_802_1X_AUTH_FAILED:         return F("802 1X auth failed");
    case WIFI_DISCONNECT_REASON_CIPHER_SUITE_REJECTED:      return F("Cipher suite rejected");
    case WIFI_DISCONNECT_REASON_BEACON_TIMEOUT:             return F("Beacon timeout");
    case WIFI_DISCONNECT_REASON_NO_AP_FOUND:                return F("No AP found");
    case WIFI_DISCONNECT_REASON_AUTH_FAIL:                  return F("Auth fail");
    case WIFI_DISCONNECT_REASON_ASSOC_FAIL:                 return F("Assoc fail");
    case WIFI_DISCONNECT_REASON_HANDSHAKE_TIMEOUT:          return F("Handshake timeout");
  }
  return F("Unknown");
}

}
