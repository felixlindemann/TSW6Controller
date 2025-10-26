#include "WifiManager.h"
#ifndef BUILD_NUMBER
#define BUILD_NUMBER 1
#endif

#ifndef BUILD_DATE
#define BUILD_DATE __DATE__ " " __TIME__
#endif

void setup() {
  Serial.begin(115200);
  delay(1000);
  Serial.println();
  Serial.println("TSW Pico Booting...");
  
Serial.print("ESP32 SDK Version: ");
Serial.println(ESP.getSdkVersion());
Serial.print("Board: ");
Serial.println(ARDUINO_BOARD);

  
  Serial.print("Build #: ");
  Serial.print(BUILD_NUMBER);
  Serial.print("   Datum: ");
  Serial.println(BUILD_DATE);

  beginWiFiManager();
}

void loop() {
  loopWiFiManager();
}
