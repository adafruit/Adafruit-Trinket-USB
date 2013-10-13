TrinketFakeUsbSerial by Frank Zhao

A part of Adafruit's Trinket/Gemma USB Arduino Library

TrinketFakeUsbSerial allows the user to use a serial terminal to communicate with the Trinket directly via USB. This depends on several pieces of host-side software to be installed first.

TrinketFakeUsbSerialHostSW only works on Windows! It is a system tray application.

The host-side software is named TrinketFakeUsbSerialHostSW. It requires a 3rd party program called "com0com" to be installed, and then use com0com to setup a pair of COM ports.

For example, setup COM1 and COM2 using com0com, if you want to use Arduino's serial terminal, select COM1 inside Arduino IDE, and then select COM2 inside TrinketFakeUsbSerialHostSW.

Here's how data flows for the above example

Trinket <-> LibUsbDotNet <-> TrinketFakeUsbSerialHostSW <-> (COM2) com0com (COM1) <-> Arduino IDE's serial terminal

It works with any serial terminal program, such as Hyperterminal, Teraterm, Putty, RealTerm, etc

TrinketFakeUsbSerialHostSW also uses LibUsbDotNet, and provides compatible libusb drivers, see the "Driver" directory. If Windows asks for a driver, tell it to look inside there.

The executable files you are looking for are inside "TrinketFakeUsbSerialHostSW_Windows\bin\Release", all the DLLs in there are also required. Run the exe, which is a system tray app that runs in the background. Without it running in the background, there won't be any communication.

Feel free to put the exe inside your system startup. It remembers the last port number you used.

Copyright (c) 2013 Adafruit Industries
All rights reserved.

TrinketFakeUsbSerialHostSW is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation, either version 3 of
the License, or (at your option) any later version.

TrinketFakeUsbSerialHostSW is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with TrinketFakeUsbSerialHostSW. If not, see
<http://www.gnu.org/licenses/>.