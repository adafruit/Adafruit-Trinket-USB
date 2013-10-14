TrinketFakeUsbSerial, by Frank Zhao

A part of Adafruit's Trinket/Gemma USB Arduino Library

See tutorial at http://learn.adafruit.com/trinket-fake-usb-serial

TrinketFakeUsbSerial allows the user to use a serial terminal to communicate with the Trinket directly via USB. This depends on several pieces of host-side software to be installed first (see TrinketFakeUsbSerialHostSW).

Usage is similar to HardwareSerial, except that task() should be called at least once every 10 ms, or call a function that uses task(), such as available(), or send something. Basically, let the computer know that the Trinket is alive at least once every 10 ms.

TrinketFakeUsbSerial uses V-USB (http://www.obdev.at/products/vusb/) to implement USB with a bitbang technique. See the folder "usbdrv" for V-USB files.

Adafruit's Trinket and Gemma can do almost anything that V-USB can do, including USB mouse, keyboards, joysticks, gamepads, MIDI devices, and much more (including custom devices that do not fit any existing category).

Copyright (c) 2013 Adafruit Industries
All rights reserved.

TrinketFakeUsbSerial is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation, either version 3 of
the License, or (at your option) any later version.

TrinketFakeUsbSerial is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with TrinketFakeUsbSerial. If not, see
<http://www.gnu.org/licenses/>.