/**
 * @file GamepadJoystick.h
 * @brief Two-axis analog joystick with mandatory click button implementing the Control interface.
 *
 * @details
 * Reads two analog axes (X/Y) and one digital button (click). Provides
 * centered values (−100 … +100 %) for both axes, and a normalized overall
 * magnitude via getValue() (0.0 … 1.0). Implements the Control interface
 * so that the button pin is the fixed hardware identifier.
 *
 * Example:
 * @code
 *   GamepadJoystick viewPad("PAD_VIEW", A0, A1, 4);
 *   viewPad.begin();
 *   if (viewPad.update()) {
 *       int x = viewPad.getXCentered();
 *       int y = viewPad.getYCentered();
 *       bool pressed = viewPad.isPressed();
 *   }
 * @endcode
 *
 * @note
 *  - X/Y centered range: −100 … +100 %
 *  - Button returns 1.0 when pressed
 *  - The Control::pin refers to the button GPIO
 *
 * @auth or Felix Lindemann
 * @date 2025-10-28
 * @version 2.0
 */

#pragma once
#include <Arduino.h>
#include "Control.h"

class GamepadJoystick : public Control {
private:
  uint8_t xPin, yPin;
  int xRaw, yRaw;
  int xZero, yZero;
  int xDeadZone, yDeadZone;
  int xThreshold, yThreshold;
  bool xInverted, yInverted;

  unsigned long lastRead;
  unsigned long interval;

  bool buttonPressed;
  bool lastButtonPressed;

  static constexpr int MAX_ANALOG =
#if defined(ESP32) || defined(ESP8266) || defined(ARDUINO_ARCH_SAMD)
      4095;
#else
      1023;
#endif

  int toCentered(int raw, int zero) const;

public:
  GamepadJoystick(const String& id, uint8_t x, uint8_t y, uint8_t button);

  void begin() override;
  bool update() override;
  float getValue() const override;     // normalized vector magnitude 0.0–1.0

  // Additional getters
  int getXCentered() const;
  int getYCentered() const;
  bool isPressed() const { return buttonPressed; }

  void calibrateCenter();
  void setXInverted(bool inv) { xInverted = inv; }
  void setYInverted(bool inv) { yInverted = inv; }
  void setInterval(unsigned long i) { interval = i; }
};
