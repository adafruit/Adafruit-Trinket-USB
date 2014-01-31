/*
This is the part of the TrinketFakeUsbSerial code that is usually written in C
I didn't want to mix it with the object oriented C++ code

This file implements a low speed USB device classified as a "vendor class" device, with one interrupt-in (device to host) endpoint. The control endpoint is used for host-to-device communication

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

#include "cmdline_defs.h"
#include "TrinketFakeUsbSerialC.h"
#include "usbconfig.h"
#include "usbdrv/usbdrv.h"
#include <avr/pgmspace.h>
#include <avr/interrupt.h>
#include <avr/power.h>
#include <util/delay.h>
#include <stdint.h>

static uint8_t currentPosition, bytesRemaining;
uint8_t tfus_rx_buffer[TFUS_RX_BUFFER_SIZE];
uint8_t tfus_rx_buffer_read = 0;
uint8_t tfus_rx_buffer_write = 0;
//char usb_hasCommed = 0;

usbMsgLen_t usbFunctionSetup(uchar d[8])
{
	usbRequest_t *rq = (void *)d;    // cast to structured data for parsing
	if (rq->bRequest == 0x01) {
		currentPosition = 0;                 // initialize position index
		bytesRemaining = rq->wLength.word;   // store the amount of data requested
		return USB_NO_MSG;                  // tell driver to use usbFunctionRead()
	}
	// TODO: maybe add more features?
	return 0;                               // ignore all unknown requests
}

uint8_t usbFunctionWrite(uint8_t* data, uint8_t len)
{
	uint8_t i;
	if(len > bytesRemaining) {              // if this is the last incomplete chunk
		len = bytesRemaining;               // limit to the amount we can store
	}
	bytesRemaining -= len;
	for(i = 0; i < len; i++) {
		// push into fifo if room is available
		uint8_t next = (tfus_rx_buffer_write + 1) % TFUS_RX_BUFFER_SIZE;
		if (next != tfus_rx_buffer_read)
		{
			tfus_rx_buffer[tfus_rx_buffer_write] = data[i];
			tfus_rx_buffer_write = next;
		}
	}
	return bytesRemaining == 0;             // return 1 if we have all data
}

void usbPollWrapper()
{
	usbPoll();
}

void tfus_tx(uint8_t d)
{
	static uint8_t to_send;
	to_send = d;
	// perform usb background tasks until the report can be sent, then send it
	while (1)
	{
		usbPoll(); // this needs to be called at least once every 10 ms
		if (usbInterruptIsReady())
		{
			usbSetInterrupt((uint8_t*)&to_send, 1); // send
			break;

			// see http://vusb.wikidot.com/driver-api
		}
	}
}

void usbBegin()
{
	cli();

	// run at full speed, because Trinket defaults to 8MHz for low voltage compatibility reasons
	clock_prescale_set(clock_div_1);

	// fake a disconnect to force the computer to re-enumerate
	PORTB &= ~(_BV(USB_CFG_DMINUS_BIT) | _BV(USB_CFG_DPLUS_BIT));
	usbDeviceDisconnect();
	_delay_ms(250);
	usbDeviceConnect();

	// start the USB driver
	usbInit();
	sei();
}

#if defined(__AVR_ATtiny85__) || defined(__AVR_ATtiny45__) || defined(__AVR_ATtiny25__)
/* ------------------------------------------------------------------------- */
/* ------------------------ Oscillator Calibration ------------------------- */
/* ------------------------------------------------------------------------- */
// section copied from EasyLogger
/* Calibrate the RC oscillator to 8.25 MHz. The core clock of 16.5 MHz is
 * derived from the 66 MHz peripheral clock by dividing. Our timing reference
 * is the Start Of Frame signal (a single SE0 bit) available immediately after
 * a USB RESET. We first do a binary search for the OSCCAL value and then
 * optimize this value with a neighboorhod search.
 * This algorithm may also be used to calibrate the RC oscillator directly to
 * 12 MHz (no PLL involved, can therefore be used on almost ALL AVRs), but this
 * is wide outside the spec for the OSCCAL value and the required precision for
 * the 12 MHz clock! Use the RC oscillator calibrated to 12 MHz for
 * experimental purposes only!
 */
void calibrateOscillator(void)
{
    uchar       step = 128;
    uchar       trialValue = 0, optimumValue;
    int         x, optimumDev, targetValue = (unsigned)(1499 * (double)F_CPU / 10.5e6 + 0.5);

    /* do a binary search: */
    do{
        OSCCAL = trialValue + step;
        x = usbMeasureFrameLength();    /* proportional to current real frequency */
        if(x < targetValue)             /* frequency still too low */
            trialValue += step;
        step >>= 1;
    }while(step > 0);
    /* We have a precision of +/- 1 for optimum OSCCAL here */
    /* now do a neighborhood search for optimum value */
    optimumValue = trialValue;
    optimumDev = x; /* this is certainly far away from optimum */
    for(OSCCAL = trialValue - 1; OSCCAL <= trialValue + 1; OSCCAL++){
        x = usbMeasureFrameLength() - targetValue;
        if(x < 0)
            x = -x;
        if(x < optimumDev){
            optimumDev = x;
            optimumValue = OSCCAL;
        }
    }
    OSCCAL = optimumValue;
}
/*
Note: This calibration algorithm may try OSCCAL values of up to 192 even if
the optimum value is far below 192. It may therefore exceed the allowed clock
frequency of the CPU in low voltage designs!
You may replace this search algorithm with any other algorithm you like if
you have additional constraints such as a maximum CPU clock.
For version 5.x RC oscillators (those with a split range of 2x128 steps, e.g.
ATTiny25, ATTiny45, ATTiny85), it may be useful to search for the optimum in
both regions.
*/
#endif