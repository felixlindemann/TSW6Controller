#include "DisplayLog.h"

DisplayLog::DisplayLog(uint8_t lineCount,
                       uint16_t maxLineLength,
                       RedrawCallback redrawCb)
    : _lineCount(lineCount),
      _maxLineLength(maxLineLength),
      _redrawCb(redrawCb),
      _lines(nullptr)
{
}

void DisplayLog::init()
{
    // extern const int I2C_SDA;
    // extern const int I2C_SCL;
    Wire.setPins(I2C_SDA, I2C_SCL);
    Wire.begin();
}

void DisplayLog::begin()
{
    if (_lineCount == 0)
    {
        return;
    }

    _lines = new String[_lineCount];
    for (uint8_t i = 0; i < _lineCount; i++)
    {
        _lines[i] = "";
        // Helps reduce heap fragmentation when lines change frequently
        _lines[i].reserve(_maxLineLength + 4);
    }
    redraw();
}

void DisplayLog::clear()
{
    if (!_lines)
    {
        return;
    }

    for (uint8_t i = 0; i < _lineCount; i++)
    {
        _lines[i] = "";
    }
    redraw();
}

void DisplayLog::pushLineUp()
{
    for (uint8_t i = 0; i < _lineCount - 1; i++)
    {
        _lines[i] = _lines[i + 1];
    }
    _lines[_lineCount - 1] = "";
}

void DisplayLog::appendLine(const String &line)
{
    if (!_lines)
    {
        return;
    }

    pushLineUp();

    if (line.length() > _maxLineLength)
    {
        _lines[_lineCount - 1] = line.substring(0, _maxLineLength);
    }
    else
    {
        _lines[_lineCount - 1] = line;
    }
}

void DisplayLog::redraw()
{
    if (_redrawCb && _lines)
    {
        _redrawCb(_lines, _lineCount);
    }
}

void DisplayLog::printLine(const String &message)
{
    appendLine(message);
    redraw();
}

void DisplayLog::print(const String &message, bool enableWrap)
{
    if (!enableWrap)
    {
        printLine(message);
        return;
    }

    // Very simple wrapping:
    // - Split by spaces
    // - Build lines up to max length
    String current = "";
    current.reserve(_maxLineLength + 4);

    int start = 0;
    while (start < (int)message.length())
    {
        // Find next space
        int spacePos = message.indexOf(' ', start);
        bool lastToken = (spacePos < 0);

        int end = lastToken ? message.length() : spacePos;
        String token = message.substring(start, end);

        // Move start beyond the space (or end)
        start = lastToken ? message.length() : (spacePos + 1);

        if (token.length() == 0)
        {
            continue;
        }

        // If token itself is longer than max, hard-split it
        while (token.length() > _maxLineLength)
        {
            if (current.length() > 0)
            {
                appendLine(current);
                current = "";
            }
            appendLine(token.substring(0, _maxLineLength));
            token = token.substring(_maxLineLength);
        }

        // Try to append token to current line
        if (current.length() == 0)
        {
            current = token;
        }
        else
        {
            // +1 for the separating space
            if (current.length() + 1 + token.length() <= _maxLineLength)
            {
                current += " ";
                current += token;
            }
            else
            {
                appendLine(current);
                current = token;
            }
        }
    }

    if (current.length() > 0)
    {
        appendLine(current);
    }

    redraw();
}
