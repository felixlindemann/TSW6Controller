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
  void updateAndSend();
};
