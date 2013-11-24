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
using System.Diagnostics;
using System.Drawing;
using System.Collections;
using System.Collections.Generic;
using System.Collections.Specialized;
using System.Collections.ObjectModel;
using System.Threading;
using System.Reflection;
using System.Runtime.CompilerServices;
using System.Runtime.Versioning;
using System.Windows.Forms;
using System.IO.Ports;
using LibUsbDotNet;
using LibUsbDotNet.Main;
using LibUsbDotNet.DeviceNotify;

namespace TrinketFakeUsbSerialHostSW
{
	public sealed class TrinketFakeUsbSerialHostSW
	{
		private NotifyIcon notifyIcon;
		private ContextMenu notificationMenu;
        private SerialPort port;
        private string lastPortName;
        private VUsbDevice trinket;
        private UsbEndpointReader trinketReader;
        private IniFile ini;
        private MenuItem portMenu;
        private long bytesProcessed = 0;
		
		#region Initialize icon and menu
        public TrinketFakeUsbSerialHostSW()
        {
            notifyIcon = new NotifyIcon();
            notificationMenu = new ContextMenu(InitializeMenu());

            // notifyIcon.DoubleClick += IconDoubleClick;
            System.ComponentModel.ComponentResourceManager resources = new System.ComponentModel.ComponentResourceManager(typeof(TrinketFakeUsbSerialHostSW));
            notifyIcon.Icon = (Icon)resources.GetObject("mainicon");
            notifyIcon.ContextMenu = notificationMenu;

            trinket = new VUsbDevice(0x1781, 0x1111); // VID and PID set inside usbconfig.h, which is the Trinket's VID and PID while using the TrinketFakeUsbSerial library
            if (trinket.TryConnect())
            {
                try
                {
                    if (trinket.MyUsbDevice.IsOpen == false)
                    {
                        if (trinket.MyUsbDevice.Open() == false)
                        {
                            throw new Exception("Can't Open USB Device");
                        }
                    }

                    trinketReader = trinket.MyUsbDevice.OpenEndpointReader(LibUsbDotNet.Main.ReadEndpointID.Ep01);
                    trinketReader.DataReceived += new EventHandler<LibUsbDotNet.Main.EndpointDataEventArgs>(trinket_DataReceived);
                    trinketReader.DataReceivedEnabled = true;
                }
                catch (Exception ex)
                {
                    MessageBox.Show("Trinket Connecting Error: " + ex.Message);
                }
            }
            //trinket.OnOtherDeviceNotifyEvent += new EventHandler<DeviceNotifyEventArgs>(trinket_OnOtherDeviceNotifyEvent);
            trinket.OnConnect += new EventHandler(trinket_OnConnect);
            trinket.OnDisconnect += new EventHandler(trinket_OnDisconnect);

            // the last used serial port is remembered inside an INI file
            ini = new IniFile(Application.ExecutablePath + ".settings.ini");
            lastPortName = ini.Read("SerialPort", "LastPort");
            if (lastPortName != null)
            {
                lastPortName = lastPortName.Trim();
                if (lastPortName.Length == 0)
                {
                    lastPortName = null;
                    // just in case the ini file is malformed
                }
            }

            if (lastPortName != null)
            {
                try
                {
                    // attempt to open the serial port immediately if the previous port exists
                    port = new SerialPort(lastPortName);
                    port.Open();
                    port.ReadTimeout = 200;
                    port.ReceivedBytesThreshold = 1;
                    port.DataReceived += new SerialDataReceivedEventHandler(port_DataReceived);
                }
                catch (Exception ex)
                {
                    //MessageBox.Show("Serial Port \"" + lastPortName + "\" Open Error: " + ex.Message);
                }
            }

            if (lastPortName == null || port == null || port.IsOpen == false)
            {
                MessageBox.Show("Please Select a COM Port!");
            }

            Thread t = new Thread(new ThreadStart(SerPortCheckThread));
            t.Start();
        }
		
