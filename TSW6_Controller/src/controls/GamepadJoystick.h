#pragma once
#include <Arduino.h>
/**
 * @file GamepadJoystick.h
 * @brief Class representing a 2-axis analog joystick with optional button.
 *
 * @details
 * The GamepadJoystick class reads two analog axes (X/Y) and an optional
 * digital button input. It provides normalized values both in percentage
 * (0–100 %) and centered range (−100 … +100 %), suitable for HID joystick
 * or gamepad interfaces.
 *
 * Features:
 * - Periodic analog sampling with configurable interval
 * - Adjustable dead zone and change threshold
 * - Optional inversion for each axis
 * - Optional button with state change detection
 * - Smoothed readings via simple moving average
 *
 * Lifecycle example:
 * @code
 *   GamepadJoystick joy(A0, A1, 4);
 *   joy.begin();
 *   if (joy.update()) {
 *       int x = joy.getXPercent();
 *       int y = joy.getYPercent();
 *       bool pressed = joy.isPressed();
 *   }
 * @endcode
 *
 * Notes:
 * - No analogRead() is performed in the constructor to ensure USB stability
 *   on boards like the Arduino Leonardo.
 * - MAX_ANALOG resolution is determined at compile time (1023 or 4095).
 *
 * @author Felix Lindemann
 * @date 2025-10-26
 * @version 1.0
 *
 * @copyright
 * This code is part of the TSW Controller Project.
 * Licensed under the MIT License.
 */
 
class GamepadJoystick {
private:
  uint8_t xPin, yPin, btnPin;
  int xRaw, yRaw;
  int xZero, yZero;
  int xDeadZone, yDeadZone;
  int xThreshold, yThreshold;
  bool xInverted, yInverted;
  unsigned long lastRead;
  unsigned long interval;
  int smoothFactor;
  bool hasButton;
  bool buttonPressed;
  bool lastButtonPressed;
  int maxAnalog;  // replaces MAXANALOG

  void defaultInit();
  void detectAnalogResolution();

public:
  GamepadJoystick(uint8_t x, uint8_t y);
  GamepadJoystick(uint8_t x, uint8_t y, uint8_t button);

  void begin();
  void calibrateCenter();

  // Configuration
  void setXDeadZone(int dz);
  void setYDeadZone(int dz);
  void setXThreshold(int t);
  void setYThreshold(int t);
  void setXInverted(bool inv);
  void setYInverted(bool inv);
  void setInterval(unsigned long i);
  void setSmoothing(int s);

  // Update
  bool update();

  // Getters
  int getXRaw() const;
  int getYRaw() const;
  bool isPressed() const;
  int getXPercent() const;
  int getYPercent() const;
  int getXCentered() const;
  int getYCentered() const;
  int getXHID() const;
  int getYHID() const;
};
