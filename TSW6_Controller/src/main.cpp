#include "WifiManager.h"
#include "TSW_Controls/TSWButton.h"
#ifndef BUILD_NUMBER
#define BUILD_NUMBER 1
#endif
#include "TSW_Controls/TSWRotaryKnob.h"

#ifndef BUILD_DATE
#define BUILD_DATE __DATE__ " " __TIME__
#endif
#include "repo/controlsRepo.h"
#include "TSW_Controls/TSWLever.h"
#include "controls/GamepadJoystick.h"

void setupControls()
{
  static Button btn1("btnRed", 26);
  static Button btn2("btnGreen", 27);
  static RotaryKnob btn3("rot01", 12, 14);
  static AnalogSlider sld1("sld01", 32);
  static AnalogSlider sld2("sld02", 33);
  static AnalogSlider sld3("sld03", 36);
  static GamepadJoystick pad1("pad01", 35, 34, 17);

  // Registrierung: bewusst, explizit, transparent
  ControlRegistry::registerControl(&btn1, "Button");
  ControlRegistry::registerControl(&btn2, "AnalogSlider");
  ControlRegistry::registerControl(&btn3, "AnalogSlider");
  ControlRegistry::registerControl(&sld1, "AnalogSlider");
  ControlRegistry::registerControl(&sld2, "AnalogSlider");
  ControlRegistry::registerControl(&sld3, "AnalogSlider");
  ControlRegistry::registerControl(&pad1, "GamepadJoystick");

  ControlRegistry::listAll();
}

void setup()
{
  Serial.begin(115200);
  delay(1000);
  Serial.println();
  Serial.print(DEVICE_NAME);
  Serial.println(" Booting...");

  Serial.print("ESP32 SDK Version: ");
  Serial.println(ESP.getSdkVersion());
  Serial.print("Board: ");
  Serial.println(ARDUINO_BOARD);

  Serial.print("Build #: ");
  Serial.print(BUILD_NUMBER);
  Serial.print("   Datum: ");
  Serial.println(BUILD_DATE);

  beginWiFiManager();
  setupControls();
}
void genericLoop()
{
  static unsigned long lastUpdate = 0;
  unsigned long now = millis();
  if (now - lastUpdate < 50)
    return; // 20 Hz Poll-Rate
  lastUpdate = now;

  for (auto &entry : ControlRegistry::getAll())
  {
    Control *c = entry.instance;
    if (!c)
      continue;

    if (c->update())
    {
      float val = c->getValue();
      Serial.printf("[%-10s] %-14s => %.2f\n",
                    entry.type, entry.id.c_str(), val);
    }
  }
}
void loop()
{
  loopWiFiManager();
  genericLoop();
}
