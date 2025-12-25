/**
 * @file TSWGamePadControl.h
 * @brief 2-axis GamePad joystick (X/Y + Button) mapped to TSW via Spider API.
 *
 * @details
 * Integrates a GamepadJoystick hardware control with the TSW communication layer.
 * Provides per-axis notch mapping and a binary button notch mapping (default 0/1).
 *
 * Example:
 * @code
 *   TSWGamePadControl afbPad("AFBPad", A0, A1, 4, "AFBX", "AFBY", "AFBConfirm", &spider);
 *   afbPad.begin();
 *   afbPad.updateAndSend();
 * @endcode
 *
 * @note
 *  - X/Y range: −100 … +100 %
 *  - getXCentered(), getYCentered() from GamepadJoystick are used
 *  - optional NotchTables per axis
 *  - Button mapping: 1=Pressed, 0=Released
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
#include "../controls/GamepadJoystick.h"
#include "../controls/Button.h"

class TSWGamePadControl : public TSWControl {
private:
  GamepadJoystick gamepad;
  bool hasButton;

  NotchTable notchX;         // Holds controllerX name
  NotchTable notchY;         // Holds controllerY name
  NotchTable buttonNotches;  // Holds controllerButton name

  unsigned long lastSentTime;
  unsigned long sendInterval;

public:
  // 2-Axis joystick (mandatory button)
  TSWGamePadControl(const String& id,
                    uint8_t pinX, uint8_t pinY, uint8_t pinButton,
                    const String& ctrlX, const String& ctrlY,
                    const String& ctrlBtn,
                    TSWSpider* s,
                    unsigned long sendInt = 100);

  void begin();
  void loadNotchesX(const String& filePath);
  void loadNotchesY(const String& filePath);
  void loadButtonNotches(const String& filePath);
  void updateAndSend();

  // Controller names are now stored in NotchTables (single source of truth)
  void setControllerX(const String& controller) { notchX.setControllerName(controller); }
  void setControllerY(const String& controller) { notchY.setControllerName(controller); }
  void setControllerButton(const String& controller) { buttonNotches.setControllerName(controller); }
  
  const String& getControllerX() const { return notchX.getControllerName(); }
  const String& getControllerY() const { return notchY.getControllerName(); }
  const String& getControllerButton() const { return buttonNotches.getControllerName(); }

  void setXInverted(bool inv) { gamepad.setXInverted(inv); }
  void setYInverted(bool inv) { gamepad.setYInverted(inv); }
};
