/*
This is the part of the TrinketHidCombo code that is exposed to the user
See the header file for more comments on the functions

Copyright (c) 2013 Adafruit Industries
All rights reserved.

TrinketHidCombo is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation, either version 3 of
the License, or (at your option) any later version.

TrinketHidCombo is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with TrinketHidCombo. If not, see
<http://www.gnu.org/licenses/>.
*/

#include "cmdline_defs.h"
#include "TrinketHidCombo.h"
#include "TrinketHidComboC.h"

#include <stdint.h>
#include <avr/interrupt.h>
#include <util/delay.h>

// create an instance that the user can use
Trinket_Hid_Combo TrinketHidCombo;

// empty constructor
Trinket_Hid_Combo::Trinket_Hid_Combo()
{
	// nothing to do
}

// starts the USB driver, causes re-enumeration
void Trinket_Hid_Combo::begin()
{
	usbBegin();
}

// this must be called at least once every 10ms
void Trinket_Hid_Combo::poll()
{
	usbPollWrapper();
}

// makes a mouse movement
void Trinket_Hid_Combo::mouseMove(signed char x, signed char y, uint8_t buttonMask)
{
	signed char * signed_ptr = (signed char *)report_buffer; // this converts signed to unsigned

	// format the report structure
	signed_ptr[2] = x;
	signed_ptr[3] = y;
	report_buffer[1] = buttonMask;
	report_buffer[0] = REPID_MOUSE;

	usbReportSend(REPSIZE_MOUSE);
}

void Trinket_Hid_Combo::pressKey(uint8_t modifiers, uint8_t keycode1)
{
	pressKey(modifiers, keycode1, 0, 0, 0, 0);
}

void Trinket_Hid_Combo::pressKey(uint8_t modifiers, uint8_t keycode1, uint8_t keycode2)
{
	pressKey(modifiers, keycode1, keycode2, 0, 0, 0);
}

void Trinket_Hid_Combo::pressKey(uint8_t modifiers, uint8_t keycode1, uint8_t keycode2, uint8_t keycode3)
{
	pressKey(modifiers, keycode1, keycode2, keycode3, 0, 0);
}

void Trinket_Hid_Combo::pressKey(uint8_t modifiers, uint8_t keycode1, uint8_t keycode2, uint8_t keycode3, uint8_t keycode4)
{
	pressKey(modifiers, keycode1, keycode2, keycode3, keycode4, 0);
}

void Trinket_Hid_Combo::pressKey(uint8_t modifiers, uint8_t keycode1, uint8_t keycode2, uint8_t keycode3, uint8_t keycode4, uint8_t keycode5)
{
	// construct the report, follow the standard format as described
	// this format is compatible with "boot protocol"
	report_buffer[1] = modifiers;
	report_buffer[2] = 0; // reserved
	report_buffer[3] = keycode1;
	report_buffer[4] = keycode2;
	report_buffer[5] = keycode3;
	report_buffer[6] = keycode4;
	report_buffer[7] = keycode5;
	report_buffer[0] = REPID_KEYBOARD;
	usbReportSend(REPSIZE_KEYBOARD);
}

// presses a list of keys, do not exceed 5 keys
void Trinket_Hid_Combo::pressKeys(uint8_t modifiers, uint8_t* keycodes, uint8_t sz)
{
	report_buffer[0] = REPID_KEYBOARD;
	report_buffer[1] = modifiers;
	report_buffer[2] = 0; // reserved
	uint8_t i;
	for (i = 0; i < sz; i++)
	{
		report_buffer[3 + i] = keycodes[i];
	}
	usbReportSend(REPSIZE_KEYBOARD);
}

void Trinket_Hid_Combo::typeChar(uint8_t ascii)
{
	uint8_t modifier, keycode;
	ASCII_to_keycode(ascii, getLEDstate(), &modifier, &keycode);
	pressKey(modifier, keycode);
	pressKey(0, 0); // immediately release the key after
}

size_t Trinket_Hid_Combo::write(uint8_t ascii)
{
	typeChar(ascii);
	return 1;
}

void Trinket_Hid_Combo::pressMultimediaKey(uint8_t key)
{
	report_buffer[0] = REPID_MMKEY;
	report_buffer[1] = key;
	report_buffer[2] = 0;
	usbReportSend(REPSIZE_MMKEY);
	// immediate release
	report_buffer[0] = REPID_MMKEY;
	report_buffer[1] = 0;
	report_buffer[2] = 0;
	usbReportSend(REPSIZE_MMKEY);
}

