"""
File: pymore_controller.py
Description: Controller file for handling Wii Remote and Nunchuck Inputs.
Author: Humza Ali
"""

import asyncio
import struct
from bleak import BleakScanner, BleakClient
from dsu import DSU_Server
import threading


class Wiimote(DSU_Server):
    """
    A Wii Remote class for storing inputs to be transmitted through
    a connected DSU Server.

    Attributes:
        DSU_Server
    """

    def __init__(self, ip, port):
        """ 
        Initializes the Wiimote and a DSU Server to send the Wiimote inputs.
        """
        super().__init__(ip, port)
        self.buttonInputLengthBytes = 2 
        self.sensorInputLengthBytes = 24

    def buttonInputs(self, inputs):
        """ 
        Updates the Wii Remote button inputs.

        Params:
            inputs (List): The Wii Remote button inputs.
        """
        # Grab the two bytes for each input
        self.buttons1 = inputs[0]
        self.buttons2 = inputs[1]

    def sensorInputs(self, accelData, gyroData):
        """ 
        Updates the Wii Remote sensor inputs.

        Params:
            accelData (Tuple): The Wii Remote accelorometer data.
            gyroData (Tuple): The Wii Remote gyroscope data
        """
        self.accelData = accelData
        self.gyroData = gyroData

    def wiimote_button_input_cb(self, sender, data):
        """ 
        Callback called by the BLE class to update the button inputs based on
        the received characteristic data.

        Params:
            sender(BleakGATTCharacteristicWinRT): Unused positional parameter
            data (Tuple): Received data from the characteristic, in bytes.
        """
        if len(data) < self.buttonInputLengthBytes:
            print("Wii Remote Button Input Underflow:")
            print(f"Expected number of bytes: {self.buttonInputLengthBytes}")
            print(f"Received number of bytes: {len(data)}")
            return
        wiiRemote.buttonInputs(data)

    def wiimote_sensor_input_cb(self, sender, data):
        """ 
        Callback called by the BLE class to update the Wii Remote accelorometer
        and gyroscope inputs based on the received characteristic data.

        Params:
            sender(BleakGATTCharacteristicWinRT): Unused positional parameter
            data (Tuple): Received data from the characteristic, in bytes.
        """
        if len(data) < self.sensorInputLengthBytes:
            print("Wii Remote Sensor Input Underflow:")
            print(f"Expected number of bytes: {self.sensorInputLengthBytes}")
            print(f"Received number of bytes: {len(data)}")
            return
        gyroDataStartIndex = int(len(data) / 2)
        accelData = struct.unpack('<3f', data[:gyroDataStartIndex])
        gyroData = struct.unpack('<3f', data[gyroDataStartIndex:])

        wiiRemote.sensorInputs(accelData, gyroData)


class Nunchuck(DSU_Server):
    """
    A Nunchuck class for storing inputs to be transmitted through
    a connected DSU Server.

    Attributes:
        DSU_Server
    """

    def __init__(self, ip, port):
        """ 
        Initializes the Nunchuck and a DSU Server to send the Wiimote inputs.

        Params:
            ip (String): IP address of the DSU server.
            port (int): Port of the DSU server.
        """
        # Initializes the Nunchuck DSU server
        super().__init__(ip, port)
        self.buttonJoystickInputLengthBytes = 3
        self.sensorInputLengthBytes = 12

    def buttonInputs(self, inputs):
        """ 
        Updates the Nunchuck button inputs.

        Params:
            inputs (List): The Nunchuck button inputs.
        """
        self.joystickData = (inputs[0], inputs[1])
        self.extraButtons = inputs[2]

    def accelDataInputs(self, accelData):
        """ 
        Updates the Nunchuck sensor inputs.

        Params:
            accelData (Tuple): The Wii Remote accelorometer data.
            gyroData (Tuple): The Wii Remote gyroscope data
        """
        self.accelData = accelData

    def nunchuck_button_joystick_input_cb(self, sender, data):
        """ 
        Callback called by the BLE class to update the button and joystick
        inputs based on the received characteristic data.

        Params:
            sender (BleakGATTCharacteristicWinRT): Unused positional parameter
            data (bytearray): Received data from the characteristic, in bytes.
        """
        if len(data) < self.buttonJoystickInputLengthBytes:
            print("Nunchuck Button and Joystick Input Underflow:")
            print(f"Expected number of bytes: {self.buttonJoystickInputLengthBytes}")
            print(f"Received number of bytes: {len(data)}")
            return
        nunchuck.buttonInputs(data)

    def nunchuck_sensor_input_cb(self, sender, data):
        """ 
        Callback called by the BLE class to update the nunchuck accelorometer
        inputs based on the received characteristic data.

        Params:
            sender(BleakGATTCharacteristicWinRT): Unused positional parameter
            data (Tuple): Received data from the characteristic, in bytes.
        """
        if len(data) < self.sensorInputLengthBytes:
            print("Nunchuck Sensor Input Underflow:")
            print(f"Expected number of bytes: {self.sensorInputLengthBytes}")
            print(f"Received number of bytes: {len(data)}")
            return
        accelData = struct.unpack('<3f', data)
        nunchuck.accelDataInputs(accelData)


