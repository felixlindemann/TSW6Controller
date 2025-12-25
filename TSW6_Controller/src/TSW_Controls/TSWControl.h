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
  NotchTable notches;        // NotchTable holds the controllerName
  TSWSpider* spider;
  float lastSentValue;

public:
  TSWControl(const String& ctrl, TSWSpider* s)
      : spider(s), lastSentValue(-999.0f) {
    notches.setControllerName(ctrl);  // Set controllerName in NotchTable
  }

  virtual ~TSWControl() = default;

  void loadNotches(const String& filePath) { notches.loadFromFile(filePath); }
  
  // controllerName now comes from NotchTable (single source of truth)
  const String& getControllerName() const { return notches.getControllerName(); }
  void setControllerName(const String& ctrl) { notches.setControllerName(ctrl); }

protected:
  void sendValueToTSW(float tswValue) {
    if (!spider) return; 
    spider->setControllerValue(notches.getControllerName(), tswValue);
    lastSentValue = tswValue;
  }
};
