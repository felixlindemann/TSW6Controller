#include "../config.h"
#include "../controls/Control.h"
#include "../repo/controlsRepo.h"

#if USE_MCP_BUTTON_ARRAY

#include "TSWMCPButtonArray.h"

static TSWMCPButtonArray* mcpButtonsPtr = nullptr;

inline void setup_MCPButtonArray(TSWSpider* spider)
{
  // Static lifetime like your TSWGamePadControl pattern
  static TSWMCPButtonArray mcpButtons("mcpButtons", spider, 50 /*debounceMs*/, 0 /*sendIntervalMs*/);

  // Option A: auto-prefix mapping (BTN_00..)
  // mcpButtons.setDefaultControllerPrefix("BTN_");

  // Option B: explicit mapping examples
  // mcpButtons.setMapping(0, "Horn", 1.0f, 0.0f);
  // mcpButtons.setMapping(1, "Sander", 1.0f, 0.0f);

  mcpButtons.begin();
  mcpButtonsPtr = &mcpButtons;
}

inline void update_MCPButtonArray()
{
  if (mcpButtonsPtr)
  {
    mcpButtonsPtr->updateAndSend();
  }
}

#define SETUP_MCP_BUTTON_ARRAY(spiderPtr) setup_MCPButtonArray(spiderPtr)
#define UPDATE_MCP_BUTTON_ARRAY() update_MCPButtonArray()

#else
#define SETUP_MCP_BUTTON_ARRAY(...)
#define UPDATE_MCP_BUTTON_ARRAY()
#endif
