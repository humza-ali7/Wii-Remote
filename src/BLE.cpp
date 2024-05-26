#include "include/BLE.h"
#include "Arduino.h"

bool deviceConnected = false;
bool oldDeviceConnected = false;
uint32_t value = 0;
uint32_t value2 = 0;

class MyServerCallbacks: public BLEServerCallbacks {
    void onConnect(BLEServer* pServer) {
      deviceConnected = true;
    };

    void onDisconnect(BLEServer* pServer) {
      deviceConnected = false;
    }
};

void BLE::initBle()
{
    // Create the BLE Device
    BLEDevice::init(deviceName);

    // Create the BLE Server
    pServer = BLEDevice::createServer();
    pServer->setCallbacks(new MyServerCallbacks());

    // Create the BLE Service
    BLEService *pService = pServer->createService(SERVICE_UUID);

    // Create a BLE Characteristic
    pButtonInputCharacteristic = pService->createCharacteristic (
                                    BUTTON_INPUT_CHARACTERISTIC_UUID,
                                    BLECharacteristic::PROPERTY_NOTIFY
                                 );                   

    // Create a BLE Characteristic
    pSensorInputCharacteristic = pService->createCharacteristic (
                                    SENSOR_INPUT_CHARACTERISTIC_UUID,
                                    BLECharacteristic::PROPERTY_NOTIFY
                                 );                   

    // Create a BLE Descriptor

    pDescr = new BLEDescriptor((uint16_t)0x2901);
    pDescr->setValue("Wii Remote Button Inputs.");
    pButtonInputCharacteristic->addDescriptor(pDescr);

    pButtonInputNotifier = new BLE2902();
    pButtonInputNotifier->setNotifications(true);
    pButtonInputCharacteristic->addDescriptor(pButtonInputNotifier);


    pSensorInputNotifier = new BLE2902();
    pSensorInputNotifier->setNotifications(true);
    pSensorInputCharacteristic->addDescriptor(pSensorInputNotifier);

    // Start the service
    pService->start();

    // Start advertising
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(false);
    pAdvertising->setMinPreferred(0x0);  // set value to 0x00 to not advertise this parameter
    BLEDevice::startAdvertising();
    Serial.println("Waiting a client connection to notify...");
}

void BLE::updateButtonInputValue(uint32_t buttonInputs) 
{
    // notify changed value
    if (deviceConnected) {
        pButtonInputCharacteristic->setValue(buttonInputs);
        pButtonInputCharacteristic->notify();
        delay(25);
    }
    // disconnecting
    if (!deviceConnected && oldDeviceConnected) {
        delay(500); // give the bluetooth stack the chance to get things ready
        pServer->startAdvertising(); // restart advertising
        Serial.println("start advertising");
        oldDeviceConnected = deviceConnected;
    }
    // connecting
    if (deviceConnected && !oldDeviceConnected) {
        // do stuff here on connecting
        oldDeviceConnected = deviceConnected;
    }
}

void BLE::updateSensorInputValue(AccelData accelData, GyroData gyroData)
{
    if (deviceConnected) {
        // Consolidate both sensor data into one characteristic
        // to minimize characteristic overhead
        uint8_t accelGyroDataBytes[ACCEL_GYRO_DATA_SIZE];

        // Load the acceleration data
        memcpy(accelGyroDataBytes, &accelData, ACCEL_DATA_STRUCT_SIZE);
        // Load the gyroscope data
        memcpy(accelGyroDataBytes + ACCEL_DATA_STRUCT_SIZE, &gyroData, GYRO_DATA_STRUCT_SIZE);
        // Transmit the data
        pSensorInputCharacteristic->setValue(accelGyroDataBytes, ACCEL_GYRO_DATA_SIZE);
        pSensorInputCharacteristic->notify();
        delay(25);
    }
    // disconnecting
    if (!deviceConnected && oldDeviceConnected) {
        delay(500); // give the bluetooth stack the chance to get things ready
        pServer->startAdvertising(); // restart advertising
        Serial.println("start advertising");
        oldDeviceConnected = deviceConnected;
    }
    // connecting
    if (deviceConnected && !oldDeviceConnected) {
        // do stuff here on connecting
        oldDeviceConnected = deviceConnected;
    }
}