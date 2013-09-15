/*
This example simply starts a keyboard and types out the system time
*/

#include <TrinketKeyboard.h>

void setup()
{
  TrinketKeyboard.begin();
}

void loop()
{
  TrinketKeyboard.println(millis(), 10);
}