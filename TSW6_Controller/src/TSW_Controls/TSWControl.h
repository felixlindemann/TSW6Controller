/**
 * @file TSWControl.h
 * @brief Abstract base class for all TSW-integrated input controls.
 *
 * @details
 * Provides shared functionality for TSWLever, TSWButton, and TSWRotaryKnob.
 * Handles communication with TSWSpider and NotchTable mapping.
 *
 * Derived classes must implement:
 *   - void updateAndSend();
 *
 * @author Felix Lindemann
 * @date 2025-10-27
 * @version 1.0
 */

#pragma once
#include <Arduino.h>
#include "NotchTable.h"
#include "TSWSpider.h"
#include "../config.h"

class TSWControl {
protected:
  String controllerName;
  NotchTable notches;
  TSWSpider* spider;
  float lastSentValue;

public:
  TSWControl(const String& ctrl, TSWSpider* s)
      : controllerName(ctrl), spider(s), lastSentValue(-999.0f) {}

  virtual ~TSWControl() = default;

  void loadNotches(const String& filePath) { notches.loadFromFile(filePath); }
  const String& getControllerName() const { return controllerName; }

protected:
  void sendValueToTSW(float tswValue) {
    if (!spider) return;
    if (fabs(tswValue - lastSentValue) > 0.001f) {
      spider->setControllerValue(controllerName, tswValue);
      lastSentValue = tswValue;
    }
  }
};
