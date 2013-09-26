/*
TrinketKeyboard example
For Trinket by Adafruit Industries
*/

#include <TrinketKeyboard.h>

#define PIN_BUTTON_CAPITAL_A 0
#define PIN_BUTTON_STRING    2

void setup()
{
  // button pins as inputs
  pinMode(PIN_BUTTON_CAPITAL_A, INPUT);
  pinMode(PIN_BUTTON_STRING, INPUT);

  // setting input pins to high means turning on internal pull-up resistors
  digitalWrite(PIN_BUTTON_CAPITAL_A, HIGH);
  digitalWrite(PIN_BUTTON_STRING, HIGH);
  // remember, the buttons are active-low, they read LOW when they are not pressed

  // start USB stuff
  TrinketKeyboard.begin();
}

void loop()
{
  TrinketKeyboard.poll();
  // the poll function must be called at least once every 10 ms
  // or cause a keystroke
  // if it is not, then the computer may think that the device
  // has stopped working, and give errors

  if (digitalRead(PIN_BUTTON_CAPITAL_A) == LOW)
  {
    TrinketKeyboard.pressKey(KEYCODE_MOD_LEFT_SHIFT, KEYCODE_A);
    // this should type a capital A
    TrinketKeyboard.pressKey(0, 0);
    // this releases the key
  }

  if (digitalRead(PIN_BUTTON_STRING) == LOW)
  {
    // type out a string using the Print class
    TrinketKeyboard.print("Hello World!");
  }
}