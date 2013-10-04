/*
 * The purpose of this file is to provide the compiler any definitions that would usually go in the command line options, since Arduino IDE does not allow such additions
 
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

#ifndef _CMDLINE_DEFS_H_
#define _CMDLINE_DEFS_H_

#include <avr/io.h>

#if defined(__AVR_ATtiny85__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny25__)
// the oscillator calibration will cause the F_CPU to change
#undef F_CPU
#define F_CPU 16500000UL

#endif

#endif