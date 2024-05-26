
/**
 * @file Wii-Remote.ino
 * @brief Main controller file for the ESP32 Wii Remote
 * @author Humza Ali
 */

#include "src/include/Wii_Remote.h"
//#include "src/include/BLE.h"

WiiRemote wiiRemote;
// Set to 1 to print debug code
#define DEBUG 0

void setup()
{
  Serial.begin(115200);
  wiiRemote.initWiiRemote();
}

void loop()
{
  wiiRemote.updateButtonInputs();
  wiiRemote.updateSensorInputs();
  wiiRemote.printIMUdata();
  delay(100);
}
