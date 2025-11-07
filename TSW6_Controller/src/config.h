// config.h
#pragma once
#define TRACE 1
#define STATUS_LED 22

#define USE_WIFIMANAGER 1

#define USE_ANALOG_SLIDER 1
#define PIN_ANALOG_SLIDER {GPIO_NUM_32, GPIO_NUM_35, GPIO_NUM_34}
#define ANALOG_SLIDER_INVERTED {true, true, false}

#define USE_Rotary 0
#define PIN_Rotary {GPIO_NUM_32, GPIO_NUM_35}

#define USE_GAMEPAD 0
#define PIN_GAMEPAD {GPIO_NUM_32, GPIO_NUM_35, GPIO_NUM_12}
#define USE_Button 0
#define PIN_BUTTONS \
  {                 \
  }

#define PIN_EXPANDERS {4, 5}
#define PIN_EXPANDERSRESET GPIO_NUM_25
#define NUM_OF_EXPANDERS 2

// WLAN
#define SETUP_BUTTON 26 // if pressed LOLIN Starts in AP-Mode
#define DNS_PORT 53
#define MyIP "192.168.4.1"

// Build Properties
#ifndef BUILD_NUMBER
#define BUILD_NUMBER 1
#endif

#ifndef BUILD_DATE
#define BUILD_DATE __DATE__ " " __TIME__
#endif

// function

#if TRACE
#define TRACE_PRINT(...) Serial.printf(__VA_ARGS__)
#else
#define TRACE_PRINT(...)
#endif

#if (NUM_OF_EXPANDERS > 0)
#define USE_MCPBUTTONARRAY 1
#else
#define USE_MCPBUTTONARRAY 0
#endif
