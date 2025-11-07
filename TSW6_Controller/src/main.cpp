#include "config.h"
#include "repo/controlsRepo.h"

#if USE_WIFIMANAGER
#include "WifiManager.h"
#endif

#include "TSW_Controls/TSWSpider.h"
TSWSpider tswSpider = TSWSpider();

#include "TSW_Controls/TSWLever.setup.h"
#include "TSW_Controls/TSWRotaryKnob.setup.h"
#include "TSW_Controls/TSWGamePadControl.setup.h"
#include "TSW_Controls/TSWMCPButton.setup.h"
#include "TSW_Controls/TSWButton.setup.h"

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

#if USE_WIFIMANAGER
  beginWiFiManager();
#endif

  SETUP_ANALOG_SLIDER(&tswSpider);
  SETUP_ROTARYBUTTON(&tswSpider);
  SETUP_GAMEPAD(&tswSpider);
  SETUP_MCPButtonArray(&tswSpider);
  SETUP_BUTTONS(&tswSpider);

  ControlRegistry::listAll();

  delay(100);
}

void loopAnalogControls(unsigned long now)
{
  for (auto &entry : ControlRegistry::getAll())
  {
    if (entry.type != "AnalogSlider" && entry.id != "pad1")
      continue;

    Control *control = entry.instance;
    if (!control)
      continue;

    bool changed = control->update();
    if (!changed)
      continue;

    float value = control->getValue();

#if TRACE
    if (entry.id == "pad1")
    {
#if USE_GAMEPAD
      TRACE_PRINT("[%lu ms] %-12s %-14s => x: %d  y: %d",
                  now, entry.type, entry.id.c_str(),
                  pad1Ptr->getXCentered(), pad1Ptr->getYCentered());
      TRACE_PRINT("   [CHANGED: %s]\n", control->getChangeReason());
      TRACE_PRINT("\n");
#endif
    }
    else
    {
      TRACE_PRINT("[%lu ms] %-12s %-14s => %.2f   [CHANGED: %s]\n",
                  now, entry.type, entry.id.c_str(), value,
                  control->getChangeReason());
    }
#endif
  }
}

void loopButtonControls(unsigned long now)
{
  for (auto &entry : ControlRegistry::getAll())
  {
    if (entry.type != "Button" &&
        entry.type != "MCPButton" &&
        // entry.type != "MCPButtonArray" &&
        entry.type != "GamepadJoystick")
      continue;

    Control *control = entry.instance;
    if (!control)
      continue;

    bool changed = control->update();
    if (!changed)
      continue;

    float value = control->getValue();
  }
}

void loopRotaryControls(unsigned long now)
{
  for (auto &entry : ControlRegistry::getAll())
  {
    if (entry.type != "RotaryKnob")
      continue;

    Control *control = entry.instance;
    if (!control)
      continue;

    bool changed = control->update();
    if (!changed)
      continue;

#if TRACE

    TRACE_PRINT("[%lu ms] %-12s %-14s => %.2f   [CHANGED: %s]\n",
                now, entry.type, entry.id.c_str(),
                control->getValue(), control->getChangeReason());
#endif
  }
}

void loopTraceHeartbeat(unsigned long now)
{
#if TRACE
  static unsigned long lastTrace = 0;
  if (now - lastTrace > 2000)
  {
    lastTrace = now;
    TRACE_PRINT("---- Trace heartbeat at %lu ms ----\n", now);
  }
#endif
}

void loop()
{

#if USE_WIFIMANAGER
  loopWiFiManager();
#endif

  static unsigned long lastUpdate = 0;
  unsigned long now = millis();

  if (now - lastUpdate < 50)
    return; // 20 Hz polling rate

  loopAnalogControls(now);
  loopButtonControls(now);
  loopRotaryControls(now);
  loopTraceHeartbeat(now);
  lastUpdate = now;
  delay(1);
}
