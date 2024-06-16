/**
 * @file Wii_Remote.h
 * @brief Wii Remote Header File
 * @author Humza Ali
 */

#pragma once

#include "BLE.h"
#include "IMU_Sensor.h"
#include "BaseController.h"

/**
 * @enum Button_Mapping_t
 * @brief Contains bit mapping for DS4 inputs transmitted through a DSU server
 */
typedef enum Button_Mapping {
    DS4_SHARE    = 0x0001U, /** DS4 Share Button Bit Value */
    DS4_L3       = 0x0002U, /** DS4 L3 Button Bit Value */
    DS4_R3       = 0x0004U, /** DS4 R3 Button Bit Value */
    DS4_OPTIONS  = 0x0008U, /** DS4 Options Button Bit Value */
    DS4_UP       = 0x0010U, /** DS4 D-Pad Up Bit Value */
    DS4_RIGHT    = 0x0020U, /** DS4 D-Pad Right Bit Value */
    DS4_DOWN     = 0x0040U, /** DS4 D-Pad Down Bit Value */
    DS4_LEFT     = 0x0080U, /** DS4 D-Pad Left Bit Value */
    DS4_L2       = 0x0100U, /** DS4 L2 Button Bit Value */
    DS4_R2       = 0x0200U, /** DS4 R2 Button Bit Value */
    DS4_L1       = 0x0400U, /** DS4 L1 Button Bit Value */
    DS4_R1       = 0x0800U, /** DS4 R1 Button Bit Value */
    DS4_TRIANGLE = 0x1000U, /** DS4 Triangle Button Bit Value */
    DS4_CIRCLE   = 0x2000U, /** DS4 Circle Button Bit Value */
    DS4_CROSS    = 0x4000U, /** DS4 Cross Button Bit Value */
    DS4_SQUARE   = 0x8000U, /** DS4 Square Button Bit Value */
} Button_Mapping_t;

/** GPIO pin mappings for the Wii Remote buttons. */
#define BUTTON_A_PIN     (Pins_t)32U /** Wii Remote Button A Pin */
#define BUTTON_B_PIN     (Pins_t)23U /** Wii Remote Button B Pin */
#define BUTTON_1_PIN     (Pins_t)19U /** Wii Remote Button 1 Pin */
#define BUTTON_2_PIN     (Pins_t)26U /** Wii Remote Button 2 Pin */
#define BUTTON_PLUS_PIN  (Pins_t)36U /** Wii Remote Button + Pin */
#define BUTTON_HOME_PIN  (Pins_t)12U /** Wii Remote Button Home Pin */
#define BUTTON_MINUS_PIN (Pins_t)16U /** Wii Remote Button - Pin */
#define DPAD_UP_PIN      (Pins_t)15U /** Wii Remote D-Pad Up Pin */
#define DPAD_DOWN_PIN    (Pins_t)4U  /** Wii Remote D-Pad Down Pin */
#define DPAD_LEFT_PIN    (Pins_t)17U /** Wii Remote D-Pad Left Pin */
#define DPAD_RIGHT_PIN   (Pins_t)34U /** Wii Remote D-Pad Right Pin */

/** Wii Remote Button Input Characteristic UUID */
#define WIIMOTE_BUTTON_INPUT_CHARACTERISTIC_UUID  "7e3092ce-5b65-44c7-afef-c7722ef964b3"
/** Wii Remote Sensor Input Characteristic UUID */
#define WIIMOTE_SENSOR_INPUT_CHARACTERISTIC_UUID  "eb854de2-f0b3-48bf-90ca-1f2a85ef29c8"

/** Wii Remote Accelorometer Range */
#define WIIMOTE_ACCELOROMETER_RANGE 2

/**
 * @class WiiRemote
 * @brief A class used to emulate a Wii Remote.
 *
 * This class inherits from the BaseController class and is designed to handle
 * standard Wii Remote operations and transmit input data through BLE.
 */
class WiiRemote : public BaseController
{
public:
    /**
     * @brief Constructor for the WiiRemote class.
     *
     * Constructs a new instance of the WiiRemote class with the specified
     * BLE and IMU interface.
     *
     * @param[in] pBle Pointer to a BLE object used for transmitting input data
     * @param[in] pWiiRemoteImu Pointer to an IMU sensor object that reads
     *                          sensor data from the IMU sensor equipped on the
     *                          Wii Remote.
     */
    WiiRemote(BLE* pBle, IMU_Sensor* pWiiRemoteImu)
        : pBle(pBle), pWiiRemoteImu(pWiiRemoteImu) {};
    
    /**
     * @brief Deconstructor for the WiiRemote class.
     */
    ~WiiRemote() {};

    /**
     * @brief Interrupt handler called when a button has been pressed
     *        or released.
     *
     * @param[in] pin GPIO pin of the button pressed/released
     */
    static void buttonChangeIRQHandler(Pins_t pin);

    /**
     * @brief Initializes the Wii Remote.
     *
     * @return Status code indicating the result of the call.
     */
    status_t initController(void) override;

    /**
     * @brief Initializes the GPIO pins used for each button on the Wii Remote.
     */
    void initButtonPins(void) override;

    /**
     * @brief Updates the Wii Remote button input data to be transmitted.
     */
    void updateButtonInputs(void) override;

    /**
     * @brief Updates the Wii Remote sensor input data to be transmitted.
     *        The sensor input data will include both accelerometer and
     *        gyroscope data.
     */
    void updateSensorInputs(void) override;

#if DEBUG
    /**
     * @brief Prints the IMU data recorded from the Wii Remote IMU Sensor.
     */
    void printIMUdata(void) override { pWiiRemoteImu->printSensorData(); }
#endif //DEBUG

private:

    BLE* pBle = nullptr; /** Pointer to a BLE object */
    /** Pointer to the button input characteristic object. */
    BLECharacteristic* pButtonInputCharacteristic = nullptr;
    /** Pointer to the sensor input characteristic object. */
    BLECharacteristic* pSensorInputCharacteristic = nullptr;
    /** Pointer to a notifier object for button input values. */
    BLE2902* pButtonInputNotifier = nullptr;
    /** Pointer to a notifier object for sensor input values. */
    BLE2902* pSensorInputNotifier = nullptr;
    /**
     * Map object containing associated pin and button mapping values
     * as a key-value pair.
     */
    static std::map<Pins_t, Button_Mapping_t> buttonPins;
    static uint32_t buttonInput; /** Holds the button input bit values. */
    /**
     * Pointer to an IMU sensor object for initializing an IMU sensor
     * and reading accelorometer and gyroscope data.
     */
    IMU_Sensor* pWiiRemoteImu;
};