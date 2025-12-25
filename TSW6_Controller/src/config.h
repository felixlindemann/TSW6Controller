// config.h
// =============================================================================
// Compile-Time Hardware & Software Configuration
// =============================================================================
#pragma once

#include <Arduino.h>

// =============================================================================
// SECTION 1: BUILD PROPERTIES
// =============================================================================

#ifndef BUILD_NUMBER
#define BUILD_NUMBER 0
#endif

#ifndef BUILD_DATE
#define BUILD_DATE __DATE__ " " __TIME__
#endif

// =============================================================================
// SECTION 2: LOGGING CONFIGURATION
// =============================================================================
// Log Levels: 0=OFF, 1=ERROR, 2=WARN, 3=INFO, 4=DEBUG, 5=TRACE

#define LOG_OFF   0
#define LOG_ERROR 1
#define LOG_WARN  2
#define LOG_INFO  3
#define LOG_DEBUG 4
#define LOG_TRACE 5

// Configure log levels per subsystem
#define LOG_LEVEL_HARDWARE 3  // Hardware (Pins, I2C, SPI)
#define LOG_LEVEL_SOFTWARE 3  // Software (Control logic)
#define LOG_LEVEL_HTTP     3  // HTTP/API communication
#define LOG_LEVEL_SYSTEM   4  // System (Boot, WiFi, Config)

// Hardware Logging Macros
#define LOG_HW_ERROR(...)   if(LOG_LEVEL_HARDWARE >= LOG_ERROR) { Serial.printf("[HW ERROR] "); Serial.printf(__VA_ARGS__); }
#define LOG_HW_WARN(...)    if(LOG_LEVEL_HARDWARE >= LOG_WARN)  { Serial.printf("[HW WARN] ");  Serial.printf(__VA_ARGS__); }
#define LOG_HW_INFO(...)    if(LOG_LEVEL_HARDWARE >= LOG_INFO)  { Serial.printf("[HW INFO] ");  Serial.printf(__VA_ARGS__); }
#define LOG_HW_DEBUG(...)   if(LOG_LEVEL_HARDWARE >= LOG_DEBUG) { Serial.printf("[HW DEBUG] "); Serial.printf(__VA_ARGS__); }
#define LOG_HW_TRACE(...)   if(LOG_LEVEL_HARDWARE >= LOG_TRACE) { Serial.printf("[HW TRACE] "); Serial.printf(__VA_ARGS__); }

// Software Logging Macros
#define LOG_SW_ERROR(...)   if(LOG_LEVEL_SOFTWARE >= LOG_ERROR) { Serial.printf("[SW ERROR] "); Serial.printf(__VA_ARGS__); }
#define LOG_SW_WARN(...)    if(LOG_LEVEL_SOFTWARE >= LOG_WARN)  { Serial.printf("[SW WARN] ");  Serial.printf(__VA_ARGS__); }
#define LOG_SW_INFO(...)    if(LOG_LEVEL_SOFTWARE >= LOG_INFO)  { Serial.printf("[SW INFO] ");  Serial.printf(__VA_ARGS__); }
#define LOG_SW_DEBUG(...)   if(LOG_LEVEL_SOFTWARE >= LOG_DEBUG) { Serial.printf("[SW DEBUG] "); Serial.printf(__VA_ARGS__); }
#define LOG_SW_TRACE(...)   if(LOG_LEVEL_SOFTWARE >= LOG_TRACE) { Serial.printf("[SW TRACE] "); Serial.printf(__VA_ARGS__); }

// HTTP Logging Macros
#define LOG_HTTP_ERROR(...) if(LOG_LEVEL_HTTP >= LOG_ERROR) { Serial.printf("[HTTP ERROR] "); Serial.printf(__VA_ARGS__); }
#define LOG_HTTP_WARN(...)  if(LOG_LEVEL_HTTP >= LOG_WARN)  { Serial.printf("[HTTP WARN] ");  Serial.printf(__VA_ARGS__); }
#define LOG_HTTP_INFO(...)  if(LOG_LEVEL_HTTP >= LOG_INFO)  { Serial.printf("[HTTP INFO] ");  Serial.printf(__VA_ARGS__); }
#define LOG_HTTP_DEBUG(...) if(LOG_LEVEL_HTTP >= LOG_DEBUG) { Serial.printf("[HTTP DEBUG] "); Serial.printf(__VA_ARGS__); }
#define LOG_HTTP_TRACE(...) if(LOG_LEVEL_HTTP >= LOG_TRACE) { Serial.printf("[HTTP TRACE] "); Serial.printf(__VA_ARGS__); }

