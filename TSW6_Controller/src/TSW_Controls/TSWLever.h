/**
 * @file TSWLever.h
 * @brief TSW-integrated analog lever control.
 *
 * @details
 * Reads an AnalogSlider input, maps its percentage value via NotchTable
 * and sends the corresponding mapped value to the TSWSpider interface.
 *
 * Example:
 * @code
 *   TSWLever throttle(A0, "Throttle", &spider);
 *   throttle.begin();
 *   throttle.updateAndSend();
 * @endcode
 *
 * @note
 *   - Analog input (0–100 %) mapped via NotchTable.
 *   - Default behavior: pass-through if no Notches loaded.
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
#include "../controls/AnalogSlider.h"

class TSWLever : public AnalogSlider, public TSWControl {
private:
  float lastSentValue;

public:
  TSWLever(uint8_t pin, const String& ctrl, TSWSpider* s);

  void loadNotches(const String& filePath);
  void updateAndSend() override;

  void setinverted(bool inv);

  // --- JSON Serialization ---
  const char* getControlType() const override { return "TSWLever"; }
  const char* getHardwareType() const override { return "AnalogSlider"; }
  
  void toJson(JsonObject& doc) const override {
    TSWControl::toJson(doc);
    doc["pin"] = AnalogSlider::getPin();
    doc["currentPercent"] = AnalogSlider::getPercentValue();
    doc["inverted"] = AnalogSlider::getInverted();
  }
};
