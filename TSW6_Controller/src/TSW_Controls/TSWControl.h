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

  /**
   * Get the control type name for JSON serialization.
   * Override in derived classes.
   */
  virtual const char* getControlType() const { return "TSWControl"; }

  /**
   * Get the hardware type name for JSON serialization.
   * Override in derived classes.
   */
  virtual const char* getHardwareType() const { return "Unknown"; }

  /**
   * Serialize control to JSON for REST API.
   * Derived classes should call base and add their specific fields.
   */
  virtual void toJson(JsonObject& doc) const {
    doc["controllerName"] = notches.getControllerName();
    doc["type"] = getControlType();
    doc["hardwareType"] = getHardwareType();
    doc["lastSentValue"] = lastSentValue;
    
    JsonObject notchObj = doc.createNestedObject("notches");
    notches.toJson(notchObj);
  }

  /**
   * Abstract method - all TSW controls must implement updateAndSend.
   */
  virtual void updateAndSend() = 0;

protected:
  void sendValueToTSW(float tswValue) {
    if (!spider) return; 
    spider->setControllerValue(notches.getControllerName(), tswValue);
    lastSentValue = tswValue;
  }
};
