/*
This is the part of the TrinketFakeUsbSerial code that is usually written in C
I didn't want to mix it with the object oriented C++ code

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

#ifndef TRINKETFAKEUSBSERIALC_H_
#define TRINKETFAKEUSBSERIALC_H_

#ifdef __cplusplus
extern "C" {
#endif

#include <stdint.h>

#define TFUS_RX_BUFFER_SIZE 16

//extern char usb_hasCommed;
extern uint8_t tfus_rx_buffer[TFUS_RX_BUFFER_SIZE];
extern uint8_t tfus_rx_buffer_read;
extern uint8_t tfus_rx_buffer_write;

void usbBegin();
void usbPollWrapper();
void tfus_tx(uint8_t);

#ifdef __cplusplus
}
#endif

#endif