// System Logging Macros
#define LOG_SYS_ERROR(...)  if(LOG_LEVEL_SYSTEM >= LOG_ERROR) { Serial.printf("[SYS ERROR] "); Serial.printf(__VA_ARGS__); }
#define LOG_SYS_WARN(...)   if(LOG_LEVEL_SYSTEM >= LOG_WARN)  { Serial.printf("[SYS WARN] ");  Serial.printf(__VA_ARGS__); }
#define LOG_SYS_INFO(...)   if(LOG_LEVEL_SYSTEM >= LOG_INFO)  { Serial.printf("[SYS INFO] ");  Serial.printf(__VA_ARGS__); }
#define LOG_SYS_DEBUG(...)  if(LOG_LEVEL_SYSTEM >= LOG_DEBUG) { Serial.printf("[SYS DEBUG] "); Serial.printf(__VA_ARGS__); }
#define LOG_SYS_TRACE(...)  if(LOG_LEVEL_SYSTEM >= LOG_TRACE) { Serial.printf("[SYS TRACE] "); Serial.printf(__VA_ARGS__); }

// =============================================================================
// SECTION 3: HARDWARE CONFIGURATION - Pins & Buses
// =============================================================================

// --- Status LED ---
#define PIN_STATUS_LED GPIO_NUM_22

// --- I2C Bus (for Display, etc.) ---
#define PIN_I2C_SDA GPIO_NUM_17
#define PIN_I2C_SCL GPIO_NUM_16

// --- SPI Bus (for MCP23S17 expanders) ---
#define PIN_SPI_SCK  GPIO_NUM_18
#define PIN_SPI_MISO GPIO_NUM_19
#define PIN_SPI_MOSI GPIO_NUM_23
#define PIN_SPI_CS   GPIO_NUM_5

// --- Analog Inputs (ADC1 pins: GPIO 32-39) ---
#define PIN_ANALOG_SLIDER      { GPIO_NUM_34, GPIO_NUM_35, GPIO_NUM_39, GPIO_NUM_36 }
#define ANALOG_SLIDER_INVERTED { false, true, false, true } // Invert reading for each slider if needed

// --- Rotary Encoder ---
#define PIN_ROTARY_CLK GPIO_NUM_0
#define PIN_ROTARY_DT  GPIO_NUM_0

// --- Gamepad/Joystick ---
#define PIN_GAMEPAD_X   GPIO_NUM_32
#define PIN_GAMEPAD_Y   GPIO_NUM_33
#define PIN_GAMEPAD_BTN GPIO_NUM_12

// --- WiFi Setup Button ---
#define PIN_SETUP_BUTTON GPIO_NUM_26

// =============================================================================
// SECTION 4: HARDWARE FEATURE TOGGLES
// =============================================================================

#define USE_DISPLAY          0
#define USE_WIFIMANAGER      1
#define USE_ANALOG_SLIDER    1
#define USE_ROTARY           0
#define USE_GAMEPAD          0
#define USE_BUTTON           0
#define USE_MCP_BUTTON_ARRAY 1

// MCP23S17 Expander Configuration
#define NUM_MCP_EXPANDERS 2
#if (NUM_MCP_EXPANDERS > 0)
  #define USE_MCPBUTTONARRAY 1
#else
  #define USE_MCPBUTTONARRAY 0
#endif

// =============================================================================
// SECTION 5: NETWORK DEFAULTS (overridden by config.json at runtime)
// =============================================================================

#define DEFAULT_DEVICE_NAME    "TSW-Controller"
#define DEFAULT_MDNS_NAME      "tsw-controller"
#define DEFAULT_AP_SSID        "TSW-Controller-AP"
#define DEFAULT_AP_PASSWORD    "12345678"
#define DEFAULT_AP_IP          "192.168.4.1"
#define DEFAULT_DNS_PORT       53

// =============================================================================
// SECTION 6: TSW API DEFAULTS (overridden by config.json at runtime)
// =============================================================================

#define DEFAULT_TSW_HOST "10.10.78.96"
#define DEFAULT_TSW_PORT 31270

// =============================================================================
// SECTION 7: DISPLAY MACROS
// =============================================================================

#if USE_DISPLAY
  #include "controls/display.h"
  #define SETUP_DISPLAY() setupDisplay()
  #define LOG2DISPLAY(...) gDisplayLog.printLine(__VA_ARGS__)
#else
  #define SETUP_DISPLAY()
  #define LOG2DISPLAY(...)
#endif

