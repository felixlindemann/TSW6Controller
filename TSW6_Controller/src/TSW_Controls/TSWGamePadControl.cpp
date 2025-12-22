/**
 * @file TSWGamePadControl.cpp
 * @brief Implementation of the TSWGamePadControl.
 *
 * @details
 * Uses GamepadJoystick as physical input (X/Y + Button)
 * and sends mapped TSW values through TSWSpider.
 */

#include "TSWGamePadControl.h"
#include "../config.h"
#include <cmath> // fabs

// --- Constructor ---
TSWGamePadControl::TSWGamePadControl(const String &id,
                                     uint8_t pinX, uint8_t pinY, uint8_t pinButton,
                                     const String &ctrlX, const String &ctrlY,
                                     const String &ctrlBtn,
                                     TSWSpider *s,
                                     unsigned long sendInt)
    : TSWControl(id, s),
      gamepad(id + "_HW", pinX, pinY, pinButton),
      hasButton(true),
      controllerX(ctrlX),
      controllerY(ctrlY),
      controllerButton(ctrlBtn),
      lastSentTime(0),
      sendInterval(sendInt)
{
  // Default NotchTable for button
  Notch released = {"Released", 0.0f, 0, 0};
  Notch pressed = {"Pressed", 1.0f, 1, 1};
  buttonNotches.loadFromArray({released, pressed});
}

// --- Begin ---
void TSWGamePadControl::begin()
{
  gamepad.begin();
}

// --- Load custom notch mappings ---
void TSWGamePadControl::loadNotchesX(const String &filePath) { notchX.loadFromFile(filePath); }
void TSWGamePadControl::loadNotchesY(const String &filePath) { notchY.loadFromFile(filePath); }
void TSWGamePadControl::loadButtonNotches(const String &filePath) { buttonNotches.loadFromFile(filePath); }




  void TSWGamePadControl::setControllerX(const String& controller){
    controllerX = controller;
  }
  void TSWGamePadControl::setControllerY(const String& controller){
    controllerY = controller;
  }
  void TSWGamePadControl::setControllerButton(const String& controller){
    controllerButton = controller;
  }
    

// --- Update and send ---
void TSWGamePadControl::updateAndSend()
{
  if (!spider)
    return;
  if (!gamepad.update())
    return;

  unsigned long now = millis();

  // X axis
  int xVal = gamepad.getXCentered(); // −100 … +100
  float tswX = notchX.hasPositions() ? notchX.mapToTSW(xVal) : xVal / 100.0f;

  // Y axis
  int yVal = gamepad.getYCentered(); // −100 … +100
  float tswY = notchY.hasPositions() ? notchY.mapToTSW(yVal) : yVal / 100.0f;

  lastSentTime = now;

#if TRACE_API_CALL
  TRACE_PRINT("[%lu ms] GamePad => x: %d  y: %d BTN: %d\n", now, gamepad.getXCentered(), gamepad.getYCentered(), gamepad.isPressed() ? 1 : 0);
#endif

  // Button
  float btnVal = buttonNotches.mapToTSW(gamepad.isPressed() ? 1.0f : 0.0f);

  spider->setControllerValue(controllerX, tswX);
  spider->setControllerValue(controllerY, tswY);
  spider->setControllerValue(controllerButton, btnVal);
}
