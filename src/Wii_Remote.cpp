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

std::map<Pins_t, Button_Mapping_t> WiiRemote::buttonPins;

//std::vector<Button_Mapping_t> WiiRemote::activeButtons;

uint32_t WiiRemote::buttonInput = 0U;

void WiiRemote::buttonPressedIRQHandler(Pins_t pin)
{
#if DEBUG
    Serial.println(pin);
    Serial.println(WiiRemote::buttonPins[pin]);
    uint8_t pinStatus = digitalRead(pin);
    if (!pinStatus) {
        // Pin read is 0, indicating the button input was let go
        WiiRemote::activeButtons.erase(std::remove(WiiRemote::activeButtons.begin(),
                                                   WiiRemote::activeButtons.end(),
                                                   WiiRemote::buttonPins[pin]),
                                                   WiiRemote::activeButtons.end());
        return;
    }
    if (WiiRemote::activeButtons.size() < MAX_INPUTS) {
        WiiRemote::activeButtons.push_back(WiiRemote::buttonPins[pin]);
    }
#endif
    if (digitalRead(pin)) {
        buttonInput |= buttonPins[pin];
    } else {
        buttonInput &= ~buttonPins[pin];
    }
}

void WiiRemote::initButtonPins(void)
{
    pinMode(BUTTON_A_PIN, INPUT);
    WiiRemote::buttonPins[BUTTON_A_PIN] = DS4_CROSS;
    attachInterrupt(digitalPinToInterrupt(BUTTON_A_PIN), []() { WiiRemote::buttonPressedIRQHandler(BUTTON_A_PIN); }, CHANGE);

    pinMode(BUTTON_B_PIN, INPUT);
    WiiRemote::buttonPins[BUTTON_B_PIN] = DS4_CIRCLE;
    attachInterrupt(digitalPinToInterrupt(BUTTON_B_PIN), []() { WiiRemote::buttonPressedIRQHandler(BUTTON_B_PIN); }, CHANGE);

    pinMode(BUTTON_1_PIN, INPUT);
    WiiRemote::buttonPins[BUTTON_1_PIN] = DS4_SQUARE;
    attachInterrupt(digitalPinToInterrupt(BUTTON_1_PIN), []() { WiiRemote::buttonPressedIRQHandler(BUTTON_1_PIN); }, CHANGE);

    pinMode(BUTTON_2_PIN, INPUT);
    WiiRemote::buttonPins[BUTTON_2_PIN] = DS4_TRIANGLE;
    attachInterrupt(digitalPinToInterrupt(BUTTON_2_PIN), []() { WiiRemote::buttonPressedIRQHandler(BUTTON_2_PIN); }, CHANGE);

    pinMode(BUTTON_PLUS_PIN, INPUT);
    WiiRemote::buttonPins[BUTTON_PLUS_PIN] = DS4_R3;
    attachInterrupt(digitalPinToInterrupt(BUTTON_PLUS_PIN), []() { WiiRemote::buttonPressedIRQHandler(BUTTON_PLUS_PIN); }, CHANGE);

    pinMode(BUTTON_HOME_PIN, INPUT);
    WiiRemote::buttonPins[BUTTON_HOME_PIN] = DS4_SHARE;
    attachInterrupt(digitalPinToInterrupt(BUTTON_HOME_PIN), []() { WiiRemote::buttonPressedIRQHandler(BUTTON_HOME_PIN); }, CHANGE);

    pinMode(BUTTON_MINUS_PIN, INPUT);
    WiiRemote::buttonPins[BUTTON_MINUS_PIN] = DS4_L3;
    attachInterrupt(digitalPinToInterrupt(BUTTON_MINUS_PIN), []() { WiiRemote::buttonPressedIRQHandler(BUTTON_MINUS_PIN); }, CHANGE);

    pinMode(DPAD_UP_PIN, INPUT);
    WiiRemote::buttonPins[DPAD_UP_PIN] = DS4_UP;
    attachInterrupt(digitalPinToInterrupt(DPAD_UP_PIN), []() { WiiRemote::buttonPressedIRQHandler(DPAD_UP_PIN); }, CHANGE);

    pinMode(DPAD_DOWN_PIN, INPUT);
    WiiRemote::buttonPins[DPAD_DOWN_PIN] = DS4_DOWN;
    attachInterrupt(digitalPinToInterrupt(DPAD_DOWN_PIN), []() { WiiRemote::buttonPressedIRQHandler(DPAD_DOWN_PIN); }, CHANGE);

    pinMode(DPAD_LEFT_PIN, INPUT);
    WiiRemote::buttonPins[DPAD_LEFT_PIN] = DS4_LEFT;
    attachInterrupt(digitalPinToInterrupt(DPAD_LEFT_PIN), []() { WiiRemote::buttonPressedIRQHandler(DPAD_LEFT_PIN); }, CHANGE);

    pinMode(DPAD_RIGHT_PIN, INPUT);
    WiiRemote::buttonPins[DPAD_RIGHT_PIN] = DS4_RIGHT;
    attachInterrupt(digitalPinToInterrupt(DPAD_RIGHT_PIN), []() { WiiRemote::buttonPressedIRQHandler(DPAD_RIGHT_PIN); }, CHANGE);
}

void WiiRemote::initWiiRemote(void)
{
    imu.initImuSensor();
    ble.initBle();
    initButtonPins();
}

void WiiRemote::updateButtonInputs(void)
{
    // Could maybe place in header file depending on Nunchuck implementation
    ble.updateButtonInputValue(WiiRemote::buttonInput);
}

void WiiRemote::updateSensorInputs(void)
{
    imu.IMU.update();
    imu.IMU.getAccel(&imu.accelData);
    imu.IMU.getGyro(&imu.gyroData);
    ble.updateSensorInputValue(imu.accelData, imu.gyroData);
}
