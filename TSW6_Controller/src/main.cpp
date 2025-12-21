#include "config.h"
#include "repo/controlsRepo.h"

#if USE_WIFIMANAGER
#include "WifiManager.h"
#endif



#include "controls/AnalogSlider.h"
#include "controls/GamepadJoystick.h"
#include "controls/MCPButtonArray.h"

#if USEDISPLAY
#include "controls/display.h"
#define SETUPDISPLAY() setupDisplay()
#define LOG2DISPLAY(...) gDisplayLog.printLine(__VA_ARGS__)
#else
#define SETUPDISPLAY() 
#define LOG2DISPLAY(...) 
#endif

AnalogSlider sld1("AFB", GPIO_NUM_34);
AnalogSlider sld2("Leistungsregler", GPIO_NUM_35);
AnalogSlider sld3("Rollo", GPIO_NUM_39);
AnalogSlider sld4("Bremse", GPIO_NUM_36);

GamepadJoystick joy1("Gamepad1", GPIO_NUM_33, GPIO_NUM_32, GPIO_NUM_12);
MCPButtonArray buttonArray("BTN", 50);

void updateSld(AnalogSlider &sld)
{
  if (sld.update())
  {
    int raw = sld.getRawValue();
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
  LOG2DISPLAY("ESP32 SDK Version: " + String(ESP.getSdkVersion())  );
  LOG2DISPLAY("Board: " + String(ARDUINO_BOARD));

  LOG2DISPLAY("Build #: " + String(BUILD_NUMBER) ); 
  LOG2DISPLAY( "   Datum: " + String(BUILD_DATE) );
  LOG2DISPLAY("Device: " ); LOG2DISPLAY( String( DEVICE_NAME) );
   
  // Use full 0–3.3V range for ADC1 pins (GPIO 32–39)
#if defined(ESP32) || defined(ESP8266) || defined(ARDUINO_ARCH_SAMD)
  analogSetWidth(12); // 12-bit resolution (0–4095)
#endif

#if USE_WIFIMANAGER
  beginWiFiManager();
  LOG2DISPLAY("WiFi Manager started");
#endif

  sld1.begin();
  sld2.begin();
  sld2.setInverted(true);
  sld3.begin();
  sld4.begin();

  joy1.begin();
  joy1.setYInverted(true);
  joy1.setXInverted(true);

  buttonArray.begin();


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

  updateSld(sld1);
  updateSld(sld2);
  updateSld(sld3);
  updateSld(sld4);

  if (joy1.update())
  {
    int x = joy1.getXCentered();
    int y = joy1.getYCentered();
    bool pressed = joy1.isPressed();
    Serial.printf("GamepadJoystick %s changed: x=%d  y=%d  pressed=%d  reason=%s\n",
                  joy1.getId().c_str(), x, y, pressed ? 1 : 0, joy1.getChangeReason());
  }

  if (buttonArray.update())
  {
    String title = "Pin.   :";
    String value = "Value  :";

    for (int i = 0; i < TOTAL_BUTTONS; i++)
    {
      if(i<16){
      char buf[4]; // reicht für "07\0"
      sprintf(buf, " %02d", i+1);
      title += buf;
}
if(i==16){

      value += "\n        ";
}
      value += String(" ") + (buttonArray.getButtonState(i) ? "T" : "F") + String(" ");
    }
    Serial.println(title);
    Serial.println(value);
  }

  lastUpdate = now;
  delay(1);
}
