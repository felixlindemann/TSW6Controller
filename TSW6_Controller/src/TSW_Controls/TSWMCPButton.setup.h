#include "../config.h"
#include "../controls/Control.h"
#include "../repo/controlsRepo.h"

#if USE_MCPBUTTONARRAY

#include "../controls/MCPButtonArray.h"
#include "../controls/MCPButtonProxy.h"
#include "TSW_Controls/TSWMCPButton.h"

static constexpr uint8_t MCP_CS_PINS[] = PIN_EXPANDERS;
static constexpr uint8_t MCP_RESET_PIN = PIN_EXPANDERSRESET;
inline void setupMCPButtonArray(TSWSpider *spider)
{

    static MCPButtonArray mcpButtons("BTN");

    mcpButtons.begin(); // registriert sich selbst und seine Buttons

    for (int i = 0; i < TOTAL_BUTTONS; ++i)
    {
        String id = "BTN_" + String(i + 1);
        MCPButtonProxy *proxy = (MCPButtonProxy *)ControlRegistry::find(id);

        if (!proxy)
            continue;

        String ctrl = "Button_" + String(i + 1);
        static TSWMCPButton btn(proxy, ctrl, spider);
        ControlRegistry::registerControl(&btn, "TSWMCPButton");
    }
}

#define SETUP_MCPButtonArray(spiderPtr) setupMCPButtonArray(spiderPtr)

#else
#define SETUP_MCPButtonArray(...)
#endif