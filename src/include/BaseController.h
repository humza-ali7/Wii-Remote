/**
 * @file BaseController.h
 * @brief Base controller header file.
 * @author Humza Ali
 */

#ifndef BASECONTROLLER_H
#define BASECONTROLLER_H

#include "generic_types.h"

/**
 * @class BaseController
 * @brief A class object containing common functions utilized by
 *        different controllers.
 */
class BaseController
{
public:
    /**
     * @brief Initializes the controller.
     */
    virtual status_t initController(void) = 0;
    /**
     * @brief Initializes the GPIO pins for each button on the controller.
     */
    virtual void initButtonPins(void) = 0;
    /**
     * @brief Updates the button input payload based on each input.
     */
    virtual void updateButtonInputs(void) = 0;
    /**
     * @brief Updates the sensor input payload based on each input.
     */
    virtual void updateSensorInputs(void) = 0;
#if DEBUG
    /**
     * @brief Prints the IMU data of the equipped IMU on the controller.
     */
    virtual void printIMUdata(void) = 0;
#endif
};

#endif