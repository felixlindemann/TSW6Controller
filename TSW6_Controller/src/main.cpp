#include "config.h"
#include "repo/controlsRepo.h"

#if USE_WIFIMANAGER
#include "WifiManager.h"
#endif

#include "TSW_Controls/TSWSpider.h"
#include "TSW_Controls/TSWLever.setup.h"
#include "TSW_Controls/TSWGamePadControl.setup.h"
#include "TSW_Controls/TSWMCPButtonArray.setup.h"
  
#include "controls/MCPButtonArray.h"

#if USEDISPLAY
#include "controls/display.h"
#define SETUPDISPLAY() setupDisplay()
#define LOG2DISPLAY(...) gDisplayLog.printLine(__VA_ARGS__)
#else
#define SETUPDISPLAY()
#define LOG2DISPLAY(...)
#endif
TSWSpider* tswSpider = new TSWSpider();
 

/* 
AnalogSlider sld1("AFB", GPIO_NUM_34);
AnalogSlider sld2("Leistungsregler", GPIO_NUM_35);
AnalogSlider sld3("Rollo", GPIO_NUM_39);
AnalogSlider sld4("Bremse", GPIO_NUM_36);
*/

GamepadJoystick joy1("Gamepad1", GPIO_NUM_33, GPIO_NUM_32, GPIO_NUM_12);
MCPButtonArray buttonArray("BTN", 50);

void updateSld(AnalogSlider &sld)
{
  if (sld.update())
  {
    int raw = sld.getCurrentRawValue();
    int pct = sld.getPercentValue();
    float norm = sld.getValue();
    Serial.printf("AnalogSlider %s changed: raw=%d  pct=%d  norm=%.3f  reason=%s\n",
                  sld.getId().c_str(), raw, pct, norm, sld.getChangeReason());
  }
}

void setup()
{
  Serial.begin(115200);
  delay(1000);
  Serial.setDebugOutput(false); // unterbindet Core-Debug auf UART0

  SETUPDISPLAY();

  LOG2DISPLAY("Booting...");
  LOG2DISPLAY("ESP32 SDK Version: " + String(ESP.getSdkVersion()));
  LOG2DISPLAY("Board: " + String(ARDUINO_BOARD));

  LOG2DISPLAY("Build #: " + String(BUILD_NUMBER));
  LOG2DISPLAY("   Datum: " + String(BUILD_DATE));
  LOG2DISPLAY("Device: ");
  LOG2DISPLAY(String(DEVICE_NAME));

  // Use full 0–3.3V range for ADC1 pins (GPIO 32–39)
#if defined(ESP32) || defined(ESP8266) || defined(ARDUINO_ARCH_SAMD)
  analogSetWidth(12); // 12-bit resolution (0–4095)
#endif

#if USE_WIFIMANAGER
  beginWiFiManager();
  LOG2DISPLAY("WiFi Manager started");
#endif
  
  // tswSpider.begin();
  SETUP_ANALOG_SLIDER(tswSpider);
  SETUP_GAMEPAD(tswSpider);
  SETUP_MCP_BUTTON_ARRAY(tswSpider);
   
  delay(100);
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
    
  UPDATE_ANALOG_SLIDER();
  UPDATE_GAMEPAD();
   UPDATE_MCP_BUTTON_ARRAY();
  lastUpdate = now;
  delay(1);
}
