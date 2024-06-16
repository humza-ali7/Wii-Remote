/**
 * @file Nunchuck.cpp
 * @brief Wii Remote Source File
 * @author Humza Ali
 */

#include "Arduino.h"

#include "include/Nunchuck.h"

std::map<Pins_t, uint8_t> Nunchuck::buttonPins;

uint8_t Nunchuck::buttonInput = 0U;

void Nunchuck::buttonChangeIRQHandler(Pins_t pin)
{
#if DEBUG
    Serial.println(pin);
    Serial.println(Nunchuck::buttonPins[pin]);
    Serial.print("Digital Input Value: ");
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

void Nunchuck::initButtonPins(void)
{
    // Initialize pins, assign pin and button mapping pairs,
    // and specified IRQ handlers for each button

    pinMode(BUTTON_C_PIN, INPUT);
    Nunchuck::buttonPins[BUTTON_C_PIN] = DS4_HOME;
    attachInterrupt(digitalPinToInterrupt(BUTTON_C_PIN), []() { Nunchuck::buttonChangeIRQHandler(BUTTON_C_PIN); }, CHANGE);

    pinMode(BUTTON_Z_PIN, INPUT);
    Nunchuck::buttonPins[BUTTON_Z_PIN] = DS4_PAD_CLICK;
    attachInterrupt(digitalPinToInterrupt(BUTTON_Z_PIN), []() { Nunchuck::buttonChangeIRQHandler(BUTTON_Z_PIN); }, CHANGE);
}

status_t Nunchuck::initController(void)
{
    initButtonPins();
    status_t status = pNunchuckImu->initImuSensor();
    status = pBle->createCharacteristic(NUNCHUCK_BUTTON_JOYSTICK_INPUT_CHARACTERISTIC_UUID,
                                       pButtonJoystickInputCharacteristic,
                                       pButtonInputNotifier);
    status = pBle->createCharacteristic(NUNCHUCK_SENSOR_INPUT_CHARACTERISTIC_UUID,
                                        pSensorInputCharacteristic,
                                        pSensorInputNotifier);

    return status;
}

void Nunchuck::updateSensorInputs(void)
{
    // Null checks
    if (pBle == nullptr) {
        #if DEBUG
        Serial.println("pBLE is NULL in Nunchuck::updateSensorInputs().");
        #endif
        return;
    }
    if (pNunchuckImu->IMU == nullptr) {
        #if DEBUG
        Serial.println("pNunchuckImu->IMU is NULL in Nunchuck::updateSensorInputs().");
        #endif
        return;
    }
    if (pSensorInputCharacteristic == nullptr) {
        #if DEBUG
        Serial.println("pSensorInputCharacteristic is NULL in Nunchuck::updateSensorInputs().")
        #endif
        return;
    }
    // Update IMU data
    pNunchuckImu->IMU->update();

    AccelData accelData;
    // Get accelorometer data
    pNunchuckImu->IMU->getAccel(&accelData);
    /**
     * Payload Format of \ref accelGyroDataBytes:
     * ----------------------------------------------
     * | ax (4 bytes) | ay (4 bytes) | az (4 bytes) |
     * ----------------------------------------------
     */
    uint8_t accelDataBytes[ACCEL_GYRO_DATA_SIZE];
    // Load the accelorometer data
    memcpy(accelDataBytes, &accelData, ACCEL_DATA_STRUCT_SIZE);
    // Updaate notification value
    pSensorInputCharacteristic->setValue(accelDataBytes, ACCEL_DATA_STRUCT_SIZE);
    // Transmit the data
    pBle->notifyCharacterisitic(pSensorInputCharacteristic);
}

void Nunchuck::updateButtonInputs(void)
{
    // Null check
    if (pBle == nullptr) {
        #if DEBUG
        Serial.println("pBLE is NULL in Nunchuck::updateButtonInputs().");
        #endif
        return;
    }
    if (pButtonJoystickInputCharacteristic == nullptr) {
        #if DEBUG
        Serial.println("pButtonJoystickInputCharacteristic is NULL in Nunchuck::updateButtonInputs().");
        #endif
        return;
    }
    // C++ compilers should recognize dividing by a power of 2, in this case 16,
    // however I'm not really too sure, so just know that all this is doing is
    // dividing the analog reading by 16.
    uint8_t xAxisValue = (uint8_t)(analogRead(JOYSTICK_VRX_PIN) >> JOYSTICK_SCALE_DOWN_SHIFT);
    uint8_t yAxisValue = (uint8_t)(analogRead(JOYSTICK_VRY_PIN) >> JOYSTICK_SCALE_DOWN_SHIFT);
    /**
     * Payload Format of \ref buttonJoystickInputs:
     * -------------------------------------------------------------------------------
     * | Nunchuck::buttonInput (1 byte) | xAxisValue (1 bytes) | yAxisValue (1 byte) |
     * -------------------------------------------------------------------------------
     */
    uint8_t buttonJoystickInputs[BUTTON_JOYSTICK_DATA_SIZE] = {
        xAxisValue, yAxisValue, Nunchuck::buttonInput
    };
    // Load the button input + joystick data
    pButtonJoystickInputCharacteristic->setValue(buttonJoystickInputs, BUTTON_JOYSTICK_DATA_SIZE);
    // Trasmit the data
    pBle->notifyCharacterisitic(pButtonJoystickInputCharacteristic);
}