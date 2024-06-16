/**
 * @file Wii_Remote.cpp
 * @brief Wii Remote Source File
 * @author Humza Ali
 */

#include "Arduino.h"

#include "include/Wii_Remote.h"
#include "include/BLE.h"
#include "include/IMU_Sensor.h"
#include "include/generic_types.h"

// \ref WiiRemote Static Variables
std::map<Pins_t, Button_Mapping_t> WiiRemote::buttonPins;
uint32_t WiiRemote::buttonInput = 0U;

void WiiRemote::buttonChangeIRQHandler(Pins_t pin)
{
#if DEBUG
    Serial.println(pin);
    Serial.println(WiiRemote::buttonPins[pin]);
    Serial.print("Digital Input Value: ")
    Serial.println(digitalRead(pin));
#endif
    if (digitalRead(pin)) {
        // Button pressed (logical high)
        buttonInput |= buttonPins[pin];
    } else {
        // Button released (logical low)
        buttonInput &= ~buttonPins[pin];
    }
}

void WiiRemote::initButtonPins(void)
{
    // Initialize pins, assign pin and button mapping pairs,
    // and specified IRQ handlers for each button

    pinMode(BUTTON_A_PIN, INPUT);
    WiiRemote::buttonPins[BUTTON_A_PIN] = DS4_CROSS;
    attachInterrupt(digitalPinToInterrupt(BUTTON_A_PIN), []() { WiiRemote::buttonChangeIRQHandler(BUTTON_A_PIN); }, CHANGE);

    pinMode(BUTTON_B_PIN, INPUT);
    WiiRemote::buttonPins[BUTTON_B_PIN] = DS4_CIRCLE;
    attachInterrupt(digitalPinToInterrupt(BUTTON_B_PIN), []() { WiiRemote::buttonChangeIRQHandler(BUTTON_B_PIN); }, CHANGE);

    pinMode(BUTTON_1_PIN, INPUT);
    WiiRemote::buttonPins[BUTTON_1_PIN] = DS4_SQUARE;
    attachInterrupt(digitalPinToInterrupt(BUTTON_1_PIN), []() { WiiRemote::buttonChangeIRQHandler(BUTTON_1_PIN); }, CHANGE);

    pinMode(BUTTON_2_PIN, INPUT);
    WiiRemote::buttonPins[BUTTON_2_PIN] = DS4_TRIANGLE;
    attachInterrupt(digitalPinToInterrupt(BUTTON_2_PIN), []() { WiiRemote::buttonChangeIRQHandler(BUTTON_2_PIN); }, CHANGE);

    pinMode(BUTTON_PLUS_PIN, INPUT);
    WiiRemote::buttonPins[BUTTON_PLUS_PIN] = DS4_R3;
    attachInterrupt(digitalPinToInterrupt(BUTTON_PLUS_PIN), []() { WiiRemote::buttonChangeIRQHandler(BUTTON_PLUS_PIN); }, CHANGE);

    pinMode(BUTTON_HOME_PIN, INPUT);
    WiiRemote::buttonPins[BUTTON_HOME_PIN] = DS4_SHARE;
    attachInterrupt(digitalPinToInterrupt(BUTTON_HOME_PIN), []() { WiiRemote::buttonChangeIRQHandler(BUTTON_HOME_PIN); }, CHANGE);

    pinMode(BUTTON_MINUS_PIN, INPUT);
    WiiRemote::buttonPins[BUTTON_MINUS_PIN] = DS4_L3;
    attachInterrupt(digitalPinToInterrupt(BUTTON_MINUS_PIN), []() { WiiRemote::buttonChangeIRQHandler(BUTTON_MINUS_PIN); }, CHANGE);

    pinMode(DPAD_UP_PIN, INPUT);
    WiiRemote::buttonPins[DPAD_UP_PIN] = DS4_UP;
    attachInterrupt(digitalPinToInterrupt(DPAD_UP_PIN), []() { WiiRemote::buttonChangeIRQHandler(DPAD_UP_PIN); }, CHANGE);

    pinMode(DPAD_DOWN_PIN, INPUT);
    WiiRemote::buttonPins[DPAD_DOWN_PIN] = DS4_DOWN;
    attachInterrupt(digitalPinToInterrupt(DPAD_DOWN_PIN), []() { WiiRemote::buttonChangeIRQHandler(DPAD_DOWN_PIN); }, CHANGE);

    pinMode(DPAD_LEFT_PIN, INPUT);
    WiiRemote::buttonPins[DPAD_LEFT_PIN] = DS4_LEFT;
    attachInterrupt(digitalPinToInterrupt(DPAD_LEFT_PIN), []() { WiiRemote::buttonChangeIRQHandler(DPAD_LEFT_PIN); }, CHANGE);

    pinMode(DPAD_RIGHT_PIN, INPUT);
    WiiRemote::buttonPins[DPAD_RIGHT_PIN] = DS4_RIGHT;
    attachInterrupt(digitalPinToInterrupt(DPAD_RIGHT_PIN), []() { WiiRemote::buttonChangeIRQHandler(DPAD_RIGHT_PIN); }, CHANGE);
}

