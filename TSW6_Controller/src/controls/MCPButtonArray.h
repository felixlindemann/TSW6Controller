/**
 * @file MCPButtonArray.h
 * @brief Multi-button control using one or more MCP23S17 port expanders.
 *
 * @details
 *   Provides debounced digital inputs for up to 16 buttons per MCP23S17.
 *   Each button is addressed sequentially (BTN_01 ... BTN_32) and
 *   exposes a normalized Control interface (1.0 = pressed, 0.0 = released).
 *
 * @requires SPI bus initialized before begin().
 *
 * @version 1.0
 * @date 2025-11-02
 * @author
 *   Felix Lindemann
 */

#pragma once
#include <Arduino.h>
#include <SPI.h>
#include "Control.h"
#include "Adafruit_MCP23X17.h"

class MCPButtonArray : public Control {
private:
  Adafruit_MCP23X17* expanders;     // dynamic array of expanders
  uint8_t expanderCount;
  bool states[32];                  // last stable states (max 2x16)
  bool readings[32];
  unsigned long debounceTimes[32];
  unsigned int debounceDelay;
  uint8_t mcpReset;                 // reset expander-array from arduino
  uint8_t csBase;                   // base chip select or address offset
  int lastEventIndex;               // index of last changed button

public:
  explicit MCPButtonArray(const String& idPrefix,
                          uint8_t expanderCount = 1,
                          uint8_t mcpReset = 25,
                          uint8_t csBase = 0,
                          unsigned int debounce = 50);

  void begin() override;
  bool update() override;
  float getValue() const override;        // not meaningful (returns last changed)
  
  // Additional API
  void reset()  ;
  bool getButtonState(uint8_t index) const;
  int  getLastEventIndex() const { return lastEventIndex; }
  String getButtonId(uint8_t index) const;
    
  void loop();
};
