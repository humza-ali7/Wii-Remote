/**
 * @file IMU.h
 * @brief Inertial Measurement Unit (IMU) source file
 * @author Humza Ali
 */

#include "include/IMU_Sensor.h"
#include "Arduino.h"

status_t IMU_Sensor::initImuSensor()
{
    Wire.begin();
    Wire.setClock(400000);
    calData calib = { 0, };
    // Initialize the IMU
    int initStatus = IMU->init(calib, deviceAddress);
    if (initStatus != 0) {
        #if DEBUG
        Serial.print("Could not initialize IMU with address:");
        Serial.println(deviceAddress);
        #endif
        return STATUS_IMU_INIT_FAILURE;
    }

#ifdef PERFORM_CALIBRATION
    // Calibration example from the FastIMU library. Can be found at:
    // https://github.com/LiquidCGS/FastIMU/blob/main/examples/Calibrated_sensor_output/Calibrated_sensor_output.ino
    Serial.println("FastIMU calibration & data example");
    delay(5000);
    Serial.println("Keep IMU level.");
    delay(5000);
    IMU->calibrateAccelGyro(&calib);
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
    delay(5000);
    IMU->init(calib, IMU_ADDRESS);
#endif
    initStatus = IMU->setAccelRange(accelRange);
    if (initStatus != 0) {
        #if DEBUG
        Serial.print("Could not set accelorometer range of +/-");
        Serial.print(accelRange);
        Serial.println("g");
        #endif
        return STATUS_IMU_INIT_FAILURE;
    }

    return STATUS_COMPLETE;
}

#if DEBUG
void IMU_Sensor::printSensorData()
{
    AccelData accelData;
    GyroData gyroData;
    IMU->update();
    IMU->getAccel(&accelData);
    Serial.print(accelData.accelX);
    Serial.print("\t");
    Serial.print(accelData.accelY);
    Serial.print("\t");
    Serial.print(accelData.accelZ);
    Serial.print("\t");
    IMU->getGyro(&gyroData);
    Serial.print(gyroData.gyroX);
    Serial.print("\t");
    Serial.print(gyroData.gyroY);
    Serial.print("\t");
    Serial.println(gyroData.gyroZ);
}
#endif