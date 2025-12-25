
/**
 * @file TSWMCPButton.h
 * @brief Virtual button that references a single pin on a shared MCPButtonArray.
 *
 * @details
 * This class represents a single button from an MCP23S17 expander array.
 * Instead of each button owning its own MCPButtonArray, all TSWMCPButton
 * instances share a reference to a common MCPButtonArray (hardware layer).
 *
 * This design allows:
 * - All controls to derive from TSWControl
 * - Runtime discovery of all controls for web service configuration
 * - Individual button configuration via NotchTable
 *
 * @author Felix Lindemann
 * @date 2025-12-25
 * @version 1.0
 */

#pragma once

#include <Arduino.h>
#include "TSWControl.h"
#include "NotchTable.h"
#include "../controls/MCPButtonArray.h"

class TSWMCPButton : public TSWControl
{
private:
    MCPButtonArray* buttonArrayRef;  // Shared reference, NOT owned
    uint8_t buttonIndex;
    bool lastState;  // Track state for edge detection

public:
    /**
     * @param id            Logical id for this TSW control (e.g., "Horn", "Sander")
     * @param spider        TSWSpider instance used to send controller values
     * @param buttonArray   Pointer to the shared MCPButtonArray (hardware layer)
     * @param index         Index of the button in the MCPButtonArray (0..TOTAL_BUTTONS-1)
     */
    TSWMCPButton(const String &id,
                 TSWSpider *spider,
                 MCPButtonArray *buttonArray,
                 uint8_t index)
        : TSWControl(id, spider),
          buttonArrayRef(buttonArray),
          buttonIndex(index),
          lastState(false)
    {
        // Setup default binary button mapping (Released=0, Pressed=1)
        notches.setupBinaryButton(id, 0.0f, 1.0f);
    }

    /**
     * Get the button index in the MCPButtonArray
     */
    uint8_t getButtonIndex() const { return buttonIndex; }

    /**
     * Check if this button's state changed in the last update
     */
    bool hasChanged() const {
        if (!buttonArrayRef) return false;
        uint32_t changed = buttonArrayRef->getChangedPins();
        return (changed & (1UL << buttonIndex)) != 0;
    }

    /**
     * Get current button state (true = pressed)
     */
    bool isPressed() const {
        if (!buttonArrayRef) return false;
        return !buttonArrayRef->getButtonState(buttonIndex);  // Inverted: LOW = pressed
    }

    /**
     * Update and send value to TSW if state changed.
     * Call this in the main loop after buttonArrayRef->update().
     */
    void updateAndSend() {
        if (!spider || !buttonArrayRef) return;
        if (!hasChanged()) return;

        // Map button state to TSW value via NotchTable
        // pressed = 100%, released = 0%
        int percent = isPressed() ? 100 : 0;
        float value = notches.mapToTSW(percent);

        sendValueToTSW(value);
        
        LOG_SW_DEBUG("TSWMCPButton[%d] %s: %s -> %.2f\n", 
                     buttonIndex, 
                     controllerName.c_str(),
                     isPressed() ? "PRESSED" : "RELEASED",
                     value);
    }

    /**
     * Configure button mapping with custom values
     */
    void setMapping(float releasedValue = 0.0f, float pressedValue = 1.0f) {
        notches.setupBinaryButton(controllerName, releasedValue, pressedValue);
    }
};
