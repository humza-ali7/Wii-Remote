/**
 * @file IMU_Sensor.h
 * @brief Inertial Measurement Unit (IMU) header file
 * @author Humza Ali
 */

/** 
 * TODO: I probably don't need this class as it may just be redundant
 * when calling MPU functions from the FastIMU library. Maybe
 * port all to Wii_Remote.h? Keeping for now since it works lol
 */

#pragma once

#include <Wire.h>
#include "FastIMU.h"
#include "generic_types.h"

#define MPU6500_ADDRESS 0x68

class IMU_Sensor
{
public:

    err_status_t initImuSensor();

    void printSensorData();

    MPU6500 IMU;

    AccelData accelData;

    GyroData gyroData;

    calData calib = { 0 };  //Calibration data
};