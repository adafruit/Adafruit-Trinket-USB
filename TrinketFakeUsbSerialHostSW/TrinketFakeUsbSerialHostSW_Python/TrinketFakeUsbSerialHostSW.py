#!/usr/bin/env python

# TrinketFakeUsbSerialHostSW, Python edition, by Frank Zhao
# see http://learn.adafruit.com/trinket-fake-usb-serial

# Copyright (c) 2013 Adafruit Industries
# All rights reserved.
# 
# TrinketFakeUsbSerialHostSW is free software: you can redistribute it and/or modify
# it under the terms of the GNU Lesser General Public License as
# published by the Free Software Foundation, either version 3 of
# the License, or (at your option) any later version.
# 
# TrinketFakeUsbSerialHostSW is distributed in the hope that it will be useful,
# but WITHOUT ANY WARRANTY; without even the implied warranty of
# MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
# GNU Lesser General Public License for more details.
# 
# You should have received a copy of the GNU Lesser General Public
# License along with TrinketFakeUsbSerialHostSW. If not, see
# <http://www.gnu.org/licenses/>.

# requires the installation of:
# PyUSB, http://sourceforge.net/apps/trac/pyusb/
# pySerial, http://pyserial.sourceforge.net/

import sys, time, getopt, select
import usb.core, usb.util
import serial

def main(argv):
    port = ''
    verbose = False
    silent = False
    try:
       opts, args = getopt.getopt(argv,"hvsp:",["port="])
    except getopt.GetoptError:
        print 'Error: GetoptError'
        printHelp()
        sys.exit(2)
    for opt, arg in opts:
        if opt == '-h':
            printHelp()
            sys.exit()
        elif opt in ("-p", "--port"):
            port = arg
        elif opt in ("-v", "--verbose"):
            verbose = True
        elif opt in ("-s", "--silent"):
            silent = True
    if verbose:
        print 'TrinketFakeUsbSerialHostSW Python Edition'
        print 'port is ', port

    ser = serial.Serial(port)
    if verbose and port != ser.portstr:
        print 'port actually is ', ser.portstr

    trinketHandle = False

    lastMsg = ''
    thisMsg = ''

    while True:
        time.sleep(0.01) # don't hog all CPU

        wasDisconnected = (trinketHandle == False)
        trinketHandle = findTrinket()

        thisMsg = 'Trinket Connected'
        if verbose and wasDisconnected and trinketHandle and thisMsg != lastMsg:
            print thisMsg
            lastMsg = thisMsg

        endpoint = 0x81
        if trinketHandle != False:
            endpoint = trinketHandle[0][(0,0)][0] # the first endpoint should be the only endpoint, it should be an interrupt-in endpoint

        while trinketHandle != False:
            time.sleep(0.01) # don't hog all CPU

            avail = ser.inWaiting()
            if avail > 0:
                data = ser.read(min(avail, 8))
                try:
                    ret = trinketHandle.ctrl_transfer(0x43, 0x01, 0x00, 0x00, map(ord, data))
                    thisMsg = 'Sent to Trinket'
                    if verbose and thisMsg != lastMsg:
                        print thisMsg
                        lastMsg = thisMsg
                except Exception as ex:
                    if silent == False:
                        print 'USB send error: ', ex
                    trinketHandle = False # pretend it's disconnected to retry later
                    time.sleep(0.1) # don't hog all CPU
                    break

            #end of if avail > 0

            try:
                data = trinketHandle.read(endpoint.bEndpointAddress, endpoint.wMaxPacketSize)
                readCnt = len(data)
                if readCnt > 0:
                    thisMsg = 'Read from Trinket'
                    if verbose and thisMsg != lastMsg:
                        print thisMsg
                        lastMsg = thisMsg
                    try:
                        ser.write(bytearray(data))
                    except Exception as serEx:
                        if silent == False:
                            print 'Serial Write Error: ', serEx
                        sys.exit() # this is an unrecoverable error

            except Exception as ex:
                exStr = str(ex).lower()
                if 'timeout' not in exStr: # ignore timeout errors, but all other errors signifies that the device disconnected
                    if silent == False:
                        print 'USB read error: ', ex
                    time.sleep(0.1) # don't hog all CPU
                    trinketHandle = False # disconnect to reacquire
                    break
                time.sleep(0.01) # slight delay

        # end of while
        thisMsg = 'Trinket Disconnected'
        if verbose and thisMsg != lastMsg:
            print thisMsg
            lastMsg = thisMsg

def findTrinket():
    device = usb.core.find(idVendor = 0x1781, idProduct = 0x1111)
    if device == None or device == False or device == 0 :
        time.sleep(0.1) # don't hog all CPU
        return False
    device.set_configuration()
    return device

def printHelp():
    print 'http://learn.adafruit.com/trinket-fake-usb-serial'
    print ''
    print 'command line options:'
    print '-h'
    print '    will print the help'
    print ''
    print '-p <port name>'
    print '    will set the serial port, this is required'
    print ''
    print '-v'
    print '    will enable verbose output'
    print ''
    print '-s'
    print '    will disable most error output (some errors I can\'t catch, sorry)'
    print ''
    print 'cause a keyboard interrupt to quit (press CTRL-C or CMD-C, or something like that)'
    print 'or kill it in some other way'

if __name__ == "__main__":
    main(sys.argv[1:])