# BLE Service UUID
SERVICE_UUID = '06a1ef1c-d8f5-4839-bf3a-cf1deed694d2'
# Wii Remote Input Characteristic UUIDs
WIIMOTE_BUTTON_INPUT_CHARACTERISTIC_UUID = '7e3092ce-5b65-44c7-afef-c7722ef964b3'
WIIMOTE_SENSOR_INPUT_CHARACTERISTIC_UUID = 'eb854de2-f0b3-48bf-90ca-1f2a85ef29c8'
# Nunchuck Input Characteristic UUID
NUNCHUCK_BUTTON_JOYSTICK_INPUT_CHARACTERISTIC_UUID = "3327921d-e3b3-43ff-b724-a706fae760d3"
NUNCHUCK_SENSOR_INPUT_CHARACTERISTIC_UUID = "be11ecb2-1c60-4411-9385-0436b247c5bb"


class BLE(object):
    """
    A BLE class object used to connect to BLE devices.

    Attributes:
        None
    """

    def __init__(self, bleDeviceName):
        """
        Initializes the BLE class object.

        Params:
            bleDeviceName (String): The name of the BLE device 
        """
        self.bleDeviceName = bleDeviceName
        self.bleDevice = None
        self.callbacks = {}

    def addCallback(self, characteristicUuid, callback):
        """
        Adds a callback that is called upon receiving notifications
        from a BLE characteristic.

        Params:
            characteristicUuid (String): UUID of the characteristic associated
                                         with the callback.
            callback (function): Callback associated with the characterisitic.
        """
        self.callbacks[characteristicUuid] = callback

    async def findDevice(self) -> bool:
        """
        Finds the BLE device based on the name of the device.

        Return:
            (bool): Indicates whether a device has been found that matches
                    the BLE device name.
        """
        availableBleDevices = await BleakScanner.discover()
        # Loop through detectable BLE devices
        for device in availableBleDevices:
            if device.name == self.bleDeviceName:
                # If there is a match in the available device names, update
                # the bleDevice field with device.
                print(f"Found {device.name}")
                self.bleDevice = device
                break

        if self.bleDevice != None:
            # Store the address of the bleDevice. This will be used to
            # receive notifications from the BLE server.
            address = self.bleDevice.address
            print(f"Device MAC address: {address}")
            return True
        # No device found, return false
        return False

    async def receiveData(self):
        async with BleakClient(self.bleDevice.address) as client:
            while True:
                for uuid in self.callbacks:
                    await client.start_notify(uuid, self.callbacks[uuid])
                await asyncio.sleep(10)


# Declare and initialize Wii Remote and Nunchuck
wiiRemote = Wiimote('127.0.0.1', 26760)
nunchuck = Nunchuck('127.0.0.2', 26760)
# Declare and initialize BLE object
ble = BLE("Wii Remote")

# Threads to be used to run the Wii Remote and Nunchuck DSU servers
wiiRemoteDsuThread = None
nunchuckDsuThread = None


def initControllers():
    """
    Initializes DSU threads and adds callbacks to handle received
    controller input values.
    """
    # Create and start the Wii Remote DSU thread. Run it in daemon mode
    # so that the server runs in the background and so the main program
    # does not depend on the server completing communication tasks.
    wiiRemoteDsuThread = threading.Thread(target=wiiRemote.update_inputs)
    wiiRemoteDsuThread.daemon = True
    wiiRemoteDsuThread.start()

    # Create and start the Nunchuck DSU thread. Like the Wii Remote
    # thread, run in daemon mode as a background task.
    nunchuckDsuThread = threading.Thread(target=nunchuck.update_inputs)
    nunchuckDsuThread.daemon = True
    nunchuckDsuThread.start()

    # Add callbacks used to handle notifications from the assigned
    # characteristic UUIDs
    ble.addCallback(WIIMOTE_BUTTON_INPUT_CHARACTERISTIC_UUID,
                    wiiRemote.wiimote_button_input_cb)
    ble.addCallback(WIIMOTE_SENSOR_INPUT_CHARACTERISTIC_UUID,
                    wiiRemote.wiimote_sensor_input_cb)
    ble.addCallback(NUNCHUCK_BUTTON_JOYSTICK_INPUT_CHARACTERISTIC_UUID,
                    nunchuck.nunchuck_button_joystick_input_cb)
    ble.addCallback(NUNCHUCK_SENSOR_INPUT_CHARACTERISTIC_UUID,
                    nunchuck.nunchuck_sensor_input_cb)


async def main():
    """ Main function handler. """
    initControllers()
    connectionStatus = await ble.findDevice()
    assert connectionStatus, "Failed to connect to the Wii Remote."
    await ble.receiveData()

asyncio.run(main())
