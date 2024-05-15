/**
 * @file BLE.h
 * @brief Bluetooth Low Energy (BLE) header file.
 * @author Humza Ali
 */

#pragma once

#include <BLEDevice.h>
#include <BLEUtils.h>
#include <BLEServer.h>
#include <BLE2902.h>
#include "generic_types.h"
#include <string>

/** MACROS */
// BLE Service and Characteristic UUID values
#define SERVICE_UUID         "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define CHARACTERISTIC_UUID  "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define CHARACTERISTIC_UUID2 "eb854de2-f0b3-48bf-90ca-1f2a85ef29c8"

class BLE 
{
public:

    BLE(std::string name) { deviceName = name; };

    void initBle();

    void updateButtonInputValue(uint32_t buttonInputs);

private:
    std::string deviceName;
    BLEServer* pServer = nullptr;
    BLECharacteristic* pCharacteristic = nullptr;
    BLECharacteristic* pCharacteristic2 = nullptr;
    BLEDescriptor *pDescr = nullptr;
    BLE2902 *pBLE2902 = nullptr;
    BLE2902 *pBLE29021 = nullptr;
};