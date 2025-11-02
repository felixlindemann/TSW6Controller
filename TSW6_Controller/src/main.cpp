#define TRACE 1 // if 1 show Results of every control. Set only for dev to 1

#ifndef BUILD_NUMBER
#define BUILD_NUMBER 1
#endif

#ifndef BUILD_DATE
#define BUILD_DATE __DATE__ " " __TIME__
#endif

#include "WifiManager.h"
#include "TSW_Controls/TSWButton.h"
#include "TSW_Controls/TSWRotaryKnob.h"
#include "repo/controlsRepo.h"
#include "TSW_Controls/TSWLever.h"
#include "controls/GamepadJoystick.h"
#include "controls/MCPButtonArray.h"

#if TRACE
#define TRACE_PRINT(...) Serial.printf(__VA_ARGS__)
#else
#define TRACE_PRINT(...)
#endif
static GamepadJoystick *pad1Ptr = nullptr;

void setupControls()
{
  /*
  static Button btn1("btnRed", 26);
  ControlRegistry::registerControl(&btn1, "Button");
  static Button btn2("btnGreen", 27);
  ControlRegistry::registerControl(&btn2, "Button");
  static RotaryKnob btn3("rot01", 12, 14);
  ControlRegistry::registerControl(&btn3, "Button");
  */

  static AnalogSlider sld1("sld1", GPIO_NUM_32);
  ControlRegistry::registerControl(&sld1, "AnalogSlider");
  sld1.setInverted(true);

  static AnalogSlider sld2("sld2", GPIO_NUM_35);
  ControlRegistry::registerControl(&sld2, "AnalogSlider");
  sld2.setInverted(true);

  static AnalogSlider sld3("sld3", GPIO_NUM_34);
  ControlRegistry::registerControl(&sld3, "AnalogSlider");

  static GamepadJoystick pad1("pad1", GPIO_NUM_39, GPIO_NUM_36, GPIO_NUM_13);
  ControlRegistry::registerControl(&pad1, "GamepadJoystick");
  pad1.setXInverted(true);
  pad1.setYInverted(true);
  pad1Ptr = &pad1;

  static MCPButtonArray mcpButtons("BTN", 1, 25, 0, 50);
  mcpButtons.begin();
  ControlRegistry::registerControl(&mcpButtons, "MCPButton");

  static RotaryKnob rotary01("rot01", GPIO_NUM_16, GPIO_NUM_17);
  ControlRegistry::registerControl(&rotary01, "RotaryKnob");
  rotary01.begin();

  ControlRegistry::listAll();

  Serial.printf("ADC X raw=%d, ADC Y raw=%d\n", analogRead(GPIO_NUM_39), analogRead(GPIO_NUM_36));

  delay(2000);
}

void setup()
{
  Serial.begin(115200);
  delay(1000);
  Serial.setDebugOutput(false); // unterbindet Core-Debug auf UART0

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

  // Use full 0–3.3V range for ADC1 pins (GPIO 32–39)

#if defined(ESP32) || defined(ESP8266) || defined(ARDUINO_ARCH_SAMD)
  analogSetWidth(12); // 12-bit resolution (0–4095)
#endif

  beginWiFiManager();
  setupControls();
}

void genericLoop()
{
  static unsigned long lastUpdate = 0;
  static unsigned long lastTrace = 0;
  unsigned long now = millis();

  if (now - lastUpdate < 50)
    return; // 20 Hz polling rate
  lastUpdate = now;

  for (auto &entry : ControlRegistry::getAll())
  {
    Control *control = entry.instance;
    if (!control)
      continue;

    bool changed = control->update();
    float value = control->getValue();
#if TRACE
    if (changed)
    {
      if (changed && entry.id == "pad1" && pad1Ptr)
      {
        TRACE_PRINT("[%lu ms] %-12s %-14s => x: %d  y: %d",
                    now, entry.type, entry.id.c_str(),
                    pad1Ptr->getXCentered(), pad1Ptr->getYCentered());
        TRACE_PRINT("   [CHANGED: %s]\n", control->getChangeReason());
        TRACE_PRINT("\n");
      }
      else
      {
        TRACE_PRINT("[%lu ms] %-12s %-14s => %.2f",
                    now, entry.type, entry.id.c_str(), value);
        TRACE_PRINT("   [CHANGED: %s]\n", control->getChangeReason());
        TRACE_PRINT("\n");
      }
    }
#endif
  }

#if TRACE
  if (now - lastTrace > 2000)
  {
    lastTrace = now;
    TRACE_PRINT("---- Trace heartbeat at %lu ms ----\n", now);
  }
#endif
}

void loop()
{
  loopWiFiManager();
  genericLoop();
  delay(1);
  // Vor JEDEM großen Block:
}
