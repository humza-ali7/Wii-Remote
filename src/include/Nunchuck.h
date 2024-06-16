/**
 * @file Nunchuck.h
 * @brief Nunchuck Header File
 * @author Humza Ali
 */

#pragma once

#include "BaseController.h"
#include "BLE.h"
#include "IMU_Sensor.h"
#include "generic_types.h"

// These two buttons are the only two buttons not included as
// the buttons1 and buttons2 inputs in dsu.py, so we'll use
// these as the c and z buttons on the nunchuck.
// (Yes, it makes sense to use DS4_HOME for the the Wii Remote
//  Home button, but these two buttons not having a mapping
//  makes it more convenient for the nunchuck buttons lol)
#define BUTTON_C_PIN  (Pins_t)18U /** Nunchuck Button C Pin */
#define BUTTON_Z_PIN  (Pins_t)5U  /** Nunchuck Button Z Pin */

// These are self defined bit values, not part of the buttons1
// and buttons2 variables as noted in the comment above.
#define DS4_HOME      0x01U /** DS4 Home Bit Value */
#define DS4_PAD_CLICK 0x02U /** DS4 Pad Click Bit Value */

#define JOYSTICK_VRX_PIN (Pins_t)39U /** Joystick X-Axis Pin */
#define JOYSTICK_VRY_PIN (Pins_t)35U /** Joystick Y-Axis Pin */

/** Right shift value to scale down the digital read on the axis pins */
#define JOYSTICK_SCALE_DOWN_SHIFT 4U
/** Size of the Button + Joystick payload */
#define BUTTON_JOYSTICK_DATA_SIZE 3U

/** Nunchuck Button + Joystick Input Characteristic UUID */
#define NUNCHUCK_BUTTON_JOYSTICK_INPUT_CHARACTERISTIC_UUID  "3327921d-e3b3-43ff-b724-a706fae760d3"
/** Nunchuck Sensor Input Characteristic UUID */
#define NUNCHUCK_SENSOR_INPUT_CHARACTERISTIC_UUID           "be11ecb2-1c60-4411-9385-0436b247c5bb"

/** Nunchuck Accelorometer Range */
#define NUNCHUCK_ACCELOROMETER_RANGE 2

/**
 * @class Nunchuck
 * @brief A class used to emulate a Wii Nunchuck.
 *
 * This class inherits from the BaseController class and is designed to handle
 * standard Nunchuck operations and transmit input data through BLE.
 */
class Nunchuck : public BaseController
{
public:
    /**
     * @brief Constructor for the Nunchuck class.
     *
     * Constructs a new instance of the Nunchuck class with the specified
     * BLE and IMU interface.
     *
     * @param[in] pBle Pointer to a BLE object used for transmitting input data
     * @param[in] pNunchuckImu Pointer to an IMU sensor object that reads
     *                         sensor data from the IMU sensor equipped on the
     *                         Nunchuck.
     */
    Nunchuck(BLE* pBle, IMU_Sensor* pNunchuckImu)
        : pBle(pBle), pNunchuckImu(pNunchuckImu) {};

    /**
     * @brief Deconstructor for the Nunchuck class.
     */
    ~Nunchuck() {};

    /**
     * @brief Interrupt handler called when a button has been pressed
     *        or released.
     *
     * @param[in] pin GPIO pin of the button pressed/released
     */
    static void buttonChangeIRQHandler(Pins_t pin);

    /**
     * @brief Initializes the Nunchuck.
     * 
     * @return Status code indicating the result of the call.
     */
    status_t initController(void) override;

    /**
     * @brief Initializes the GPIO pins used for each button on the Nunchuck.
     */
    void initButtonPins(void) override;

    /**
     * @brief Updates the Nunchuck button input data to be transmitted.
     *        This also includes the joystick input from the Nunchuck.
     *
     * @return Status code indicating the result of the call.
     */
    void updateButtonInputs(void) override;

    /**
     * @brief Updates the Nunchuck sensor input data to be transmitted.
     *        The sensor input data will only include the accelerometer
     *        data from the IMU sensor.
     *
     * @return Status code indicating the result of the call.
     */
    void updateSensorInputs(void) override;

#if DEBUG
    /**
     * @brief Prints the IMU data recorded from the Nunchuck IMU Sensor.
     */
    void printIMUdata(void) override { pNunchuckImu->printSensorData(); }
#endif

private:

    BLE* pBle = nullptr; /** Pointer to a BLE object */
    /** Pointer to the button + joystick input characteristic object */
    BLECharacteristic* pButtonJoystickInputCharacteristic = nullptr;
    /** Pointer to the sensor input characteristic object */
    BLECharacteristic* pSensorInputCharacteristic = nullptr;
    /** Pointer to a notifier object for button input values.*/
    BLE2902* pButtonInputNotifier = nullptr;
    /** Pointer to a notifier object for sensor input values.*/
    BLE2902* pSensorInputNotifier = nullptr;
    /** Holds the button input bit values. */
    static uint8_t buttonInput;
    /** 
     * Map object containing associated pin and button mapping values
     * as a key-value pair.
     */
    static std::map<Pins_t, uint8_t> buttonPins;
    /** 
     * A pointer to an IMU sensor object for initializing an IMU sensor and
     * reading accelorometer data.
     */
    IMU_Sensor* pNunchuckImu;
};