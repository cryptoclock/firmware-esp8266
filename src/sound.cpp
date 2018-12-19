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

#include "sound.hpp"
#include <NonBlockingRtttl.h>

Sound::Sound(int output_pin)
  : m_pin(output_pin), m_current_music("")
{
  pinMode(m_pin, OUTPUT);
}

void Sound::playMusicRTTTL(const String &music)
{
  if (rtttl::isPlaying())
    rtttl::stop();

  m_current_music = music;

  // if the name section is missing, add it ([name]:[defaults]:[tones])
  if (m_current_music.indexOf(":") == m_current_music.lastIndexOf(":"))
    m_current_music = "cc:" + m_current_music;

  rtttl::begin(m_pin, m_current_music.c_str());
}

void Sound::tick()
{
  if (!rtttl::done())
    rtttl::play();
}