		private MenuItem[] InitializeMenu()
		{
            portMenu = new MenuItem("Pick COM#:");
            portMenu.MenuItems.Add(""); // dummy
            portMenu.MeasureItem += new MeasureItemEventHandler(pickCom_OnRepopulate);
            portMenu.Popup += new EventHandler(pickCom_OnRepopulate);

            MenuItem[] menu = new MenuItem[] {
                portMenu,
				new MenuItem("About", menuAboutClick),
				new MenuItem("Exit", menuExitClick)
			};
			return menu;
		}
        #endregion

        // port_DataReceived triggers really slowly if we use the event
        // so we implement a polling thread that runs faster
        private void SerPortCheckThread()
        {
            while (true)
            {
                try
                {
                    if (port != null && port.IsOpen)
                    {
                        if (port.BytesToRead > 0)
                        {
                            port_DataReceived(null, null);
                        }
                    }
                    Thread.Sleep(0); // prevents this thread from hogging all the CPU time
                }
                catch
                {
                }
            }
        }

        #region GUI Events
        private void pickCom_OnRepopulate(object sender, EventArgs e)
        {
            // fill the list of ports
            portMenu.MenuItems.Clear();
            string[] portNames = SerialPort.GetPortNames();
            for (int i = 0; i < portNames.Length; i++)
            {
                MenuItem nm = portMenu.MenuItems.Add(portNames[i]);
                if (lastPortName != null && portNames[i] == lastPortName)
                {
                    // this looks like the previously selected port, indicate as such
                    nm.RadioCheck = true;
                    nm.Checked = true;
                }
                nm.Click += new EventHandler(serPortName_Click);
            }
        }

        void serPortName_Click(object sender, EventArgs e)
        {
            MenuItem m = sender as MenuItem;
            string portName = m.Text;

            if (port != null && port.IsOpen && port.PortName != portName)
            {
                // a different port is selected, so close the previous port before opening a new one
                try
                {
                    port.Close();
                }
                catch { }
                port = null;
            }

            // if the previous port is the same as the newly selected port, then port would not be null
            if (port == null)
            {
                try
                {
                    port = new SerialPort(portName);
                    port.Open();
                    port.ReadTimeout = 200;
                    port.ReceivedBytesThreshold = 1;
                    port.DataReceived += new SerialDataReceivedEventHandler(port_DataReceived);

                    lastPortName = portName;
                    ini.Write("SerialPort", "LastPort", lastPortName); // remember it inside the ini file so this exe can start up and automatically open it next time
                    MenuItem p = notifyIcon.ContextMenu.MenuItems[1];
                    for (int i = 0; i < p.MenuItems.Count; i++)
                    {
                        // all other ones are deselected
                        p.MenuItems[i].RadioCheck = false;
                        p.MenuItems[i].Checked = false;
                    }
                    // this one is selected
                    m.RadioCheck = true;
                    m.Checked = true;
                }
                catch (Exception ex)
                {
                    MessageBox.Show("Serial Port Change Error: " + ex.Message);
                }
            }
        }

        private void menuAboutClick(object sender, EventArgs e)
        {
            Assembly assembly = Assembly.GetExecutingAssembly();
            AssemblyDescriptionAttribute adAttr = (AssemblyDescriptionAttribute)Attribute.GetCustomAttribute(assembly, typeof(AssemblyDescriptionAttribute));
            string disc = (adAttr == null) ? "" : adAttr.Description;

            string msg = assembly.FullName + Environment.NewLine +
                            disc + Environment.NewLine;

            if (port != null && port.IsOpen)
            {
                msg += "Serial Port: " + port.PortName + Environment.NewLine;
            }
            else
            {
                msg += "Serial Port Closed" + Environment.NewLine;
            }

            if (trinket.IsConnected)
            {
                msg += "Trinket Present" + Environment.NewLine;
            }
            else
            {
                msg += "Trinket Missing" + Environment.NewLine;
            }

            msg += "Bytes Passed: " + bytesProcessed.ToString() + Environment.NewLine;

            MessageBox.Show(msg);
        }

