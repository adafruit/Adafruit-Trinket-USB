/*
This is the part of the TrinketMouse code that is exposed to the user

Copyright (c) 2013 Adafruit Industries
All rights reserved.

TrinketMouse is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation, either version 3 of
the License, or (at your option) any later version.

TrinketMouse is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with TrinketMouse. If not, see
<http://www.gnu.org/licenses/>.
*/

#include "cmdline_defs.h"
#include "TrinketMouse.h"
#include "TrinketMouseC.h"

#include <stdint.h>
#include <avr/interrupt.h>
#include <util/delay.h>

// create an instance that the user can use
Trinket_Mouse TrinketMouse;

// empty constructor
Trinket_Mouse::Trinket_Mouse()
{
	// nothing to do
}

// starts the USB driver, causes re-enumeration
void Trinket_Mouse::begin()
{
	usbBegin();
}

// makes a mouse movement, must be called at least once every 10ms, even if no movement
void Trinket_Mouse::move(signed char x, signed char y, signed char wheel, uint8_t buttonMask)
{
	signed char * signed_ptr = (signed char *)report_buffer; // this converts signed to unsigned

	// format the report structure
	signed_ptr[1] = x;
	signed_ptr[2] = y;
	signed_ptr[3] = wheel;
	report_buffer[0] = buttonMask;

	usbReportSend();
}

// checks if USB is connected, 0 if not connected
char Trinket_Mouse::isConnected()
{
	return usb_hasCommed;
}