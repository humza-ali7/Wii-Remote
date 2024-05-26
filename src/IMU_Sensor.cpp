/**
 * @file IMU.h
 * @brief Inertial Measurement Unit (IMU) source file
 * @author Humza Ali
 */

#include "include/IMU_Sensor.h"
#include "Arduino.h"

err_status_t IMU_Sensor::initImuSensor()
{
    Wire.begin();
    Wire.setClock(400000); //400khz clock
    while (!Serial) {
        ;
    }

    int err = IMU.init(calib, MPU6500_ADDRESS);
    Serial.print("IMU.init() returned: ");
    Serial.println(err); // Debug print to check the return value
    delay(500);
    if (err != 0) {
        return STATUS_ERROR;
    }

    #ifdef PERFORM_CALIBRATION
    Serial.println("FastIMU calibration & data example");
    if (IMU.hasMagnetometer()) {
        delay(1000);
        Serial.println("Move IMU in figure 8 pattern until done.");
        delay(3000);
        IMU.calibrateMag(&calib);
        Serial.println("Magnetic calibration done!");
    }
    else {
        delay(5000);
    }

    delay(5000);
    Serial.println("Keep IMU level.");
    delay(5000);
    IMU.calibrateAccelGyro(&calib);
    Serial.println("Calibration done!");
    Serial.println("Accel biases X/Y/Z: ");
    Serial.print(calib.accelBias[0]);
    Serial.print(", ");
    Serial.print(calib.accelBias[1]);
    Serial.print(", ");
    Serial.println(calib.accelBias[2]);
    Serial.println("Gyro biases X/Y/Z: ");
    Serial.print(calib.gyroBias[0]);
    Serial.print(", ");
    Serial.print(calib.gyroBias[1]);
    Serial.print(", ");
    Serial.println(calib.gyroBias[2]);
    if (IMU.hasMagnetometer()) {
        Serial.println("Mag biases X/Y/Z: ");
        Serial.print(calib.magBias[0]);
        Serial.print(", ");
        Serial.print(calib.magBias[1]);
        Serial.print(", ");
        Serial.println(calib.magBias[2]);
        Serial.println("Mag Scale X/Y/Z: ");
        Serial.print(calib.magScale[0]);
        Serial.print(", ");
        Serial.print(calib.magScale[1]);
        Serial.print(", ");
        Serial.println(calib.magScale[2]);
    }
    delay(5000);
    IMU.init(calib, IMU_ADDRESS);
    #endif

    //err = IMU.setGyroRange(500);      //USE THESE TO SET THE RANGE, IF AN INVALID RANGE IS SET IT WILL RETURN -1
    //err = IMU.setAccelRange(2);       //THESE TWO SET THE GYRO RANGE TO ±500 DPS AND THE ACCELEROMETER RANGE TO ±2g
    
    if (err != 0) {
        return STATUS_ERROR;
    }
    return STATUS_COMPLETE;
}

void IMU_Sensor::printSensorData()
{
    IMU.update();
    IMU.getAccel(&accelData);
    Serial.print(accelData.accelX);
    Serial.print("\t");
    Serial.print(accelData.accelY);
    Serial.print("\t");
    Serial.print(accelData.accelZ);
    Serial.print("\t");
    IMU.getGyro(&gyroData);
    Serial.print(gyroData.gyroX);
    Serial.print("\t");
    Serial.print(gyroData.gyroY);
    Serial.print("\t");
    Serial.println(gyroData.gyroZ);
}