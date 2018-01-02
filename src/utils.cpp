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

}
