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

#ifndef _TRINKETMOUSE_H_
#define _TRINKETMOUSE_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

// use these masks with the "move" function
#define MOUSEBTN_LEFT_MASK		0x01
#define MOUSEBTN_RIGHT_MASK		0x02
#define MOUSEBTN_MIDDLE_MASK	0x04

class Trinket_Mouse
{
	private:
	public:
		Trinket_Mouse(); // empty constructor, ignore me
		void begin(); // starts the USB driver, causes re-enumeration
		void move(signed char x, signed char y, signed char wheel, uint8_t buttonMask); // makes a mouse movement, must be called at least once every 10ms, even if no movement
		char isConnected(); // checks if USB is connected, 0 if not connected
};

// an instance that the user can use
extern Trinket_Mouse TrinketMouse;

#endif