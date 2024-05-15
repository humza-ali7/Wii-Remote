/**
 * @file Wii_Remote.cpp
 * @brief Wii Remote Source File
 * @author Humza Ali
 */

#include "Arduino.h"

#include "include/Wii_Remote.h"
#include "include/BLE.h"
#include "include/generic_types.h"

std::map<Pins_t, Button_Mapping_t> WiiRemote::buttonPins;

std::vector<Button_Mapping_t> WiiRemote::activeButtons;

void WiiRemote::buttonPressedIRQHandler(Pins_t pin)
{
    //Serial.println(pin);
    //Serial.println(WiiRemote::buttonPins[pin]);
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
}

void WiiRemote::initButtonPins(void)
{
    pinMode(BUTTON_A_PIN, INPUT);
    WiiRemote::buttonPins[BUTTON_A_PIN] = GAMEPAD_A;
    attachInterrupt(digitalPinToInterrupt(BUTTON_A_PIN), []() { WiiRemote::buttonPressedIRQHandler(BUTTON_A_PIN); }, CHANGE);

    pinMode(BUTTON_B_PIN, INPUT);
    WiiRemote::buttonPins[BUTTON_B_PIN] = GAMEPAD_B;
    attachInterrupt(digitalPinToInterrupt(BUTTON_B_PIN), []() { WiiRemote::buttonPressedIRQHandler(BUTTON_B_PIN); }, CHANGE);

    pinMode(BUTTON_1_PIN, INPUT);
    WiiRemote::buttonPins[BUTTON_1_PIN] = GAMEPAD_X;
    attachInterrupt(digitalPinToInterrupt(BUTTON_1_PIN), []() { WiiRemote::buttonPressedIRQHandler(BUTTON_1_PIN); }, CHANGE);
}

void WiiRemote::initWiiRemote(void)
{
    ble.initBle();
    initButtonPins();
}

void WiiRemote::updateButtonInputs(void)
{
    CRITICAL_SECTION_START();

    buttonInput = 0U;
    uint8_t shift = 0U;
    uint8_t size = WiiRemote::activeButtons.size();

    if (size > 0) {
        buttonInput |= WiiRemote::activeButtons[0];
        if (size > 1) {
            buttonInput |= (uint32_t)WiiRemote::activeButtons[1] << 16U;
        }
    }
    //Serial.print("Button Input:");
    //Serial.println(buttonInput);
    ble.updateButtonInputValue(buttonInput);

    CRITICAL_SECTION_END();
}
