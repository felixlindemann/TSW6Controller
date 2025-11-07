/**
 * @file MCPButtonArray.h
 * @brief Multi-button input handler for MCP23S17 port expanders.
 *
 * @details
 * Provides debounced digital inputs for up to 16 buttons per MCP23S17 device.
 * Each button is internally represented by an MCPButtonProxy instance and
 * can be accessed individually via the ControlRegistry.
 * The MCPButtonArray itself is also registered as a Control to allow
 * central debugging and polling in the main loop.
 *
 * Hardware requirements:
 *  - One or more MCP23S17 expanders connected via SPI
 *  - Each input pin configured as INPUT_PULLUP
 *  - Optional external pull-ups or debouncing capacitors
 *
 * @note
 *  This class handles debouncing and state tracking for all attached expanders.
 *  The actual button logic and ControlRegistry integration are handled in the
 *  associated MCPButtonProxy class.
 *
 * @author
 *   Felix Lindemann
 * @date
 *   2025-11-02
 * @version
 *   1.2
 */

#pragma once
#include <Arduino.h>
#include <SPI.h>
#include "Control.h"
#include "Adafruit_MCP23X17.h"
#include "../config.h"
#ifndef NUM_OF_EXPANDERS
#define NUM_OF_EXPANDERS 1
#endif
#ifndef PIN_EXPANDERS
#define PIN_EXPANDERS {5}
#endif
#ifndef PIN_EXPANDERSRESET
#define PIN_EXPANDERSRESET 25
#endif
#define BUTTONS_PER_EXPANDER 16
#define TOTAL_BUTTONS (NUM_OF_EXPANDERS * BUTTONS_PER_EXPANDER)

const uint8_t csPins[] = PIN_EXPANDERS;
class MCPButtonArray : public Control
{
private: 
  Adafruit_MCP23X17* expanders;  // <== HIER
  uint8_t expanderCount = NUM_OF_EXPANDERS;
  bool states[TOTAL_BUTTONS];
  bool readings[TOTAL_BUTTONS];
  unsigned long debounceTimes[TOTAL_BUTTONS];
  unsigned int debounceDelay;
  uint8_t mcpReset = PIN_EXPANDERSRESET;
  int lastEventIndex;
  unsigned long lastPollTime;

public:
  explicit MCPButtonArray(const String &idPrefix ="BTN",  
                          unsigned int debounce = 50);

  void begin() override;
  bool update() override;
  float getValue() const override;

  void reset();
  bool getButtonState(uint8_t index) const;
  int getLastEventIndex() const { return lastEventIndex; }
  String getButtonId(uint8_t index) const;
};
