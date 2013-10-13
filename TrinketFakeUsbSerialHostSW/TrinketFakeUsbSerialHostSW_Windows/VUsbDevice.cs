/*
TrinketFakeUsbSerialHostSW, by Frank Zhao

Copyright (c) 2013 Adafruit Industries
All rights reserved.

TrinketFakeUsbSerialHostSW is free software: you can redistribute it and/or modify
it under the terms of the GNU Lesser General Public License as
published by the Free Software Foundation, either version 3 of
the License, or (at your option) any later version.

TrinketFakeUsbSerialHostSW is distributed in the hope that it will be useful,
but WITHOUT ANY WARRANTY; without even the implied warranty of
MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
GNU Lesser General Public License for more details.

You should have received a copy of the GNU Lesser General Public
License along with TrinketFakeUsbSerialHostSW. If not, see
<http://www.gnu.org/licenses/>.
*/
using System;
using LibUsbDotNet;
using LibUsbDotNet.DeviceNotify;
using LibUsbDotNet.Main;

namespace TrinketFakeUsbSerialHostSW
{
    /// <summary>
    /// VUsbDevice represents a single USB device
    /// </summary>
    public class VUsbDevice
    {
        /// <summary>
        /// MyUsbDevice provides access to the UsbDevice object (from LibUsbDotNet)
        /// </summary>
        public UsbDevice MyUsbDevice
        {
            get;
            private set;
        }

        private UsbDeviceFinder MyUsbFinder;
        private IDeviceNotifier UsbDeviceNotifier;

        /// <summary>
        /// OnConnect is fired when the device connects to the computer, it is NOT fired when you call the TryConnect method even if it succeeds
        /// </summary>
        public event EventHandler OnConnect;
        
        /// <summary>
        /// OnDisconnect is fired when the current device is disconnected, it is NOT fired when you call the ForceClose method
        /// </summary>
        public event EventHandler OnDisconnect;

        /// <summary>
        /// OnOtherDeviceNotifyEvent is fired when a USB event occures that is not related to the current device, or is not a OnConnect or OnDisconnect event
        /// </summary>
        public event EventHandler<DeviceNotifyEventArgs> OnOtherDeviceNotifyEvent;

        /// <summary>
        /// Checks if the device is connected, it will call ForceClose if IsOpen or IsAlive is false and then return false
        /// </summary>
        public bool IsConnected
        {
            get
            {
                if (MyUsbDevice == null)
                    return false;

                if (MyUsbDevice.IsOpen == false || MyUsbDevice.UsbRegistryInfo.IsAlive == false)
                {
                    ForceClose();
                    return false;
                }

                return true;
            }
        }

        /// <summary>
        /// Creates an instance of a USB device using a VID PID pair
        /// </summary>
        /// <param name="vid">VID</param>
        /// <param name="pid">PID</param>
        public VUsbDevice(int vid, int pid)
        {
            MyUsbFinder = new UsbDeviceFinder(vid, pid);
            UsbDeviceNotifier = DeviceNotifier.OpenDeviceNotifier();
            UsbDeviceNotifier.OnDeviceNotify += new EventHandler<DeviceNotifyEventArgs>(UsbDeviceNotifier_OnDeviceNotify);
            UsbDeviceNotifier.Enabled = true;
        }

        /// <summary>
        /// TryConnect attempt to connect a USB device, will not fire the OnConnect event if successful. Returns true if successfully connected.
        /// </summary>
        /// <returns>Returns true if successfully connected</returns>
        public virtual bool TryConnect()
        {
            MyUsbDevice = UsbDevice.OpenUsbDevice(MyUsbFinder);

            return IsConnected;
        }

        /// <summary>
        /// ForceClose calls the Close method on the device, and then sets the current device to null
        /// </summary>
        public void ForceClose()
        {
            try
            {
                MyUsbDevice.Close();
            }
            catch
            {
            }
            finally
            {
                MyUsbDevice = null;
            }
        }

        /// <summary>
        /// Matches the device info during connect/disconnect and fires events accordingly, or passes on the event if it's not handled
        /// </summary>
        /// <param name="sender"></param>
        /// <param name="e"></param>
        private void UsbDeviceNotifier_OnDeviceNotify(object sender, DeviceNotifyEventArgs e)
        {
            if (e.EventType == EventType.DeviceArrival)
            {
                if (e.Device.IdVendor == MyUsbFinder.Vid && e.Device.IdProduct == MyUsbFinder.Pid)
                {
                    MyUsbDevice = UsbDevice.OpenUsbDevice(MyUsbFinder);

                    if (IsConnected)
                    {
                        if (OnConnect != null)
                            OnConnect(this, e);
                    }

                    return;
                }
            }
            else if (e.EventType == EventType.DeviceRemoveComplete)
            {
                if (MyUsbDevice != null)
                {
                    if (MyUsbDevice.UsbRegistryInfo.Vid == e.Device.IdVendor &&
                        MyUsbDevice.UsbRegistryInfo.Pid == e.Device.IdProduct)// &&
                        //MyUsbDevice.UsbRegistryInfo.SymbolicName.ToLowerInvariant().Contains(e.Device.SymbolicName.FullName.ToLowerInvariant()))
                    {
                        ForceClose();

                        if (OnDisconnect != null)
                            OnDisconnect(this, e);

                        return;
                    }
                }
            }

            if (OnOtherDeviceNotifyEvent != null)
                OnOtherDeviceNotifyEvent(this, e);
        }
    }
}
