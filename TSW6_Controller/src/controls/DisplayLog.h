#pragma once
#include <Wire.h>
#include <Arduino.h>
#include "../config.h"

// A simple "scrolling log" for small displays:
// New messages are appended at the bottom, existing lines move up by one.
// Rendering is delegated via a redraw callback.
class DisplayLog
{
public:
    using RedrawCallback = void (*)(const String* lines, uint8_t lineCount);

    DisplayLog(uint8_t lineCount,
               uint16_t maxLineLength,
               RedrawCallback redrawCb);

               void init();
    void begin();
    void clear();

    // Adds a message as a single line (truncated if too long)
    void printLine(const String& message);

    // Adds a message, optionally split into multiple lines (word-wrap-ish)
    // If enableWrap is false, it behaves like printLine().
    void print(const String& message, bool enableWrap);

    // Access for debugging / custom rendering (optional)
    uint8_t getLineCount() const { return _lineCount; }
    const String* getLines() const { return _lines; }

private:
    void pushLineUp();
    void appendLine(const String& line);
    void redraw();

    uint8_t _lineCount;
    uint16_t _maxLineLength;
    RedrawCallback _redrawCb;

    String* _lines;
};