        private void menuExitClick(object sender, EventArgs e)
        {
            //Application.Exit();
            notifyIcon.Visible = false;
            Process p = Process.GetCurrentProcess();
            p.Kill();
        }

        private void IconDoubleClick(object sender, EventArgs e)
        {
            // MessageBox.Show("The icon was double clicked");
        }
        #endregion

        #region USB Events
        void trinket_OnDisconnect(object sender, EventArgs e)
        {
            notifyIcon.ShowBalloonTip(1000, "Trinket Fake USB Serial", "USB Device Disconnected", ToolTipIcon.Warning);
        }

        void trinket_OnConnect(object sender, EventArgs e)
        {
            notifyIcon.ShowBalloonTip(1000, "Trinket Fake USB Serial", "USB Device Connected", ToolTipIcon.Info);
            if (trinket.TryConnect())
            {
                try
                {
                    if (trinket.MyUsbDevice.IsOpen == false)
                    {
                        if (trinket.MyUsbDevice.Open() == false)
                        {
                            throw new Exception("Can't Open USB Device");
                        }
                    }

                    trinketReader = trinket.MyUsbDevice.OpenEndpointReader(LibUsbDotNet.Main.ReadEndpointID.Ep01);
                    trinketReader.DataReceived += new EventHandler<LibUsbDotNet.Main.EndpointDataEventArgs>(trinket_DataReceived);
                    trinketReader.DataReceivedEnabled = true;
                }
                catch (Exception ex)
                {
                    notifyIcon.ShowBalloonTip(1000, "Trinket Fake USB Serial", "Trinket Connecting Error: " + ex.Message, ToolTipIcon.Error);
                }
            }
        }

        void trinket_OnOtherDeviceNotifyEvent(object sender, DeviceNotifyEventArgs e)
        {
            //notifyIcon.ShowBalloonTip(1000, "Trinket Fake USB Serial", "USB Device Notification: " + Enum.GetName(typeof(LibUsbDotNet.DeviceNotify.EventType), e.EventType), ToolTipIcon.Info);
        }
        #endregion

        #region Data Events
        void trinket_DataReceived(object sender, EndpointDataEventArgs e)
        {
            try
            {
                // we got data but the port was never opened?
                if (port == null)
                {
                    if (lastPortName != null) // hmm, we have a name of the port, try opening it
                    {
                        port = new SerialPort(lastPortName);
                        port.Open();
                        port.ReadTimeout = 200;
                        port.ReceivedBytesThreshold = 1;
                        port.DataReceived += new SerialDataReceivedEventHandler(port_DataReceived);
                    }
                }
            }
            catch (Exception ex)
            {
                notifyIcon.ShowBalloonTip(1000, "Trinket Fake USB Serial", "Serial Port Opening Error: " + ex.Message, ToolTipIcon.Error);
            }

            if (port != null && port.IsOpen == true)
            {
                try
                {
                    // we have a port that is usable, send the data
                    port.Write(e.Buffer, 0, e.Count);
                    bytesProcessed += e.Count;
                }
                catch (Exception ex)
                {
                    notifyIcon.ShowBalloonTip(1000, "Trinket Fake USB Serial", "Serial Port Sending Error: " + ex.Message, ToolTipIcon.Error);
                }
            }
        }

