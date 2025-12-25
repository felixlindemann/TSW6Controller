// config.h
#pragma once

#define USEDISPLAY 0

// === Logging Configuration ===
// Log Levels: 0=OFF, 1=ERROR, 2=WARN, 3=INFO, 4=DEBUG, 5=TRACE
#define LOG_LEVEL_HARDWARE 5   // INFO
#define LOG_LEVEL_SOFTWARE 5   // INFO
#define LOG_LEVEL_HTTP 5       // INFO
#define LOG_LEVEL_SYSTEM 5     // DEBUG

// Log Level Definitions
#define LOG_OFF 0
#define LOG_ERROR 1
#define LOG_WARN 2
#define LOG_INFO 3
#define LOG_DEBUG 4
#define LOG_TRACE 5

#define STATUS_LED 22

#define USE_WIFIMANAGER 1

#define USE_ANALOG_SLIDER 1
#define PIN_ANALOG_SLIDER { GPIO_NUM_34 ,GPIO_NUM_35 } //,GPIO_NUM_39,GPIO_NUM_36}
#define ANALOG_SLIDER_INVERTED { false   , true} //, false,false}

#define USE_Rotary 0
#define PIN_Rotary {0,0}

#define USE_GAMEPAD 0
#define PIN_GAMEPAD {GPIO_NUM_32, GPIO_NUM_33, GPIO_NUM_12}

#define USE_Button 0
 

#define USE_MCP_BUTTON_ARRAY 0
// we use MCP23S17 expanders for buttons with pin adressing using pins A0,A1,A2
// SCK, MISO, MOSI, CS // Default SPI pins for LOLIN32 boards
#define PIN_SPI {GPIO_NUM_18 , GPIO_NUM_19, GPIO_NUM_23 , GPIO_NUM_5} 
// in default we use 2 expanders for 32 buttons
#define NUM_OF_EXPANDERS 2

// WLAN
#define SETUP_BUTTON GPIO_NUM_26 // if pressed LOLIN Starts in AP-Mode
#define DNS_PORT 53
#define MyIP "192.168.4.1"

// Build Properties
#ifndef BUILD_NUMBER
#define BUILD_NUMBER 1
#endif

#ifndef BUILD_DATE
#define BUILD_DATE __DATE__ " " __TIME__
#endif

// === Advanced Logging Macros ===
#define LOG_HW_ERROR(...)   if(LOG_LEVEL_HARDWARE >= LOG_ERROR) { Serial.printf("[HW ERROR] "); Serial.printf(__VA_ARGS__); }
#define LOG_HW_WARN(...)    if(LOG_LEVEL_HARDWARE >= LOG_WARN)  { Serial.printf("[HW WARN] ");  Serial.printf(__VA_ARGS__); }
#define LOG_HW_INFO(...)    if(LOG_LEVEL_HARDWARE >= LOG_INFO)  { Serial.printf("[HW INFO] ");  Serial.printf(__VA_ARGS__); }
#define LOG_HW_DEBUG(...)   if(LOG_LEVEL_HARDWARE >= LOG_DEBUG) { Serial.printf("[HW DEBUG] "); Serial.printf(__VA_ARGS__); }
#define LOG_HW_TRACE(...)   if(LOG_LEVEL_HARDWARE >= LOG_TRACE) { Serial.printf("[HW TRACE] "); Serial.printf(__VA_ARGS__); }

#define LOG_SW_ERROR(...)   if(LOG_LEVEL_SOFTWARE >= LOG_ERROR) { Serial.printf("[SW ERROR] "); Serial.printf(__VA_ARGS__); }
#define LOG_SW_WARN(...)    if(LOG_LEVEL_SOFTWARE >= LOG_WARN)  { Serial.printf("[SW WARN] ");  Serial.printf(__VA_ARGS__); }
#define LOG_SW_INFO(...)    if(LOG_LEVEL_SOFTWARE >= LOG_INFO)  { Serial.printf("[SW INFO] ");  Serial.printf(__VA_ARGS__); }
#define LOG_SW_DEBUG(...)   if(LOG_LEVEL_SOFTWARE >= LOG_DEBUG) { Serial.printf("[SW DEBUG] "); Serial.printf(__VA_ARGS__); }
#define LOG_SW_TRACE(...)   if(LOG_LEVEL_SOFTWARE >= LOG_TRACE) { Serial.printf("[SW TRACE] "); Serial.printf(__VA_ARGS__); }

#define LOG_HTTP_ERROR(...) if(LOG_LEVEL_HTTP >= LOG_ERROR)     { Serial.printf("[HTTP ERROR] "); Serial.printf(__VA_ARGS__); }
#define LOG_HTTP_WARN(...)  if(LOG_LEVEL_HTTP >= LOG_WARN)      { Serial.printf("[HTTP WARN] ");  Serial.printf(__VA_ARGS__); }
#define LOG_HTTP_INFO(...)  if(LOG_LEVEL_HTTP >= LOG_INFO)      { Serial.printf("[HTTP INFO] ");  Serial.printf(__VA_ARGS__); }
#define LOG_HTTP_DEBUG(...) if(LOG_LEVEL_HTTP >= LOG_DEBUG)     { Serial.printf("[HTTP DEBUG] "); Serial.printf(__VA_ARGS__); }
#define LOG_HTTP_TRACE(...) if(LOG_LEVEL_HTTP >= LOG_TRACE)     { Serial.printf("[HTTP TRACE] "); Serial.printf(__VA_ARGS__); }

#define LOG_SYS_ERROR(...)  if(LOG_LEVEL_SYSTEM >= LOG_ERROR)   { Serial.printf("[SYS ERROR] "); Serial.printf(__VA_ARGS__); }
#define LOG_SYS_WARN(...)   if(LOG_LEVEL_SYSTEM >= LOG_WARN)    { Serial.printf("[SYS WARN] ");  Serial.printf(__VA_ARGS__); }
#define LOG_SYS_INFO(...)   if(LOG_LEVEL_SYSTEM >= LOG_INFO)    { Serial.printf("[SYS INFO] ");  Serial.printf(__VA_ARGS__); }
#define LOG_SYS_DEBUG(...)  if(LOG_LEVEL_SYSTEM >= LOG_DEBUG)   { Serial.printf("[SYS DEBUG] "); Serial.printf(__VA_ARGS__); }
#define LOG_SYS_TRACE(...)  if(LOG_LEVEL_SYSTEM >= LOG_TRACE)   { Serial.printf("[SYS TRACE] "); Serial.printf(__VA_ARGS__); }

#if (NUM_OF_EXPANDERS > 0)
#define USE_MCPBUTTONARRAY 1
#else
#define USE_MCPBUTTONARRAY 0
#endif
// === Geräteinfos ===

extern const char* DEVICE_NAME;
extern const char* DEVICE_SSID;
extern const char* DEVICE_PASS;
extern const char* MDNS_NAME;

extern const int I2C_SDA;
extern const int I2C_SCL;

