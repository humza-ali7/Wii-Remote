"""
File: dsu.py
Description: DSU Server Class File. Based on a FreePIE script written by
             u/NanoPi on Reddit. Reformatted and adjusted to work on
             python3.12. See:
             https://tinyurl.com/36zcfjmt
"""

import socket
import struct
import select
import time
import random
from enum import Enum
from binascii import crc32


class DSU_TYPES(Enum):
    """
    DSU Server/Client key values.

    Attributes:
        Enum
    """
    DSUC_VersionReq = 0x100000
    DSUS_VersionRsp = 0x100000
    DSUC_ListPorts = 0x100001
    DSUS_PortInfo = 0x100001
    DSUC_PadDataReq = 0x100002
    DSUS_PadDataRsp = 0x100002


class DSU_Server:
    """
    DSU Server class used for providing controller inputs to the
    Dolphin Wii Emulator.

    Attributes:
        None
    """

    def __init__(self, ip, port):
        """
        Initializes the DSU server.

        Params:
            ip (String): IP address of the DSU server.
            port (int): Port of the DSU server.
        """
        # Initialize the socket
        self.dsuSocket = socket.socket(socket.AF_INET, socket.SOCK_DGRAM)
        self.dsuSocket.setblocking(0)
        self.dsuSocket.bind((ip, port))
        self.serverId = random.randint(0, 0xFFFFFFFF)
        # Player 0 info (Basically just player 1 on Dolphin)
        self.p0list = {}
        self.p0count = 0
        self.protocolVersion = 1001
        # DSU Server Button Inputs
        self.buttons1 = 0
        self.buttons2 = 0
        # Additional button inputs, used for the nunchuck inputs
        self.extraButtons = 0
        # Accelerometer and Gyroscope data
        self.accelData = None
        self.gyroData = None
        # Nunchuck Joystick input data
        self.joystickData = None

    def communicateWithDsuClient(self):
        """ Communicate with the DSU Client (in this case Dolphin). """
        readDescriptors, _, _ = select.select([self.dsuSocket], [], [], 0.0)
        for rd in readDescriptors:
            try:
                data, address = rd.recvfrom(1024)
            except:
                continue
            
            # Receive data from DSU client
            _, self.protocolVersion, msgLen, crc, _ = struct.unpack_from(
                '<4s2HiI', data)
            if (msgLen + 16) != len(data):
                # Expected messaged length and data length do not match
                continue

            # Receive the message type from the client
            msgtype, = struct.unpack_from('<1I', data, 16)
            if msgtype == DSU_TYPES.DSUC_VersionReq.value:
                # Client requesting protocol version from server
                msg = struct.pack(
                    "<IH", DSU_TYPES.DSUS_VersionRsp.value, self.protocolVersion)
                header = struct.pack(
                    "<4s2HiI", "DSUS", self.protocolVersion, len(msg), 0, self.serverId)
                rc = crc32(header + msg)
                header2 = struct.pack(
                    "<4s2HiI", "DSUS", self.protocolVersion, len(msg), rc, self.serverId)
                # Transmit data to client
                self.dsuSocket.sendto(header2+msg, address)

            if msgtype == DSU_TYPES.DSUC_ListPorts.value:
                # Client requesting controller port info value
                wpl = []
                wanted, = struct.unpack_from('<1I', data, 20)
                if (wanted > 0) and (len(data) > 24):
                    # Player 1
                    wpn, = struct.unpack_from('<1B', data, 24)
                    wpl += [wpn]
                if (wanted > 1) and (len(data) > 25):
                    # Player 2
                    wpn, = struct.unpack_from('<1B', data, 25)
                    wpl += [wpn]
                if (wanted > 2) and (len(data) > 26):
                    # Player 3
                    wpn, = struct.unpack_from('<1B', data, 26)
                    wpl += [wpn]
                if (wanted > 3) and (len(data) > 27):
                    # Player 4
                    wpn, = struct.unpack_from('<1B', data, 27)
                    wpl += [wpn]
                msg = struct.pack("<I", DSU_TYPES.DSUS_PortInfo.value)
                # Controller data the server will provide
                # (only providing data for player 1)
                enabled = [True, False, False, False]
                for p in wpl:
                    if enabled[p]:
                        # Update port info for enabled controller 
                        state = 2  # 0=disconnected, 1=reserved, 2=connected
                        model = 2  # 0=none, 1=DS3, 2=DS4
                        connection = 2  # 0=none, 1=usb, 2=bt
                        battery = 5  # 0=none, 1=dying, 2=low, 3=medium, 4=high, 5=full, 0xEE=charging, 0xEF=charged
                        active = 1  # 0=no, 1=yes
                        portinfo = struct.pack("<4B6s2B", p, state, model, connection, "FPIE0{}".format(
                            p).encode('utf-8'), battery, active)
                        header = struct.pack("<4s2HII", b"DSUS", self.protocolVersion, len(
                            msg+portinfo), 0, self.serverId)
                        rc = crc32(header + msg + portinfo)
                        header2 = struct.pack("<4s2HII", b"DSUS", self.protocolVersion, len(
                            msg+portinfo), rc, self.serverId)
                        # Transmit data to client
                        self.dsuSocket.sendto(header2+msg+portinfo, address)

            if msgtype == DSU_TYPES.DSUC_PadDataReq.value:
                regflags, slotnum, macaddr = struct.unpack_from(
                    '<2B6s', data, 20)
                timestamp = time.time()
                if True and (regflags == 0 or (regflags & 1 != 0 and slotnum == 0) or (regflags & 2 != 0 and macaddr == "FPIE00")):
                    self.p0list[address] = timestamp

    def transmitInputData(self):
        """ Transmit input data from Server to a DSU client. """
        # Increment p0count
        self.p0count += 1
        for ip, port in self.p0list:
            address = (ip, port)

            # Button input values recorded in buttons1 (Wii Remote)
            Left = self.buttons1 & 0x80
            Down = self.buttons1 & 0x40
            Right = self.buttons1 & 0x20
            Up = self.buttons1 & 0x10
            # Button input values recorded in buttons2 (Wii Remote)
            Square = self.buttons2 & 0x80
            Cross = self.buttons2 & 0x40
            Circle = self.buttons2 & 0x20
            Triangle = self.buttons2 & 0x10
            R1 = self.buttons2 & 0x8
            L1 = self.buttons2 & 0x4

            # Button input values recorded in extraButtons (Nunchuck)
            home = 1 if self.extraButtons & 0x01 else 0
            padclick = 1 if self.extraButtons & 0x02 else 0

            # Update joystick input data from Nunchuck
            if self.joystickData != None:
                leftx = self.joystickData[0]
                lefty = self.joystickData[1]
            else:
                leftx = 0
                lefty = 0
            rightx = 0
            righty = 0

            # Pressure values for buttons pressed. 255 meaning button is
            # fully pressed and 0 meaning the button is not pressed.
            left_P = 255 if Left else 0
            down_P = 255 if Down else 0
            right_P = 255 if Right else 0
            up_P = 255 if Up else 0
            square_P = 255 if Square else 0
            cross_P = 255 if Cross else 0
            circle_P = 255 if Circle else 0
            triangle_P = 255 if Triangle else 0
            r1_P = 255 if R1 else 0
            l1_P = 255 if L1 else 0

            # Misc values we don't need
            r2 = 0  # R pressure
            l2 = 0  # L pressure
            pad1touch = 0  # 0=not touched, 1=first active touch
            pad1id = 0  # global touch counter
            pad1x = 0  # 0-1919
            pad1y = 0  # 0-941
            pad2touch = 0  # 0=not touched, 1=second active touch
            pad2id = 0  # global touch counter
            pad2x = 0  # 0-1919
            pad2y = 0  # 0-941

            # Motion timestamp value
            motiontimestamp = int(time.time() * 1000000)

            if self.accelData != None:
                # Update accelorometer data, in units of m/s^2. Dolphin
                # will automatically convert to units of g. Invert these
                # values to properly emulate moving in different directions,
                # (i.e. moving the remote left will move a character left, etc.)
                ax = (-1 * self.accelData[0])
                # Swapping the ay and az axes, since the Wii Remote expects the
                # y-axis to be pointing upwards when the remote is laying flat.
                # Swap these back if the accelerometer is pointing upwards
                # when the remote is flat.
                ay = (self.accelData[2])
                az = (-1 * self.accelData[1])
            else:
                ax = 0
                ay = 0
                az = 0

            if self.gyroData != None:
                # Gyroscope data (only provided from the Wii Remote)
                roll = self.gyroData[0]
                pitch = self.gyroData[1]
                yaw = self.gyroData[2]
            else:
                roll = 0
                pitch = 0
                yaw = 0

            msg = struct.pack("<I", DSU_TYPES.DSUS_PadDataRsp.value)
            state = 2 & 0xFF  # 0=disconnected, 1=reserved, 2=connected
            model = 2 & 0xFF  # 0=none, 1=DS3, 2=DS4
            connection = 2  # 0=none, 1=usb, 2=bt
            # 0=none, 1=dying, 2=low, 3=medium, 4=high, 5=full, 0xEE=charging, 0xEF=charged
            battery = 5 & 0xFF
            active = 1 & 0xFF  # 0=no, 1=yes
            port = 0 & 0xFF
            portinfo = struct.pack("<4B6s2BI", port, state, model, connection, "FPIE00".encode(
                'utf-8'), battery, active, int(self.p0count) & 0xFFFFFFFF)
            portdata = struct.pack("<22B2H2B2HQ6f", self.buttons1, self.buttons2, home, padclick, leftx, lefty, rightx, righty, left_P, down_P, right_P, up_P, square_P, cross_P,
                                   circle_P, triangle_P, r1_P, l1_P, r2, l2, pad1touch, pad1id, pad1x, pad1y, pad2touch, pad2id, pad2x, pad2y, motiontimestamp, ax, ay, az, pitch, yaw, roll)
            header = struct.pack("<4s2HiI", "DSUS".encode(
                'utf-8'), self.protocolVersion, len(msg+portinfo+portdata), 0, self.serverId)
            rc = crc32(header + msg + portinfo + portdata)
            header2 = struct.pack("<4s2HII", "DSUS".encode(
                'utf-8'), self.protocolVersion, len(msg+portinfo+portdata), rc, self.serverId)
            # Transmit controller input data
            self.dsuSocket.sendto(header2 + msg + portinfo + portdata, address)

    def update_inputs(self):
        """ 
        Communicate with the client and transmit input data. Should be run in
        the background as a thread.
        """
        while True:
            self.communicateWithDsuClient()
            self.transmitInputData()
