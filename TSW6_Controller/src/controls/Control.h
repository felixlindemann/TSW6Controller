/**
 * @file Control.h
 * @brief Common interface for all hardware input controls.
 *
 * @details
 * Defines a unified base type for all hardware elements (sliders, buttons,
 * rotary encoders, joysticks). Each control has:
 *   - a unique technical identifier (string key)
 *   - a fixed hardware pin assignment
 *   - a standardized lifecycle: begin(), update(), getValue()
 *
 * This interface allows mapping physical controls to logical TSW functions.
 *
 * @example
 *   class MyButton : public Control {
 *     ...
 *   };
 *
 * @note
 *   - Pin assignments are fixed at compile time.
 *   - Logical mapping (TSW target) is defined separately in /TSW_Controls.
 *
 * @author
 *   Felix Lindemann
 * @date
 *   2025-10-28
 */

#pragma once
#include <Arduino.h>

class Control
{
protected:
  String controlId; // technical identifier (e.g. "BTN_SIFA")
  uint8_t pin;      // associated hardware pin
  const char *lastChangeReason = "unknown";

public:
  explicit Control(const String &id, uint8_t gpio)
      : controlId(id), pin(gpio) {}

  virtual ~Control() = default;
  virtual const char *getChangeReason() const { return lastChangeReason; }

  const String &getId() const { return controlId; }
  uint8_t getPin() const { return pin; }

  virtual void begin() = 0;
  virtual bool update() = 0;
  virtual float getValue() const = 0;
};
