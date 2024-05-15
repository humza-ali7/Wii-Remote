
/**
 * @file Wii-Remote.ino
 * @brief Main controller file for the ESP32 Wii Remote
 * @author Humza Ali
 */

#include "src/include/Wii_Remote.h"
//#include "src/include/BLE.h"

WiiRemote wiiRemote;
//BLE ble{"Wii Remote"};

void setup()
{
  Serial.begin(115200);
  wiiRemote.initWiiRemote();
}

void loop()
{
  wiiRemote.updateButtonInputs();
  //Serial.println("here");
  delay(100);
}