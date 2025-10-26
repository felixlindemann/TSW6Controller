#include "controlsRepo.h"

std::vector<ControlRegistry::Entry> ControlRegistry::controls;

bool ControlRegistry::registerControl(Control *c, const char *type)
{
  for (auto &e : controls)
  {
    if (e.id == c->getId())
    {
      Serial.printf("[WARN] Duplicate Control ID: %s\n", c->getId().c_str());
      return false;
    }
  }
  controls.push_back({c->getId(), type, c});
  return true;
}

Control *ControlRegistry::find(const String &id)
{
  for (auto &e : controls)
    if (e.id == id)
      return e.instance;
  return nullptr;
}
const std::vector<ControlRegistry::Entry> &ControlRegistry::getAll()
{
  return controls;
}

void ControlRegistry::listAll()
{
  Serial.println("--- Registered Controls ---");
  for (auto &e : controls)
    Serial.printf("%s [%s]\n", e.id.c_str(), e.type);
}
