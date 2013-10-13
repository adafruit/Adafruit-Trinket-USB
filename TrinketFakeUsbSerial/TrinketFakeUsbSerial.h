/*
TrinketFakeUsbSerial header file, see code file for details

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
*/

#ifndef TRINKETFAKEUSBSERIAL_H_
#define TRINKETFAKEUSBSERIAL_H_

#include "Stream.h"

class TrinketFakeUsbSerial : public Stream // inheritance, so use like HardwareSerial
{
	private:
	public:
		TrinketFakeUsbSerial();
		void begin(), begin(unsigned long x);
		void task();
		void end();
		virtual int available(void);
		virtual int peek(void);
		virtual int read(void);
		virtual void flush(void);
		virtual size_t write(uint8_t);
		inline size_t write(unsigned long n) { return write((uint8_t)n); }
		inline size_t write(long n) { return write((uint8_t)n); }
		inline size_t write(unsigned int n) { return write((uint8_t)n); }
		inline size_t write(int n) { return write((uint8_t)n); }
		using Print::write; // pull in write(str) and write(buf, size) from Print
		operator bool();
};

extern TrinketFakeUsbSerial TFUSerial; // instance that user can use

#endif