status_t WiiRemote::initController(void)
{
    status_t status = STATUS_COMPLETE;
    // Create characterisitics
    status = pBle->createCharacteristic(WIIMOTE_BUTTON_INPUT_CHARACTERISTIC_UUID,
                                        pButtonInputCharacteristic,
                                        pButtonInputNotifier);
    status = pBle->createCharacteristic(WIIMOTE_SENSOR_INPUT_CHARACTERISTIC_UUID,
                                        pSensorInputCharacteristic,
                                        pSensorInputNotifier);
    // Intialize Wii Remote buttons
    initButtonPins();
    // Initialize Wii Remote IMU sensor
    status = pWiiRemoteImu->initImuSensor();
    return status;
}

void WiiRemote::updateButtonInputs(void) {
    // Null Check
    if (pBle == nullptr) {
        #if DEBUG
        Serial.println("pBLE is NULL in WiiRemote::updateButtonInputs()");
        #endif
        return;
    }
    if (pButtonInputCharacteristic == nullptr) {
        #if DEBUG
        Serial.println("pButtonInputCharacteristic is NULL in WiiRemote::updateButtonInputs()");
        #endif
        return;
    }
    /**
     * Load the button input data
     * 
     * Payload Format of \ref WiiRemote::buttonInput:
     * -------------------------------------------
     * | buttons1 (2 bytes) | buttons2 (2 bytes) |
     * -------------------------------------------
     */
    pButtonInputCharacteristic->setValue(WiiRemote::buttonInput);
    // Transmit the data
    pBle->notifyCharacterisitic(pButtonInputCharacteristic);
}

void WiiRemote::updateSensorInputs(void) {
    // Null check
    if (pBle == nullptr) {
        #if DEBUG
        Serial.println("pBle is NULL in WiiRemote::updateSensorInputs()");
        #endif
        return;
    }
    if (pWiiRemoteImu->IMU == nullptr) {
        #if DEBUG
        Serial.println("pWiiRemoteImu->IMU is NULL in WiiRemote::updateSensorInputs()");
        #endif
        return;
    }
    if (pSensorInputCharacteristic == nullptr) {
        #if DEBUG
        Serial.println("pSensorInputCharacteristic is NULL in WiiRemote::updateSensorInputs()");
        #endif
        return;
    }
    // Update IMU sensor readings
    pWiiRemoteImu->IMU->update();
    
    AccelData accelData;
    GyroData gyroData;

    // Get accelorometer and gyro data
    pWiiRemoteImu->IMU->getAccel(&accelData);
    pWiiRemoteImu->IMU->getGyro(&gyroData);

    /**
     * Consolidate both sensor data into one characteristic
     * to minimize characteristic overhead
     * 
     * Payload Format of \ref accelGyroDataBytes:
     * --------------------------------------------------------------------------------------------
     * | ax (4 bytes) | ay (4 bytes) | az (4 bytes) | gx (4 bytes) | gy (4 bytes) | gz (4 bytes)  |
     * --------------------------------------------------------------------------------------------
     */
    uint8_t accelGyroDataBytes[ACCEL_GYRO_DATA_SIZE];
    // Load the accelorometer data
    memcpy(accelGyroDataBytes, &accelData, ACCEL_DATA_STRUCT_SIZE);
    // Load the gyroscope data
    memcpy(accelGyroDataBytes + ACCEL_DATA_STRUCT_SIZE, &gyroData, GYRO_DATA_STRUCT_SIZE);
    // Update notification value
    pSensorInputCharacteristic->setValue(accelGyroDataBytes, ACCEL_GYRO_DATA_SIZE);
    // Transmit the data
    pBle->notifyCharacterisitic(pSensorInputCharacteristic);
}
