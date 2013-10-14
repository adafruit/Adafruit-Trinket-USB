/*
Example for TrinketFakeUsbSerial, see library files for details

See tutorial at http://learn.adafruit.com/trinket-fake-usb-serial
*/

#include "TrinketFakeUsbSerial.h"

void setup()
{
  TFUSerial.begin();
}

void loop()
{
  TFUSerial.task(); // this should be called at least once every 10 ms

  if (TFUSerial.available()) {
    char c = TFUSerial.read();
    if (c == 'a')
    {
      TFUSerial.println("hello world");
    }
    else if (c == 'b')
    {
      TFUSerial.println(millis(), DEC);
    }
  }
}