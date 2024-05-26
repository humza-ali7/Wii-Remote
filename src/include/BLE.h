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
#include "IMU_Sensor.h"
#include "generic_types.h"
#include <string>

/** MACROS */

// BLE Service and Characteristic UUID values
#define SERVICE_UUID                      "4fafc201-1fb5-459e-8fcc-c5c9c331914b"
#define BUTTON_INPUT_CHARACTERISTIC_UUID  "beb5483e-36e1-4688-b7f5-ea07361b26a8"
#define SENSOR_INPUT_CHARACTERISTIC_UUID  "eb854de2-f0b3-48bf-90ca-1f2a85ef29c8"

#define ACCEL_DATA_STRUCT_SIZE            (size_t)sizeof(AccelData)
#define GYRO_DATA_STRUCT_SIZE             (size_t)sizeof(GyroData)
#define ACCEL_GYRO_DATA_SIZE              (size_t)(ACCEL_DATA_STRUCT_SIZE + GYRO_DATA_STRUCT_SIZE)

class BLE 
{
public:

    BLE(std::string name) { deviceName = name; };

    void initBle();

    void updateButtonInputValue(uint32_t buttonInputs);

    void updateSensorInputValue(AccelData accelData, GyroData gyroData);

private:
    std::string deviceName;
    BLEServer* pServer = nullptr;
    BLECharacteristic* pButtonInputCharacteristic = nullptr;
    BLECharacteristic* pSensorInputCharacteristic = nullptr;
    BLEDescriptor *pDescr = nullptr;
    BLE2902 *pButtonInputNotifier = nullptr;
    BLE2902 *pSensorInputNotifier = nullptr;
};