/*
TrinketKeyboard prank example
For Trinket by Adafruit Industries
*/

#include <TrinketKeyboard.h>

void setup()
{
  // start USB stuff
  TrinketKeyboard.begin();
  while (TrinketKeyboard.isConnected() == 0); // wait until connection
  randomSeed(millis()); // seed the RNG
  // the HOST takes some time before isConnected can return true
  // this amount of time is hard to predict, it depends on how busy the OS is
  // we use this unknown time as the seed
}

void loop()
{
  unsigned long secs_to_wait = random(60, 120); // generate a random amount of time
  unsigned long time_stamp = millis();
  while (millis() < (time_stamp + (secs_to_wait * 1000))) // wait the random amount of time
  {
    TrinketKeyboard.poll();
    // the poll function must be called at least once every 10 ms
    // or cause a keystroke
    // if it is not, then the computer may think that the device
    // has stopped working, and give errors
  }
  TrinketKeyboard.typeChar((char)random(33, 122)); // type out a random character (valid ASCII)
}