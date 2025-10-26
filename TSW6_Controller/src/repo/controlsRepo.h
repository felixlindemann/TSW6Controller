#pragma once
#include <Arduino.h>
#include <vector>
#include "../controls/Control.h"

class ControlRegistry
{
public:
  struct Entry
  {
    String id;
    const char *type;
    Control *instance;
  };

private:
  static std::vector<Entry> controls;

public:
  static bool registerControl(Control *c, const char *type);
  static Control *find(const String &id);
  static void listAll();

  static const std::vector<Entry> &getAll();
};
