
#include "repo/controlsRepo.h"   // dein ControlRegistry
#include "MCPButtonArray.h"
#include "MCPButtonProxy.h"
MCPButtonArray::MCPButtonArray(const String &idPrefix,
                               uint8_t expCount,
                               uint8_t mcpReset,
                               uint8_t csBase,
                               unsigned int debounce)
    : Control(idPrefix, csBase),
      mcpReset(mcpReset),
      expanderCount(expCount),
      csBase(csBase),
      debounceDelay(debounce),
      lastEventIndex(-1)
{
    expanders = new Adafruit_MCP23X17[expanderCount];
    memset(states, HIGH, sizeof(states));
    memset(readings, HIGH, sizeof(readings));
    memset(debounceTimes, 0, sizeof(debounceTimes));
}


void MCPButtonArray::begin() {
    SPI.begin();

    pinMode(mcpReset, OUTPUT);
    digitalWrite(mcpReset, HIGH);
    delay(2);
    reset();

    for (uint8_t i = 0; i < expanderCount; i++) {
        if (!expanders[i].begin_SPI(csBase + i)) {
            Serial.printf("[ERROR] MCP23S17 #%u init failed\n", i);
            continue;
        }
        for (uint8_t p = 0; p < 16; p++) {
            expanders[i].pinMode(p, INPUT_PULLUP);
        }
        Serial.printf("[INIT] MCP23S17 #%u ready\n", i);
    }

    // Initialzustände setzen
    unsigned long now = millis();
    for (int i = 0; i < expanderCount * 16; i++) {
        debounceTimes[i] = now;
        readings[i] = HIGH;
        states[i] = HIGH;
    }

    // === Neue Registrierung im ControlRegistry ===
    for (uint8_t i = 0; i < expanderCount * 16; i++) {
        String id = getButtonId(i);
        auto* proxy = new MCPButtonProxy(id, this, i);
        ControlRegistry::registerControl(proxy, "MCPButton");
    }
}
/*
void MCPButtonArray::begin()
{
    SPI.begin();
    pinMode(mcpReset, OUTPUT);
    digitalWrite(mcpReset, HIGH); // normaler Betrieb
    delay(2);
    reset();

    for (uint8_t i = 0; i < expanderCount; i++)
    {
        uint8_t addr = i; // each chip uses a unique HW address
        if (!expanders[i].begin_SPI(csBase + i))
        {
            Serial.printf("[ERROR] MCP23S17 #%u init failed\n", i);
            continue;
        }

        for (uint8_t p = 0; p < 16; p++)
        {
            expanders[i].pinMode(p, INPUT_PULLUP);
        }

        Serial.printf("[INIT] MCP23S17 #%u ready\n", i);
    }

    unsigned long now = millis();
    for (int i = 0; i < expanderCount * 16; i++)
    {
        debounceTimes[i] = now;
        readings[i] = HIGH;
        states[i] = HIGH;
    }
}*/

bool MCPButtonArray::update()
{
    lastChangeReason = "none";
    lastEventIndex = -1;

    bool changed = false;
    unsigned long now = millis();

    for (uint8_t e = 0; e < expanderCount; e++)
    {
        uint16_t pins = expanders[e].readGPIOAB();
        for (uint8_t p = 0; p < 16; p++)
        {
            uint8_t index = e * 16 + p;
            if (index >= sizeof(states))
                continue; // safety guard
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
                }
            }
        }
    }
    return changed;
}

void MCPButtonArray::reset()
{
    digitalWrite(mcpReset, LOW); // Hardware-Reset auslösen
    delay(2);
    digitalWrite(mcpReset, HIGH); // normaler Betrieb
    delay(2);
}
void MCPButtonArray::loop(){
     if (update()) {
#if TRACE
        int idx = getLastEventIndex();
        Serial.printf("[%s] %s\n",
            getButtonId(idx).c_str(),
            getButtonState(idx) ? "PRESSED" : "RELEASED");
        #endif
    }
}

float MCPButtonArray::getValue() const
{
    // Return 1.0 if last change was press, 0.0 if release, -1.0 if none
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
