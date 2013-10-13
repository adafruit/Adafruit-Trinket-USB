/*
Example for TrinketFakeUsbSerial, see library files for details
*/

#include "TrinketFakeUsbSerial.h"

void setup()
{
  TFUSerial.begin();
}

void loop()
{
  TFUSerial.task(); // this should be called at least once every 10 ms

  // simple echo
  if (TFUSerial.available()) {
    TFUSerial.write((uint8_t)TFUSerial.read());
  }
}