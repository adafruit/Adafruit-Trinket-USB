/*
Copyright (c) 2013 Adafruit Industries

TrinketMidi is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation, either version 3 of
the License, or (at your option) any later version.

TrinketMidi is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with TrinketMidi. If not, see
<http://www.gnu.org/licenses/>.
*/

#ifndef _TRINKETMIDI_H_
#define _TRINKETMIDI_H_

#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

#ifdef __cplusplus
}
#endif

class Trinket_Midi
{
	private:
        uint8_t channel;

	public:
		Trinket_Midi(); // empty constructor, ignore me
		void begin(); // starts the USB driver, causes re-enumeration
		void poll(); // this (or "press" something) must be called at least once every 10ms
		char isConnected(); // checks if USB is connected, 0 if not connected

        void setChannel(uint8_t channel);
		void noteOn(uint8_t key, uint8_t velocity);
        void noteOff(uint8_t key, uint8_t velocity);
};

// an instance that the user can use
extern Trinket_Midi TrinketMidi;

#endif