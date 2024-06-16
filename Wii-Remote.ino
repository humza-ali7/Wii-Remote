
/**
 * @file Wii-Remote.ino
 * @brief Main controller file for the ESP32 Wii Remote
 * @author Humza Ali
 */

#include "src/include/Wii_Remote.h"
#include "src/include/Nunchuck.h"
#include "src/include/IMU_Sensor.h"
#include "src/include/BLE.h"

// Initialize the BLE class
static BLE ble("Wii Remote");
// Initialize Wii Remote and Nunchuck IMUs
static IMU_Sensor wiiRemoteImu(0x68, WIIMOTE_ACCELOROMETER_RANGE, MPU6500_NAME);
static IMU_Sensor nunchuckImu(0x69, NUNCHUCK_ACCELOROMETER_RANGE, MPU9250_NAME);
// Initialize the Wii Remote and the Nunchuck
static WiiRemote wiiRemote(&ble, &wiiRemoteImu);
static Nunchuck nunchuck(&ble, &nunchuckImu);

void setup()
{
#if SERIAL_OUTPUT_LOGGING
  Serial.begin(115200);
#endif
  ble.initBle();
  status_t status = wiiRemote.initController();
  if (status != STATUS_COMPLETE) {
    #if SERIAL_OUTPUT_LOGGING
    Serial.print("Wii Remote could not be initialized. Status code: ");
    Serial.println(status);
    #endif
    while (1);
  }
  status = nunchuck.initController();
  if (status != STATUS_COMPLETE) {
    #if SERIAL_OUTPUT_LOGGING
    Serial.print("Nunchuck could not be initialized. Status code: ");
    Serial.println(status);
    #endif
    while (1);
  }
  status = ble.startAdvertising();
  if (status != STATUS_COMPLETE) {
    #if SERIAL_OUTPUT_LOGGING
    Serial.print("Could not begin advertising. Status code: ");
    Serial.println(status);
    #endif
    while (1);
  }
}

void loop()
{
  // Update Wii Remote button inputs
  wiiRemote.updateButtonInputs();
  // Update Wii Remote sensor inputs
  wiiRemote.updateSensorInputs();
  // Update Nunchuck Button Inputs
  nunchuck.updateButtonInputs();
  // Update Nunchuck Sensor Inputs
  nunchuck.updateSensorInputs();
#if DEBUG
  // wiiRemote.printIMUdata();
  // nunchuck.printIMUdata();
  // delay(25);
#endif
}
