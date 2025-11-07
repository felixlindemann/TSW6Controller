#pragma once
#include <Arduino.h>
#include <vector>
#include <type_traits>
#include "../controls/Control.h"
#include "../config.h"

class ControlRegistry
{
public:
    struct Entry
    {
        String id;
        String type;
        Control *instance;
    };

private:
    static std::vector<Entry> controls;

public:
    static bool registerControl(Control *c, const char *type);
    static Control *find(const String &id);
    static const std::vector<Entry> &getAll();
    static void listAll();

    // --- template convenience ---
    template <typename T>
    static T *findAs(const String &id)
    {
        Control *base = find(id);
        if (!base)
            return nullptr;
        return dynamic_cast<T *>(base);
    }

    static void clear() { controls.clear(); }
};
