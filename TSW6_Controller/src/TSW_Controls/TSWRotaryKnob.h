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
 *   2025-10-28
 * @version
 *   2.0
 */

#pragma once
#include "TSWControl.h"
#include "../controls/RotaryKnob.h"

class TSWRotaryKnob : public RotaryKnob, public TSWControl {
private:
  float lastValue;
  float minValue;
  float maxValue;
  float stepSize;
  unsigned long lastSentTime;
  unsigned long sendInterval;

public:
  TSWRotaryKnob(uint8_t pinA, uint8_t pinB, const String& ctrl, TSWSpider* s,
                float minV = 0.0f, float maxV = 1.0f, float step = 0.05f,
                unsigned long sendInt = 100);

  void loadNotches(const String& filePath);
  void updateAndSend();
};
