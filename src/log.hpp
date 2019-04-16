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

#undef CCLOG_COLORS

#ifdef CCLOG_COLORS
#define CCLOG_D_FORMAT "\e[37mD"
#define CCLOG_I_FORMAT "\e[32;1mI"
#define CCLOG_W_FORMAT "\e[33;1mW"
#define CCLOG_E_FORMAT "\e[31;1mE"
#else
#define CCLOG_D_FORMAT "D"
#define CCLOG_I_FORMAT "I"
#define CCLOG_W_FORMAT "W"
#define CCLOG_E_FORMAT "E"
#endif

#define CCTIME (millis() / 1000.0)
#define CCLOG(_level_, _fmt_, ...) Serial.printf_P(PSTR(_level_ " (%.3fs) %s: " _fmt_ "\n"), CCTIME, LOGTAG, ##__VA_ARGS__)

#define CCLOGD(_fmt_, ...) CCLOG(CCLOG_D_FORMAT,_fmt_, ##__VA_ARGS__)
#define CCLOGI(_fmt_, ...) CCLOG(CCLOG_I_FORMAT,_fmt_, ##__VA_ARGS__)
#define CCLOGW(_fmt_, ...) CCLOG(CCLOG_W_FORMAT,_fmt_, ##__VA_ARGS__)
#define CCLOGE(_fmt_, ...) CCLOG(CCLOG_E_FORMAT,_fmt_, ##__VA_ARGS__)
