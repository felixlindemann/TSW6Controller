/**
 * @file TSWMCPButton.h
 * @brief TSW-integrated wrapper for an existing MCPButtonProxy instance.
 *
 * @details
 *  This class links an already registered MCPButtonProxy (from MCPButtonArray)
 *  with the TSWControl system, allowing it to send mapped values to TSWSpider.
 *
 * @example
 *  // Assume MCPButtonArray already created and registered
 *  MCPButtonProxy* btn = ControlRegistry::find<MCPButtonProxy>("BTN_05");
 *  static TSWMCPButton tswBtn(btn, "Sifa", &spider);
 *  tswBtn.updateAndSend();
 *
 * @see MCPButtonProxy
 * @see MCPButtonArray
 * @see TSWControl
 */

#pragma once

#include "TSWControl.h"
#include "../controls/MCPButtonProxy.h"
#include "../controls/Control.h" // wichtig für Mehrfachvererbung
class TSWMCPButton : public Control, public TSWControl
{
private:
    MCPButtonProxy *proxy = nullptr;
    float lastSentValue = -1.0f;

public:
    TSWMCPButton(MCPButtonProxy *proxy,
                 const String &controllerName,
                 TSWSpider *spider)
        : Control(controllerName, 0),
          TSWControl(controllerName, spider),
          proxy(proxy)
    {
        if (!proxy)
        {
            Serial.println("[ERR] TSWMCPButton created with null proxy!");
            return;
        }

        Notch released = {"Released", 0.0f, 0, 0};
        Notch pressed = {"Pressed", 1.0f, 1, 1};
        notches.loadFromArray({released, pressed});
    }

    // === required overrides from Control ===
    void begin() override {} // nothing to init explicitly

    bool update() override
    {
        updateAndSend(); // delegate to existing logic
        return true;
    }

    float getValue() const override
    {
        return proxy ? proxy->getValue() : 0.0f;
    }
    // =======================================

    void updateAndSend()
    {
        if (!proxy || !spider)
            return;

        float value = proxy->getValue();
        float mapped = notches.mapToTSW(value > 0.5f ? 100 : 0);

        if (fabs(mapped - lastSentValue) > 0.001f)
        {
            spider->setControllerValue(controllerName, mapped);
            lastSentValue = mapped;
            TRACE_PRINT("[TSW] %s -> %.2f\n",
                        controllerName.c_str(), mapped);
        }
    }

    MCPButtonProxy *getProxy() const { return proxy; }
};
