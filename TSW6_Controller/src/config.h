// config.h
#pragma once
#define TRACE 1
#define STATUS_LED 22

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