/**
 * @file MCPButtonArray.cpp
 * @brief Stable MCP23S17 button array handler with debouncing and trace throttling.
 *
 * @details
 *  - Supports multiple MCP23S17 expanders via SPI.
 *  - Each input uses software debouncing.
 *  - Filters noise from unconnected inputs.
 *  - TRACE output only on real state changes (not noise).
 */
/**
 * @file MCPButtonArray.cpp
 * @brief Stable MCP23S17 button array handler with debouncing and trace throttling.
 */

#include "repo/controlsRepo.h"
#include "MCPButtonArray.h"
#include "MCPButtonProxy.h"

#define MIN_DEBOUNCE_MS 30
#define TRACE_THROTTLE_MS 150

MCPButtonArray::MCPButtonArray(const String &idPrefix, unsigned int debounce)
    : Control(idPrefix, 0),
      debounceDelay(debounce),
      lastEventIndex(-1),
      lastPollTime(0)
{
    expanders = new Adafruit_MCP23X17[expanderCount];
    memset(states, HIGH, sizeof(states));
    memset(readings, HIGH, sizeof(readings));
    memset(debounceTimes, 0, sizeof(debounceTimes));
}

void MCPButtonArray::begin()
{
    SPI.begin(18, 19, 23, csPins[0]); // explicit pins for ESP32

#if TRACE
    Serial.println("[DBG] SPI configuration before MCP init:");
    Serial.printf("  SCK: %d\n  MISO: %d\n  MOSI: %d\n", 18, 19, 23);
#endif

    pinMode(mcpReset, OUTPUT);
    digitalWrite(mcpReset, HIGH);
    delay(2);
    reset();

    for (uint8_t i = 0; i < expanderCount; i++)
    {
        if (!expanders[i].begin_SPI(csPins[i]))
        {
            Serial.printf("[ERROR] MCP23S17 #%u failed at CS=%u\n", i, csPins[i]);
            continue;
        }

        Serial.printf("[OK] MCP23S17 #%u init success (CS=%u)\n", i, csPins[i]);
        for (uint8_t p = 0; p < 16; p++)
            expanders[i].pinMode(p, INPUT_PULLUP);

        uint16_t pins = expanders[i].readGPIOAB();
        Serial.printf("[CHECK] MCP23S17 #%u GPIOAB=0x%04X (expected 0xFFFF)\n", i, pins);
    }

    unsigned long now = millis();
    for (int i = 0; i < expanderCount * 16; i++)
    {
        debounceTimes[i] = now;
        readings[i] = HIGH;
        states[i] = HIGH;
    }

    ControlRegistry::registerControl(this, "MCPButtonArray");

    for (uint8_t i = 0; i < expanderCount * 16; i++)
    {
        String id = getButtonId(i);
        auto *proxy = new MCPButtonProxy(id, this, i);
        ControlRegistry::registerControl(proxy, "MCPButton");
    }
}

bool MCPButtonArray::update()
{
    unsigned long now = millis();
    if (now - lastPollTime < 20)
        return false; // max 50 Hz polling
    lastPollTime = now;

    bool changed = false;
    lastChangeReason = "none";
    lastEventIndex = -1;

    for (uint8_t e = 0; e < expanderCount; e++)
    {
        uint16_t pins = expanders[e].readGPIOAB();

        for (uint8_t p = 0; p < 16; p++)
        {
            uint8_t index = e * 16 + p;
            bool reading = bitRead(pins, p);

            if (reading != readings[index])
            {
                debounceTimes[index] = now;
                readings[index] = reading;
            }

            if ((now - debounceTimes[index]) > debounceDelay)
            {
                if (states[index] != readings[index])
                {
                    states[index] = readings[index];
                    lastEventIndex = index;
                    lastChangeReason = (states[index] == LOW) ? "pressed" : "released";
                    changed = true;

#if TRACE
                    static unsigned long lastTrace[64] = {0};
                    if (now - lastTrace[index] > TRACE_THROTTLE_MS)
                    {
                        TRACE_PRINT("[%lu ms] MCPButtonArray %s [%02d] %s\n",
                                    now, getId().c_str(), index, lastChangeReason);
                        lastTrace[index] = now;
                    }
#endif
                }
            }
        }
    }
    return changed;
}

void MCPButtonArray::reset()
{
    digitalWrite(mcpReset, LOW);
    delay(2);
    digitalWrite(mcpReset, HIGH);
    delay(2);
}

float MCPButtonArray::getValue() const
{
    if (lastEventIndex < 0)
        return -1.0f;
    return (states[lastEventIndex] == LOW) ? 1.0f : 0.0f;
}

bool MCPButtonArray::getButtonState(uint8_t index) const
{
    if (index >= expanderCount * 16)
        return false;
    return (states[index] == LOW);
}

String MCPButtonArray::getButtonId(uint8_t index) const
{
    char buf[12];
    snprintf(buf, sizeof(buf), "%s_%02u", getId().c_str(), index + 1);
    return String(buf);
}
