
/**
 * @file Wii-Remote.ino
 * @brief Main controller file for the ESP32 Wii Remote
 * @author Humza Ali
 */

#include "src/include/Wii_Remote.h"
#include "src/include/BLE.h"
// Set to 1 to print debug code
#define DEBUG 0

BLE ble("Wii Remote");
WiiRemote wiiRemote(&ble, 21, 22);

void setup()
{
  Serial.begin(115200);
  ble.initBle();
  wiiRemote.initWiiRemote();
  ble.startAdvertising();
}

void loop()
{
  wiiRemote.updateButtonInputs();
  wiiRemote.updateSensorInputs();
  wiiRemote.printIMUdata();
  delay(25);
}