        void port_DataReceived(object sender, SerialDataReceivedEventArgs e)
        {
            if (port != null && port.IsOpen)
            {
                try
                {
                    int br = port.BytesToRead;
                    byte[] data = new byte[br];
                    int i = port.Read(data, 0, br);

                    if (trinket.IsConnected == false)
                    {
                        trinket.TryConnect();
                    }

                    if (trinket.IsConnected == true)
                    {
                        try
                        {
                            for (int j = 0; j < i; )
                            {
                                // only send by chunks
                                short toSend = 256;
                                int remaining = i - j;
                                if (toSend > remaining)
                                {
                                    toSend = (short)remaining;
                                }

                                byte[] dataToSend = new byte[toSend];
                                for (int k = 0; k < toSend; k++, j++)
                                {
                                    dataToSend[k] = data[j];
                                }

                                int actuallySent;
                                UsbSetupPacket usp = new UsbSetupPacket((byte)((byte)UsbRequestType.TypeVendor | (byte)UsbRequestRecipient.RecipOther), 0x01, 0, 0, toSend);
                                // bRequest is 0x01, see firmware source code, which checks this

                                if (trinket.MyUsbDevice.IsOpen == false)
                                {
                                    if (trinket.MyUsbDevice.Open() == false)
                                    {
                                        throw new Exception("Can't Open USB Device");
                                    }
                                }

                                if (trinket.MyUsbDevice.ControlTransfer(ref usp, dataToSend, (int)toSend, out actuallySent))
                                {
                                    bytesProcessed += actuallySent;
                                }
                                else
                                {
                                    notifyIcon.ShowBalloonTip(1000, "Trinket Fake USB Serial", "Trinket Send Error", ToolTipIcon.Error);
                                }
                            }
                        }
                        catch (Exception ex)
                        {
                            notifyIcon.ShowBalloonTip(1000, "Trinket Fake USB Serial", "Trinket Send Error: " + ex.Message, ToolTipIcon.Error);
                        }
                    }
                    else
                    {
                        notifyIcon.ShowBalloonTip(1000, "Trinket Fake USB Serial", "Trinket Send Error: No Trinket Detected", ToolTipIcon.Error);
                    }
                }
                catch (Exception ex)
                {
                    notifyIcon.ShowBalloonTip(1000, "Trinket Fake USB Serial", "Serial Port Read Error: " + ex.Message, ToolTipIcon.Error);
                }
            }
        }
        #endregion

        #region Main - Program entry point
        /// <summary>Program entry point.</summary>
        /// <param name="args">Command Line Arguments</param>
        [STAThread]
        public static void Main(string[] args)
		{
            try
            {
                Application.EnableVisualStyles();
                Application.SetCompatibleTextRenderingDefault(false);

                bool isFirstInstance;
                // Please use a unique name for the mutex to prevent conflicts with other programs
                using (Mutex mtx = new Mutex(true, "TrinketFakeUsbSerialHostSW", out isFirstInstance))
                {
                    if (isFirstInstance)
                    {
                        TrinketFakeUsbSerialHostSW notificationIcon = new TrinketFakeUsbSerialHostSW();
                        notificationIcon.notifyIcon.Visible = true;
                        Application.Run();
                        notificationIcon.notifyIcon.Dispose();
                    }
                    else
                    {
                        // The application is already running
                        // TODO: Display message box or change focus to existing application instance
                    }
                } // releases the Mutex
            }
            catch (Exception ex)
            {
                MessageBox.Show("Trinket Fake USB Serial Host Software: Fatal Error: " + ex.Message);
            }
		}
		#endregion
		
        // not used but put here just in case we need it later
        // it allows for GUI elements to be modified from any thread
		private delegate void SetControlPropertyThreadSafeDelegate(Control control, string propertyName, object propertyValue);
		public static void SetControlPropertyThreadSafe(Control control, string propertyName, object propertyValue)
		{
			if (control.InvokeRequired)
			{
				control.Invoke(new SetControlPropertyThreadSafeDelegate(SetControlPropertyThreadSafe), new object[] { control, propertyName, propertyValue });
			}
			else
			{
				control.GetType().InvokeMember(propertyName, BindingFlags.SetProperty, null, control, new object[] { propertyValue });
			}
		}
	}
}
