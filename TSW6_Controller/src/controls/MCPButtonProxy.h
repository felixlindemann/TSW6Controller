#pragma once
#include "Control.h"
#include "MCPButtonArray.h"

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
