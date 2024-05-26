/**
 * @file Wii_Remote.h
 * @brief Wii Remote Header File
 * @author Humza Ali
 */

#pragma once

#include "BLE.h"
#include "IMU_Sensor.h"

typedef enum Button_Mapping {
    DS4_SHARE    = 0x0001U,
    DS4_L3       = 0x0002U,
    DS4_R3       = 0x0004U,
    DS4_OPTIONS  = 0x0008U,
    DS4_UP       = 0x0010U,
    DS4_RIGHT    = 0x0020U,
    DS4_DOWN     = 0x0040U,
    DS4_LEFT     = 0x0080U,
    DS4_L2       = 0x0100U,
    DS4_R2       = 0x0200U,
    DS4_L1       = 0x0400U,
    DS4_R1       = 0x0800U,
    DS4_TRIANGLE = 0x1000U,
    DS4_CIRCLE   = 0x2000U,
    DS4_CROSS    = 0x4000U,
    DS4_SQUARE   = 0x8000U,
} Button_Mapping_t;

#define BUTTON_A_PIN     (Pins_t)32U
#define BUTTON_B_PIN     (Pins_t)23U
#define BUTTON_1_PIN     (Pins_t)19U
#define BUTTON_2_PIN     (Pins_t)26U
#define BUTTON_PLUS_PIN  (Pins_t)36U
#define BUTTON_HOME_PIN  (Pins_t)12U
#define BUTTON_MINUS_PIN (Pins_t)16U
#define DPAD_UP_PIN      (Pins_t)15U
#define DPAD_DOWN_PIN    (Pins_t)4U
#define DPAD_LEFT_PIN    (Pins_t)17U
#define DPAD_RIGHT_PIN   (Pins_t)34U


#define MAX_INPUTS   2U
#define INPUT1_MASK  0xFFFF0000UL
#define INPUT2_MASK  0x0000FFFFUL

class WiiRemote 
{
public:

    WiiRemote(BLE* ble, Pins_t sda, Pins_t scl)
        : ble(ble), wiiRemoteImu(sda, scl) {};

    static void buttonPressedIRQHandler(Pins_t pin);

    void initWiiRemote(void);

    void initButtonPins(void);

    void createBleCharacteristics(void);

    void updateButtonInputs(void);

    void updateSensorInputs(void);

    void printIMUdata(void) { wiiRemoteImu.printSensorData(); }

private:

    BLE* ble = nullptr;

    BLECharacteristic* pButtonInputCharacteristic = nullptr;
    BLECharacteristic* pSensorInputCharacteristic = nullptr;
    BLEDescriptor *pDescr = nullptr;
    BLE2902 *pButtonInputNotifier = nullptr;
    BLE2902 *pSensorInputNotifier = nullptr;
    
    static std::vector<Button_Mapping_t> activeButtons;    

    static std::map<Pins_t, Button_Mapping_t> buttonPins;

    static uint32_t buttonInput;

    IMU_Sensor wiiRemoteImu;
};