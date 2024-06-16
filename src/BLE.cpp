/**
 * @file BLE.cpp
 * @brief BLE Source File
 * @author Humza Ali
 */

#include "Arduino.h"
#include "include/BLE.h"

// Indicates whether there is a device that is currently paired
// with the ESP32 device
static bool deviceConnected = false;

class MyServerCallbacks : public BLEServerCallbacks {
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
    pService = pServer->createService(SERVICE_UUID);

#if SERIAL_OUTPUT_LOGGING
    Serial.println("BLE server and service has been initialized.");
#endif
}

status_t BLE::createCharacteristic(const char* characteristicUuid,
                                    BLECharacteristic*& pCharacteristic,
                                    BLE2902*& pNotifier)
{
    // Null check
    if (pService == nullptr) {
        #if DEBUG
        Serial.println("pService is NULL in BLE::createCharacteristic().");
        #endif
        return STATUS_NULL_POINTER;
    }
    // Create a characteristic to notify with
    pCharacteristic = pService->createCharacteristic (
                                    characteristicUuid,
                                    BLECharacteristic::PROPERTY_NOTIFY
                                );
    // Initialize the notifier
    pNotifier = new BLE2902();
    pNotifier->setNotifications(true);
    pCharacteristic->addDescriptor(pNotifier);

#if SERIAL_OUTPUT_LOGGING
    Serial.print("Success initializing Characteristic with UUID:");
    Serial.println(characteristicUuid);
#endif
    return STATUS_COMPLETE;
}

status_t BLE::startAdvertising(void)
{
    if (pService == nullptr) {
        #if DEBUG
        Serial.println("pService is NULL in BLE::startAdvertising().");
        #endif
        return STATUS_NULL_POINTER;
    }
#if SERIAL_OUTPUT_LOGGING
    Serial.println("Preparing to Advertise...");
#endif
    // Start the service
    pService->start();
    // Initialize the advertiser
    BLEAdvertising *pAdvertising = BLEDevice::getAdvertising();
    pAdvertising->addServiceUUID(SERVICE_UUID);
    pAdvertising->setScanResponse(false);
    pAdvertising->setMinPreferred(0x0);
    // Start advertising now that everything is initialized
    BLEDevice::startAdvertising();
#if SERIAL_OUTPUT_LOGGING
    Serial.println("Started Adverstising.");
#endif

    return STATUS_COMPLETE;
}

void BLE::notifyCharacterisitic(BLECharacteristic* pCharacteristic)
{
    // Null check
    if (pCharacteristic == nullptr) {
        #if DEBUG
        Serial.println("pCharacteristic is NULL in BLE::notifyCharacterisitic().");
        delay(100);
        #endif
        return;
    }
    // Notify the characteristic value of pCharacteristic
    // if there is a device connected
    if (deviceConnected) {
        pCharacteristic->notify();
        delay(10);
    }
#if SERIAL_OUTPUT_LOGGING
    else {
        Serial.println("No device currently connected.");
        delay(500);
    }
#endif
    return;
}