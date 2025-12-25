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
unsigned long lastUpdate_MAINLOOP = 0;
 
GamepadJoystick joy1("Gamepad1", GPIO_NUM_33, GPIO_NUM_32, GPIO_NUM_12);
MCPButtonArray buttonArray("BTN", 50);

void updateSld(AnalogSlider &sld)
{
  if (sld.update())
  {
    int raw = sld.getCurrentRawValue();
    int pct = sld.getPercentValue();
    float norm = sld.getValue();
    LOG_HW_DEBUG("AnalogSlider %s changed: raw=%d pct=%d norm=%.3f reason=%s\n",
                  sld.getId().c_str(), raw, pct, norm, sld.getChangeReason());
  }
}



void setup()
{
  Serial.begin(115200);

  LOG_SYS_INFO("=== TSW6 Controller Starting ===\n"); 
  Serial.setDebugOutput(false); // unterbindet Core-Debug auf UART0

  SETUPDISPLAY();

  LOG_SYS_INFO("Booting...\n");
  LOG_SYS_DEBUG("   ESP32 SDK Version: %s\n", ESP.getSdkVersion());
  LOG_SYS_DEBUG("   Board: %s\n", ARDUINO_BOARD);
  LOG_SYS_DEBUG("   Build #: %s\n", BUILD_NUMBER);
  LOG_SYS_DEBUG("   Datum: %s\n", BUILD_DATE);
  LOG_SYS_INFO("   Device: %s\n", DEVICE_NAME);
  LOG_SYS_DEBUG("   Free Heap: %d bytes\n", ESP.getFreeHeap());

  // Use full 0–3.3V range for ADC1 pins (GPIO 32–39)
#if defined(ESP32) || defined(ESP8266) || defined(ARDUINO_ARCH_SAMD)
  analogSetWidth(12); // 12-bit resolution (0–4095)
  LOG_HW_DEBUG("ADC configured: 12-bit resolution (0-4095)\n");
#endif

#if USE_WIFIMANAGER
  LOG_SYS_INFO("Initializing WiFi Manager...\n");
  beginWiFiManager(); 
#endif
  
  LOG_SYS_INFO("Initializing TSW Spider API Client...\n");
  tswSpider->begin();
  
  LOG_SW_INFO("Setting up analog sliders...\n");
  SETUP_ANALOG_SLIDER(tswSpider);
  
  LOG_SW_INFO("Setting up gamepad controls...\n");
  SETUP_GAMEPAD(tswSpider);
  
  LOG_SW_INFO("Setting up MCP button arrays...\n");
  SETUP_MCP_BUTTON_ARRAY(tswSpider);
   
  LOG_SYS_INFO("=== Setup Complete - Entering Main Loop ===\n");
  delay(100);
}


 



void loop()
{

#if USE_WIFIMANAGER
  loopWiFiManager();
#endif
 

  unsigned long now = millis();
  if (now - lastUpdate_MAINLOOP < 50)
    return; // 20 Hz polling rate
  
  LOG_SW_TRACE("--- MAIN LOOP @ %lu ms ---\n", now);
  UPDATE_ANALOG_SLIDER();
  UPDATE_GAMEPAD();
  UPDATE_MCP_BUTTON_ARRAY();
  lastUpdate_MAINLOOP = now;
  delay(1);
}
