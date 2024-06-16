/**
 * @file IMU_Sensor.h
 * @brief Inertial Measurement Unit (IMU) header file
 * @author Humza Ali
 */

#pragma once

#include <Wire.h>
#include <cstring>
#include "FastIMU.h"
#include "generic_types.h"

#define MPU9250_NAME "MPU9250" /** String name of the MPU9250 */
#define MPU6500_NAME "MPU6500" /** String name of the MPU6500 */
#define MPU6050_NAME "MPU6050" /** String name of the MPU6050 */

/** Size of the \ref AccelData struct */
#define ACCEL_DATA_STRUCT_SIZE            (size_t)sizeof(AccelData)
/** Size of the \ref GyroData struct */
#define GYRO_DATA_STRUCT_SIZE             (size_t)sizeof(GyroData)
/** 
 * Total size of the \ref AccelData and \ref Gyro Data structs combined.
 * Used for defining the size of the sensor input payload of the
 * Wii Remote in \ref WiiRemote::updateSensorInputs().
 */
#define ACCEL_GYRO_DATA_SIZE              (size_t)(ACCEL_DATA_STRUCT_SIZE + GYRO_DATA_STRUCT_SIZE)

/**
 * @class IMU_Sensor
 * @brief Class representing an Inertial Measurement Unit (IMU) sensor.
 */
class IMU_Sensor
{
public:
    /**
     * @brief Constructor for the IMU_Sensor class.
     *
     * Constructs a new instance of the IMU_Sensor class with a specified
     * I2C address and indicating the type of IMU sensor that is equipped.
     *
     * @param[in] deviceAddress The I2C address of the equipped IMU device
     * @param[in] accelRange The accelorometer range of the IMU, in units of g.
     * @param[in] imuSensorName A string containing the name of the type of
     *                          IMU sensor equipped.
     *
     * @note Three types of IMUs can be initialized depending on the
     *       name provided in the \ref imuSensorName.
     *
     *       1. MPU9250
     *       2. MPU6500
     *       3. MPU6050
     *
     *       There are other sensors supported in the FastIMU library, but
     *       these are the most commonly used ones. Many of the MPUs also
     *       have multiple MPUs supported on one module as well.
     */
    IMU_Sensor(uint8_t deviceAddress, int accelRange, const char* imuSensorName)
        : deviceAddress(deviceAddress), accelRange(accelRange) {
        if (!strcmp(imuSensorName, MPU9250_NAME)) {
            IMU = new MPU9250();
        } else if (!strcmp(imuSensorName, MPU6500_NAME)) {
            IMU = new MPU6500();
        } else if (!strcmp(imuSensorName, MPU6050_NAME)) {
            IMU = new MPU6050();
        } else {
            IMU = nullptr;
        }
    }

    /**
     * @brief Deconstructor for the IMU_Sensor class.
     */
    ~IMU_Sensor() {};

    /**
     * @brief Initializes the IMU sensor.
     *
     * @return Status code indicating the result of the call.
     */
    status_t initImuSensor();

#if DEBUG
    /**
     * @brief Prints sensor data recorded from \ref IMU.
     *
     * @note Should only be called when using a debug build.
     */
    void printSensorData();
#endif

    IMUBase* IMU; /** Base IMU object representing the equipped IMU. */

private:
    uint8_t deviceAddress; /** The I2C address of the equipped IMU. */
    int accelRange; /** The accelorometer range of the IMU, in units of g. */
};