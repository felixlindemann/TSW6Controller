/**
 * @file TSWRotaryKnob.h
 * @brief TSW-integrated rotary encoder with incremental value tracking.
 *
 * @details
 * Extends RotaryKnob (hardware) and TSWControl (logic) to provide
 * incremental value mapping between configurable min/max limits.
 * Each rotation step updates and sends a mapped value to TSWSpider.
 *
 * Example:
 * @code
 *   TSWRotaryKnob knob(4, 5, "TrainBrake", &spider, 0.0, 1.0);
 *   knob.begin();
 *   knob.updateAndSend();
 * @endcode
 *
 * @author
 *   Felix Lindemann
 * @date
 *   2025-11-02
 * @version
 *   2.1
 */ 

#pragma once
#include <Arduino.h>
#include "../controls/RotaryKnob.h"
#include "TSWControl.h"

class TSWRotaryKnob : public RotaryKnob, public TSWControl {
private:
  float minValue;
  float maxValue;
  float currentTSWValue;

public:
  // --- Constructors ---
  TSWRotaryKnob(const String& id, uint8_t a, uint8_t b,
                TSWSpider* spider, float minVal = 0.0f, float maxVal = 1.0f);

  // --- Legacy overload for backward compatibility ---
  TSWRotaryKnob(const String& id, uint8_t a, uint8_t b,
                int steps, float minVal, float maxVal);

  // --- Lifecycle ---
  void begin() override;
  bool update() override;

  // --- TSW mapping ---
  void updateAndSend();
};