#include "controlsRepo.h"

std::vector<ControlRegistry::Entry> ControlRegistry::controls;

bool ControlRegistry::registerControl(Control *c, const char *type)
{
    if (!c)
        return false;

    for (auto &e : controls)
    {
        if (e.id == c->getId())
        {
            e.instance = c;
            e.type = type;
            LOG_SW_DEBUG("Updated Control: %s [%s]\n",
                          c->getId().c_str(), type);
            return true;
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
    LOG_SW_INFO("--- Registered Controls ---\n");
    for (auto &e : controls)
        LOG_SW_INFO("%s [%s]\n", e.id.c_str(), e.type.c_str());
}
