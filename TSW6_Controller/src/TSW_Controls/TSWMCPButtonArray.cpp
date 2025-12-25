/**
 * @file TSWMCPButtonArray.cpp
 * @brief Implementation of the TSWMCPButtonArray.
 */

#include "TSWMCPButtonArray.h"
#include "../config.h"

TSWMCPButtonArray::TSWMCPButtonArray(const String &id,
                                     TSWSpider *s,
                                     unsigned int debounceMs,
                                     unsigned long sendIntMs)
    : TSWControl(id, s),
      buttonArray(id + "_HW", debounceMs),
      lastSentTimeMs(0),
      sendIntervalMs(sendIntMs)
{
    // mappings[] default-constructs to "disabled" (controller="")
}

void TSWMCPButtonArray::begin()
{
    buttonArray.begin();
}

void TSWMCPButtonArray::setMapping(uint8_t index,
                                   const String &controller,
                                   float pressedValue,
                                   float releasedValue)
{
    if (index >= TOTAL_BUTTONS)
        return;

    mappings[index].controller = controller;
    mappings[index].pressedValue = pressedValue;
    mappings[index].releasedValue = releasedValue;
}

void TSWMCPButtonArray::setDefaultControllerPrefix(const String &prefix,
                                                   float pressedValue,
                                                   float releasedValue)
{
    for (uint8_t i = 0; i < TOTAL_BUTTONS; i++)
    {
        char buf[16];
        snprintf(buf, sizeof(buf), "%02u", (unsigned)i);
        setMapping(i, prefix + String(buf), pressedValue, releasedValue);
    }
}

void TSWMCPButtonArray::updateBtn(uint8_t index)
{
  LOG_HW_TRACE("TSWMCPButtonArray::updateBtn(%u)\n", index);
  if (index >= TOTAL_BUTTONS) return;

    const String& controller = mappings[index].controller;
    if (controller.length() == 0) return;

    // Read the state from the same snapshot that changedPins refers to
    const uint32_t mask = (1UL << index);
    const bool currentStateHigh = (buttonArray.getLastReading() & mask) != 0;

    // MCP + pull-up: LOW = pressed
    const bool pressed = !currentStateHigh;

    const float value = pressed ? mappings[index].pressedValue
                                : mappings[index].releasedValue;

    // IMPORTANT: controller must be used
    spider->setControllerValue(controller, value);
}

void TSWMCPButtonArray::updateAndSend()
{
     if (!spider) return;

    if (!buttonArray.update()) return;

    LOG_HW_DEBUG("MCPButtonArray detected changes\n");

    const unsigned long now = millis();
    LOG_HW_TRACE("  now=%lu lastSent=%lu interval=%lu\n", now, lastSentTimeMs, sendIntervalMs);
    if (sendIntervalMs > 0 && (now - lastSentTimeMs) < sendIntervalMs) return;

    uint32_t pending = buttonArray.getChangedPins();
    LOG_HW_TRACE("  pending changes bitmask=0x%08X\n", pending);

    while (pending) {
        const uint8_t bit = __builtin_ctz(pending);
        LOG_HW_TRACE("  Sending update for button index %u\n", bit);
        updateBtn(bit);
        pending &= ~(1UL << bit);
        LOG_HW_TRACE("  remaining pending bitmask=0x%08X\n", pending);
    }

    lastSentTimeMs = now;
}
