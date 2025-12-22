/**
 * @file TSWMCPButtonArray.h
 * @brief MCP23S17 button array mapped to TSW via Spider API.
 *
 * @details
 * Wraps MCPButtonArray (hardware + debounce) and sends mapped values to TSW.
 * On each debounced edge (pressed/released), the configured controller is updated.
 *
 * Default behavior:
 *  - pressed  -> 1.0
 *  - released -> 0.0
 *
 * Mapping can be configured per button index.
 *
 * @author
 *   Felix Lindemann
 * @date
 *   2025-12-22
 * @version
 *   1.0
 */

#pragma once

#include <Arduino.h>
#include "TSWControl.h"
#include "../controls/MCPButtonArray.h"

class TSWMCPButtonArray : public TSWControl {
public:
  struct ButtonMapping {
    String controller;
    float pressedValue;
    float releasedValue;

    ButtonMapping()
        : controller(""),
          pressedValue(1.0f),
          releasedValue(0.0f) {}
  };

private:
  MCPButtonArray buttonArray;
  ButtonMapping mappings[TOTAL_BUTTONS];

  unsigned long lastSentTimeMs;
  unsigned long sendIntervalMs;

public:
  /**
   * @param id        Logical id for this TSW control wrapper
   * @param spider    TSWSpider instance used to send controller values
   * @param debounceMs Debounce time forwarded to MCPButtonArray
   * @param sendIntMs  Optional rate-limit for sending (default: 0 = send immediately on edge)
   */
  TSWMCPButtonArray(const String& id,
                    TSWSpider* spider,
                    unsigned int debounceMs = 50,
                    unsigned long sendIntMs = 0);

  void begin();

  /**
   * Configure a mapping for a given MCP button index (0..TOTAL_BUTTONS-1).
   * If controller is empty, the button is ignored.
   */
  void setMapping(uint8_t index,
                  const String& controller,
                  float pressedValue = 1.0f,
                  float releasedValue = 0.0f);

  /**
   * Convenience: assign controller names based on a prefix, e.g. "BTN_"
   * -> BTN_00, BTN_01, ...
   */
  void setDefaultControllerPrefix(const String& prefix,
                                  float pressedValue = 1.0f,
                                  float releasedValue = 0.0f);


                                  void updateBtn(uint8_t indec);
  /**
   * Call in loop; sends only when MCPButtonArray reports a debounced edge.
   */
  void updateAndSend();
  
};
