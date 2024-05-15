/**
 * @file Wii_Remote.h
 * @brief Wii Remote Header File
 * @author Humza Ali
 */

#pragma once

#include "BLE.h"

typedef enum Button_Mapping {
    DPAD_UP = 0x0001U,
    DPAD_DOWN = 0x0002U,
    DPAD_LEFT = 0x0004U,
    DPAD_RIGHT = 0x0008U,
    GAMEPAD_START = 0x0010U,
    GAMEPAD_BACK = 0x0020U,
    GAMEPAD_LEFT_THUMB = 0x0040U,
    GAMEPAD_RIGHT_THUMB = 0x0080U,
    GAMEPAD_LEFT_SHOULDER = 0x0100U,
    GAMEPAD_RIGHT_SHOULDER = 0x0200U,
    GAMEPAD_GUIDE = 0x0400U,
    GAMEPAD_A = 0x1000U,
    GAMEPAD_B = 0x2000U,
    GAMEPAD_X = 0x4000U,
    GAMEPAD_Y = 0x8000U,
} Button_Mapping_t;

typedef uint8_t Pins_t;

#define BUTTON_A_PIN (Pins_t)36U
#define BUTTON_B_PIN (Pins_t)23U
#define BUTTON_1_PIN (Pins_t)35U

#define MAX_INPUTS   2U
#define INPUT1_MASK  0xFFFF0000UL
#define INPUT2_MASK  0x0000FFFFUL

class WiiRemote {
public:

    WiiRemote() {};

    static void buttonPressedIRQHandler(Pins_t pin);

    void initWiiRemote(void);

    void initButtonPins(void);

    void updateButtonInputs(void);

private:
    
    static std::vector<Button_Mapping_t> activeButtons;    

    static std::map<Pins_t, Button_Mapping_t> buttonPins;

    uint32_t buttonInput;

    portMUX_TYPE myMutex;

    BLE ble{"Wii Remote"};
};