void Trinket_Hid_Combo::pressSystemCtrlKey(uint8_t key)
{
	report_buffer[0] = REPID_SYSCTRLKEY;
	report_buffer[1] = key;
	usbReportSend(REPSIZE_SYSCTRLKEY);
	// immediate release
	report_buffer[0] = REPID_SYSCTRLKEY;
	report_buffer[1] = key;
	usbReportSend(REPSIZE_SYSCTRLKEY);
}

void ASCII_to_keycode(uint8_t ascii, uint8_t ledState, uint8_t* modifier, uint8_t* keycode)
{
	*keycode = 0x00;
	*modifier = 0x00;
	
	// see scancode.doc appendix C
	
	if (ascii >= 'A' && ascii <= 'Z')
	{
		*keycode = 4 + ascii - 'A'; // set letter
		if (bit_is_set(ledState, 1)) // if caps is on
		{
			*modifier = 0x00; // no shift
		}
		else
		{
			*modifier = _BV(1); // hold shift // hold shift
		}
	}
	else if (ascii >= 'a' && ascii <= 'z')
	{
		*keycode = 4 + ascii - 'a'; // set letter
		if (bit_is_set(ledState, 1)) // if caps is on
		{
			*modifier = _BV(1); // hold shift // hold shift
		}
		else
		{
			*modifier = 0x00; // no shift
		}
	}
	else if (ascii >= '0' && ascii <= '9')
	{
		*modifier = 0x00;
		if (ascii == '0')
		{
			*keycode = 0x27;
		}
		else
		{
			*keycode = 30 + ascii - '1'; 
		}
	}
	else
	{
		switch (ascii) // convert ascii to keycode according to documentation
		{
			case '!':
				*modifier = _BV(1); // hold shift
				*keycode = 29 + 1;
				break;
			case '@':
				*modifier = _BV(1); // hold shift
				*keycode = 29 + 2;
				break;
			case '#':
				*modifier = _BV(1); // hold shift
				*keycode = 29 + 3;
				break;
			case '$':
				*modifier = _BV(1); // hold shift
				*keycode = 29 + 4;
				break;
			case '%':
				*modifier = _BV(1); // hold shift
				*keycode = 29 + 5;
				break;
			case '^':
				*modifier = _BV(1); // hold shift
				*keycode = 29 + 6;
				break;
			case '&':
				*modifier = _BV(1); // hold shift
				*keycode = 29 + 7;
				break;
			case '*':
				*modifier = _BV(1); // hold shift
				*keycode = 29 + 8;
				break;
			case '(':
				*modifier = _BV(1); // hold shift
				*keycode = 29 + 9;
				break;
			case ')':
				*modifier = _BV(1); // hold shift
				*keycode = 0x27;
				break;
			case '~':
				*modifier = _BV(1); // hold shift
				// fall through
			case '`':
				*keycode = 0x35;
				break;
			case '_':
				*modifier = _BV(1); // hold shift
				// fall through
			case '-':
				*keycode = 0x2D;
				break;
			case '+':
				*modifier = _BV(1); // hold shift
				// fall through
			case '=':
				*keycode = 0x2E;
				break;
			case '{':
				*modifier = _BV(1); // hold shift
				// fall through
			case '[':
				*keycode = 0x2F;
				break;
			case '}':
				*modifier = _BV(1); // hold shift
				// fall through
			case ']':
				*keycode = 0x30;
				break;
			case '|':
				*modifier = _BV(1); // hold shift
				// fall through
			case '\\':
				*keycode = 0x31;
				break;
			case ':':
				*modifier = _BV(1); // hold shift
				// fall through
			case ';':
				*keycode = 0x33;
				break;
			case '"':
				*modifier = _BV(1); // hold shift
				// fall through
			case '\'':
				*keycode = 0x34;
				break;
			case '<':
				*modifier = _BV(1); // hold shift
				// fall through
			case ',':
				*keycode = 0x36;
				break;
			case '>':
				*modifier = _BV(1); // hold shift
				// fall through
			case '.':
				*keycode = 0x37;
				break;
			case '?':
				*modifier = _BV(1); // hold shift
				// fall through
			case '/':
				*keycode = 0x38;
				break;
			case ' ':
				*keycode = 0x2C;
				break;
			case '\t':
				*keycode = 0x2B;
				break;
			case '\n':
				*keycode = 0x28;
				break;
		}
	}
}

uint8_t Trinket_Hid_Combo::getLEDstate()
{
	return led_state;
}

// checks if USB is connected, 0 if not connected
char Trinket_Hid_Combo::isConnected()
{
	return usb_hasCommed;
}