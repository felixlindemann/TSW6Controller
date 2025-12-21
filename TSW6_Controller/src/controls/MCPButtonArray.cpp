/**
 * @file MCPButtonArray.cpp
 * @brief Stable MCP23S17 button array handler with debouncing and trace throttling.
 */

#include "repo/controlsRepo.h"
#include "MCPButtonArray.h"
#include "MCPButtonProxy.h"

#ifndef TRACE_THROTTLE_MS
#define TRACE_THROTTLE_MS 150
#endif

MCPButtonArray::MCPButtonArray(const String &idPrefix, unsigned int debounceMs)
    : Control(idPrefix, 0),
      mcp1(spiPins[3], MCP_HW_ADDRESS_1),
      mcp2(spiPins[3], MCP_HW_ADDRESS_2),
      debounceDelayMs(debounceMs),
      lastEventIndex(-1),
      lastPollTimeMs(0)
{
    // Wire expander pointer array
#if (NUM_OF_EXPANDERS >= 1)
    expanders[0] = &mcp1;
#endif
#if (NUM_OF_EXPANDERS >= 2)
    expanders[1] = &mcp2;
#endif

    // Init state arrays: default = released (HIGH)
    for (uint16_t i = 0; i < TOTAL_BUTTONS; i++)
    {
        states[i] = HIGH;
        readings[i] = HIGH;
        debounceTimes[i] = 0;
    }
}

void MCPButtonArray::haltWithMcpError(uint8_t chipIndex, const char *msg)
{
    Serial.printf("[ERROR] MCP23S17 #%u: %s\n", chipIndex, msg);
    Serial.printf("        SPI pins: SCK=%u MISO=%u MOSI=%u CS=%u\n",
                  spiPins[0], spiPins[1], spiPins[2], spiPins[3]);
    Serial.println("        PLEASE CHECK THE HARDWARE CONNECTIONS / ADDRESS PINS.");
    while (true)
    {
        delay(50);
    }
}

void MCPButtonArray::initExpanderOrHalt(MCP23S17 &mcp, uint8_t chipIndex)
{
    // NOTE: You used begin(false) already. Keep it as-is.
    if (!mcp.begin(false))
    {
        haltWithMcpError(chipIndex, "begin(false) failed");
    }
    if (!mcp.enableHardwareAddress())
    {
        haltWithMcpError(chipIndex, "enableHardwareAddress() failed");
    }

    if (!mcp.pinMode16(0xFFFF))
    {
        haltWithMcpError(chipIndex, "pinMode16(0xFFFF) failed");
    }

    if (!mcp.setPullup16(0xFFFF))
    {
        haltWithMcpError(chipIndex, "setPullup16(0xFFFF) failed");
    }

    // Sanity check: with pullups enabled and nothing pressed, read should be 0xFFFF
    // (If you hold a button during boot, this can be different — then this check will trip.)
    const uint16_t expected = 0xFFFF;
    const uint16_t observed = mcp.read16();
    if (observed != expected)
    {
        char buf[128];
        snprintf(buf, sizeof(buf),
                 "hardware sanity check failed: read16=0x%04X (expected 0x%04X). "
                 "Are buttons pressed during boot? Are inputs floating? Addressing wrong?",
                 observed, expected);
        haltWithMcpError(chipIndex, buf);
    }

#if TRACE
    Serial.printf("[OK] MCP23S17 #%u init OK (read16=0x%04X)\n", chipIndex, observed);
#endif
}

void MCPButtonArray::begin()
{
    // Explicit SPI pins for ESP32
    SPI.begin(spiPins[0], spiPins[1], spiPins[2], spiPins[3]);

#if TRACE
    Serial.println("[DBG] SPI configuration before MCP init:");
    Serial.printf("      SCK:  %u\n      MISO: %u\n      MOSI: %u\n      CS:   %u\n",
                  spiPins[0], spiPins[1], spiPins[2], spiPins[3]);
#endif

    // Init each configured expander
    for (uint8_t e = 0; e < expanderCount; e++)
    {
        initExpanderOrHalt(*expanders[e], (uint8_t)(e + 1));
    }

    // Initialize debounce timers
    const unsigned long now = millis();
    for (uint16_t i = 0; i < TOTAL_BUTTONS; i++)
    {
        debounceTimes[i] = now;
        readings[i] = HIGH;
        states[i] = HIGH;
    }

Serial.printf("MCP1 addr=%u read16=0x%04X\n", mcp1.getAddress(), mcp1.read16());
Serial.printf("MCP2 addr=%u read16=0x%04X\n", mcp2.getAddress(), mcp2.read16());

    // Register this array control
    ControlRegistry::registerControl(this, "MCPButtonArray");

    // Create and register proxies (one per button)
    for (uint8_t i = 0; i < TOTAL_BUTTONS; i++)
    {
        String id = getButtonId(i);
        auto *proxy = new MCPButtonProxy(id, this, i);
        ControlRegistry::registerControl(proxy, "MCPButton");
    }
}

bool MCPButtonArray::update()
{
    const unsigned long now = millis();

    // Poll at max 50 Hz
    if (now - lastPollTimeMs < 20)
        return false;
    lastPollTimeMs = now;

    bool changed = false;
    lastChangeReason = "none";
    lastEventIndex = -1;

#if TRACE
    static unsigned long lastTrace[TOTAL_BUTTONS] = {0};
#endif

    for (uint8_t e = 0; e < expanderCount; e++)
    {
        // Read all 16 inputs in one shot
        const uint16_t pins = expanders[e]->read16();

        for (uint8_t p = 0; p < BUTTONS_PER_EXPANDER; p++)
        {
            const uint16_t index = (uint16_t)e * BUTTONS_PER_EXPANDER + p;
            const bool raw = (pins & (1u << p)) ? HIGH : LOW;

            // Raw edge -> reset debounce timer
            if (raw != readings[index])
            {
                readings[index] = raw;
                debounceTimes[index] = now;
            }

            // Debounced state update
            if ((now - debounceTimes[index]) >= debounceDelayMs)
            {
                if (states[index] != readings[index])
                {
                    states[index] = readings[index];
                    lastEventIndex = (int)index;
                    lastChangeReason = (states[index] == LOW) ? "pressed" : "released";
                    changed = true;

#if TRACE
                    if (now - lastTrace[index] > TRACE_THROTTLE_MS)
                    {
                        TRACE_PRINT("[%lu ms] %s [%02u] %s\n",
                                    now, getId().c_str(), (unsigned)index, lastChangeReason);
                        lastTrace[index] = now;
                    }
#endif
                }
            }
        }
    }

    return changed;
}

float MCPButtonArray::getValue() const
{
    if (lastEventIndex < 0)
        return -1.0f;

    return (states[lastEventIndex] == LOW) ? 1.0f : 0.0f;
}

bool MCPButtonArray::getButtonState(uint8_t index) const
{
    if (index >= TOTAL_BUTTONS)
        return false;

    return (states[index] == LOW);
}

String MCPButtonArray::getButtonId(uint8_t index) const
{
    // BTN_01, BTN_02, ...
    char buf[16];
    snprintf(buf, sizeof(buf), "%s_%02u", getId().c_str(), (unsigned)(index + 1));
    return String(buf);
}
