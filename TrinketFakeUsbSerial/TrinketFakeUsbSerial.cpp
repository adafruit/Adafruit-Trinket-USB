/*
TrinketFakeUsbSerial allows the user to use a serial terminal to communicate with the Trinket directly via USB. This depends on several pieces of host-side software to be installed first.

Usage is similar to HardwareSerial, except that task() should be called at least once every 10 ms, or call a function that uses task(), such as available(), or send something. Basically, let the computer know that the Trinket is alive at least once every 10 ms.

See tutorial at http://learn.adafruit.com/trinket-fake-usb-serial

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

#include "TrinketFakeUsbSerial.h"
#include "TrinketFakeUsbSerialC.h"
#include <stdint.h>
#include "usbdrv/usbdrv.h"

TrinketFakeUsbSerial::TrinketFakeUsbSerial(void)
{
	// empty constructor
}

void TrinketFakeUsbSerial::begin(unsigned long x)
{
	// baud rate ignored
	begin();
}

void TrinketFakeUsbSerial::begin(void)
{
	usbBegin();
}

void TrinketFakeUsbSerial::task(void)
{
	usbPollWrapper();
}

size_t TrinketFakeUsbSerial::write(uint8_t d)
{
	tfus_tx(d);
	return 1;
}

int TrinketFakeUsbSerial::available(void)
{
	task();
	return (TFUS_RX_BUFFER_SIZE + tfus_rx_buffer_write - tfus_rx_buffer_read) % TFUS_RX_BUFFER_SIZE;
}

void TrinketFakeUsbSerial::flush(void)
{
	task();
	tfus_rx_buffer_write = tfus_rx_buffer_write = 0;
}

int TrinketFakeUsbSerial::peek(void)
{
	usbPollWrapper();
	if ((TFUS_RX_BUFFER_SIZE + tfus_rx_buffer_write - tfus_rx_buffer_read) % TFUS_RX_BUFFER_SIZE <= 0) {
		return -1;
	}
	return tfus_rx_buffer[tfus_rx_buffer_read];
}

int TrinketFakeUsbSerial::read(void)
{
	usbPollWrapper();
	if ((TFUS_RX_BUFFER_SIZE + tfus_rx_buffer_write - tfus_rx_buffer_read) % TFUS_RX_BUFFER_SIZE <= 0) {
		return -1;
	}
	int r = tfus_rx_buffer[tfus_rx_buffer_read];
	tfus_rx_buffer_read = (tfus_rx_buffer_read + 1) % TFUS_RX_BUFFER_SIZE;
	return r;
}

void TrinketFakeUsbSerial::end(void)
{
	// drive both USB pins low to disconnect
	usbDeviceDisconnect();
	tfus_rx_buffer_write = tfus_rx_buffer_write = 0;
}

TrinketFakeUsbSerial::operator bool() {
	usbPollWrapper();
	return true;
}

TrinketFakeUsbSerial TFUSerial; // instance that the user can use