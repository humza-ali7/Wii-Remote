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
    pCharacteristic = pService->createCharacteristic(
                        CHARACTERISTIC_UUID,
                        BLECharacteristic::PROPERTY_NOTIFY
                    );                   

    // Create a BLE Characteristic
    pCharacteristic2 = pService->createCharacteristic(
                        CHARACTERISTIC_UUID2,
                        BLECharacteristic::PROPERTY_NOTIFY
                    );                   

    // Create a BLE Descriptor

    pDescr = new BLEDescriptor((uint16_t)0x2901);
    pDescr->setValue("Wii Remote Inputs.");
    pCharacteristic->addDescriptor(pDescr);

    pBLE2902 = new BLE2902();
    pBLE2902->setNotifications(true);
    pCharacteristic->addDescriptor(pBLE2902);

    pBLE29021 = new BLE2902();
    pBLE2902->setNotifications(true);
    pCharacteristic->addDescriptor(pBLE29021);

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
        pCharacteristic->setValue(buttonInputs);
        pCharacteristic->notify();
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