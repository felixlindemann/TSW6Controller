/**
 * @file MCPButtonProxy.h
 * @brief Proxy class exposing individual MCPButtonArray inputs as Controls.
 *
 * @details
 * Each MCPButtonProxy instance represents a single button channel of an 
 * MCPButtonArray. It delegates all hardware reads to its parent array but 
 * exposes a dedicated Control ID for use in the ControlRegistry and 
 * higher-level logic (e.g. mapping to TSW controls or web UI elements).
 *
 * This design allows all buttons of an expander to be addressed individually
 * while minimizing SPI traffic — the parent array performs shared polling.
 *
 * Example:
 * @code
 *   auto *proxy = new MCPButtonProxy("BTN_05", &mcpArray, 5);
 *   ControlRegistry::registerControl(proxy, "MCPButton");
 * @endcode
 *
 * @see MCPButtonArray
 * @see ControlRegistry
 *
 * @author
 *   Felix Lindemann
 * @date
 *   2025-11-02
 * @version
 *   1.1
 */

#pragma once
#include "Control.h"
#include "MCPButtonArray.h"
#include "../config.h"

class MCPButtonProxy : public Control {
private:
    MCPButtonArray* parent;
    uint8_t index;

public:
    MCPButtonProxy(const String& id, MCPButtonArray* parentArray, uint8_t idx)
        : Control(id, 0), parent(parentArray), index(idx) {}

    void begin() override {}
    bool update() override { return parent->update(); }

    float getValue() const override {
        return parent->getButtonState(index) ? 1.0f : 0.0f;
    }
};
