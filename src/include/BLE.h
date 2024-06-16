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
#include "IMU_Sensor.h"
#include "generic_types.h"

/** BLE Service UUID */
#define SERVICE_UUID "06a1ef1c-d8f5-4839-bf3a-cf1deed694d2"

class BLE 
{
public:
    /**
     * @brief Constructor for the BLE class.
     *
     * Constructs a new instance of the BLE class with a specified name for the
     * BLE device connection. This class will be used to create BLE,
     * characteristics, advertise, and transmit notification data.
     *
     *
     * @param[in] deviceName Name of the BLE device.
     */
    BLE(const char* deviceName) : deviceName(deviceName) {};

    /**
     * @brief Initializes the BLE connection server and service.
     */
    void initBle(void);

    /**
     * @brief Starts advertising for the BLE service.
     */
    status_t startAdvertising(void);

    /**
     * @brief Creates a BLE characteristic.
     *
     * @param[in] characteristicUuid The designated UUID of the characteristic.
     * @param[in, out] pCharacteristic Pointer to a BLE characteristic object.
     * @param[in, out] pNotifier Pointer to a BLE notifier object.
     *
     * @return Status code indicating the result of the call.
     */
    status_t createCharacteristic(const char* characteristicUuid,
                                  BLECharacteristic*& pCharacteristic,
                                  BLE2902*& pNotifier);

    /**
     * @brief Notifies the data stored in a characteristic.
     *
     * @param[in] pCharacterisitic Pointer to a BLE characteristic object.
     */
    void notifyCharacterisitic(BLECharacteristic* pCharacteristic);

    /** Pointer to a BLE service object. */
    BLEServer* pServer = nullptr;

    /** Pointer to a BLE server object. */
    BLEService* pService = nullptr;

private:
    /** Name of the BLE device. */
    const char* deviceName;
};