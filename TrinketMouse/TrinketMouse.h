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
		Trinket_Mouse();
	
		// Starts the USB driver, causes re-enumeration.
		// Typically, you'd call this in setup.
		void begin(); 

		// Reports that mouse has been moved, and/or scroll wheel has
		// been turned, and/or button(s) are being pressed. Must be
		// called every 10 ms, even if the mouse hasn't moved, or
		// otherwise been touched.
		//
	        // x specifies how much the cursor has moved to the right
		// (negative values indicate left movement). Similarly,
		// y specify how much cursor has moved down (negative
		// values indicate up movement). Units???
		//
		// wheel specifies how much to scroll the scroll wheel.
		// Positive values indicate that the scroll bar should go
		// up (equivalently, the page moves down). Units???
		//
		// buttonMask indicates which buttons are being pressed.
		// Its value is MOUSEBTN_LEFT_MASK or similarly named
		// values, which are defined above. Can bitwise or of
		// MOUSEBTN_*_MASK be passed?
		void move(signed char x, signed char y, signed char wheel, uint8_t buttonMask);

		// Returns 0 if not connected to host (PC).
		char isConnected();
};

// an instance that the user can use
extern Trinket_Mouse TrinketMouse;

#endif
