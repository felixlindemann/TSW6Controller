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
      lastSentTime(0),
      sendInterval(sendInt)
{
  // Store controller names in their respective NotchTables
  notchX.setControllerName(ctrlX);
  notchY.setControllerName(ctrlY);
  buttonNotches.setControllerName(ctrlBtn);
  
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
void TSWGamePadControl::loadNotchesX(const String &filePath) { 
  if (notchX.loadFromFile(filePath)) {
    // Apply inverted setting from config to hardware gamepad X axis
    gamepad.setXInverted(notchX.isInverted());
    LOG_SW_DEBUG("%s: X-axis inverted=%s (from config)\n", 
           getControllerName().c_str(), notchX.isInverted() ? "true" : "false");
  }
}

void TSWGamePadControl::loadNotchesY(const String &filePath) { 
  if (notchY.loadFromFile(filePath)) {
    // Apply inverted setting from config to hardware gamepad Y axis
    gamepad.setYInverted(notchY.isInverted());
    LOG_SW_DEBUG("%s: Y-axis inverted=%s (from config)\n", 
           getControllerName().c_str(), notchY.isInverted() ? "true" : "false");
  }
}

void TSWGamePadControl::loadButtonNotches(const String &filePath) { buttonNotches.loadFromFile(filePath); }
    

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
  
  LOG_HW_TRACE("GamePad => x: %d y: %d BTN: %d\n", 
               gamepad.getXCentered(), gamepad.getYCentered(), gamepad.isPressed() ? 1 : 0);

  // Button
  float btnVal = buttonNotches.mapToTSW(gamepad.isPressed() ? 1.0f : 0.0f);

  spider->setControllerValue(notchX.getControllerName(), tswX);
  spider->setControllerValue(notchY.getControllerName(), tswY);
  spider->setControllerValue(buttonNotches.getControllerName(), btnVal);
}
