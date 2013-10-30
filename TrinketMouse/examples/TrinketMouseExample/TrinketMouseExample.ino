/*
This example simply starts a mouse and randomly moves it around, while clicking random buttons
*/

#include <TrinketMouse.h>

void setup()
{
  TrinketMouse.begin();
}

void loop()
{
  TrinketMouse.move(random(), random(), random(), random() & 0x07);
}