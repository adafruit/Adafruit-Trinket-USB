/*
This is the part of the TrinketMidi code that is exposed to the user
See the header file for more comments on the functions

Copyright (c) 2013 Adafruit Industries

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

#include "cmdline_defs.h"
#include "TrinketMidi.h"
#include "TrinketMidiC.h"

#include <stdint.h>
#include <avr/interrupt.h>
#include <util/delay.h>

// create an instance that the user can use
Trinket_Midi TrinketMidi;

Trinket_Midi::Trinket_Midi()
{
	channel = 10;
}

// starts the USB driver, causes re-enumeration
void Trinket_Midi::begin()
{
	usbBegin();
}

// this must be called at least once every 10ms
void Trinket_Midi::poll()
{
	usbPollWrapper();
}

void Trinket_Midi::noteOn(uint8_t key, uint8_t velocity)
{
	midi_msg[0] = 0x09;
	midi_msg[1] = 0x90 | channel;
	midi_msg[2] = key;
	midi_msg[3] = velocity;
	usbReportSend();
}

void Trinket_Midi::noteOff(uint8_t key, uint8_t velocity)
{
	midi_msg[0] = 0x08;
	midi_msg[1] = 0x80 | channel;
	midi_msg[2] = key;
	midi_msg[3] = velocity;
	usbReportSend();
}

void Trinket_Midi::setChannel(uint8_t new_channel)
{
    if (new_channel > 15)
        new_channel = 15;
        
    channel = new_channel;
}

// checks if USB is connected, 0 if not connected
char Trinket_Midi::isConnected()
{
	return usb_hasCommed;
}