/**
 * @file TSWButton.h
 * @brief TSW-integrated button class for binary controller mappings.
 *
 * @details
 * Extends the hardware Button Control to send mapped 0/1 values to TSWSpider.
 * Each button event (press/release) is mapped via NotchTable and forwarded.
 *
 * @example
 *   TSWButton sifa(12, "Sifa", &spider);
 *   sifa.begin();
 *   sifa.updateAndSend();
 *
 * @note
 * Default mapping:
 *   - Pressed:   tswValue = 1.0
 *   - Released:  tswValue = 0.0
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
#include "../controls/Button.h"

class TSWButton : public Button, public TSWControl
{
private:
  float lastSentValue;

public:
  TSWButton(uint8_t pin, const String &ctrl, TSWSpider *s);

  void loadNotches(const String &filePath);
  void updateAndSend();
};
