# Custom MCP Tool Guide

This guide explains how to add a custom MCP tool for one board. Use this when a board needs to expose a hardware action, sensor reading, setting, or board-specific command to the backend or AI model.

For protocol details, see [`mcp-protocol.md`](./mcp-protocol.md). For general MCP usage, see [`mcp-usage.md`](./mcp-usage.md).

## What You Are Adding

An MCP tool is a C++ callback registered on the device through `McpServer`.

The backend discovers tools with `tools/list`, then invokes one tool with `tools/call`. The firmware receives the call, runs your callback, and returns a result.

Relevant files:

- `main/mcp_server.h` - tool API, parameter types, return types.
- `main/mcp_server.cc` - built-in common tools.
- `main/boards/<board>/<board>.cc` - board-specific registration point.
- `main/boards/common/press_to_talk_mcp_tool.cc` - reusable custom tool example.

## Step 1: Decide the Tool Scope

Choose one of these patterns.

| Pattern | Use When | Where to Put It |
|---------|----------|-----------------|
| Inline board tool | The tool is only useful for one board | `main/boards/<board>/<board>.cc` |
| Reusable board tool | Multiple boards can use the same feature | `main/boards/common/<tool_name>.h/.cc` |
| Common system tool | Every board should always have it | Usually avoid this; do not add custom tools to `McpServer::AddCommonTools()` unless it is truly global |

Recommended default: implement board-specific tools in that board's `InitializeTools()` method.

## Step 2: Find or Create `InitializeTools()`

Open the board implementation file under `main/boards/`.

Example:

```text
main/boards/xmini-c3/xmini_c3_board.cc
main/boards/esp-hi/esp_hi.cc
main/boards/<your-board>/<your-board>.cc
```

If the board already has `InitializeTools()`, add your tool there.

If it does not exist, add a private method:

```cpp
void InitializeTools() {
    auto& mcp_server = McpServer::GetInstance();

    // Register custom board tools here.
}
```

Then call it from the board constructor after hardware initialization:

```cpp
MyBoard() {
    InitializeI2c();
    InitializeDisplay();
    InitializeButtons();
    InitializeTools();
}
```

The tool callback should only use hardware that has already been initialized.

## Step 3: Include the MCP Header

In the board `.cc` file, include:

```cpp
#include "mcp_server.h"
```

Most board files already include it. If not, add it near the other project headers.

## Step 4: Choose a Tool Name

Use a stable, namespaced name:

```text
self.<module>.<action>
```

Examples:

```text
self.light.set_rgb
self.light.get_power
self.motor.stop
self.sensor.get_temperature
self.system.reconfigure_wifi
```

Rules:

- Prefix device-local tools with `self.`.
- Keep names lowercase.
- Use dots for grouping.
- Do not rename a tool after releasing firmware unless you also update the backend or client that calls it.
- Avoid duplicate names. `McpServer::AddTool()` ignores duplicate registrations and logs a warning.

## Step 5: Write a Good Description

The description is part of the schema returned by `tools/list`. The AI uses it to decide when to call the tool.

Bad:

```cpp
"Set LED"
```

Better:

```cpp
"Set the RGB color of the board light. Use this when the user asks to change the light color."
```

For tools with constrained values, describe the allowed values:

```cpp
"Set the button mode. The mode must be `press_to_talk` or `click_to_talk`."
```

## Step 6: Define Input Parameters

Parameters are declared with `PropertyList`.

Supported types:

| Type | C++ Property Type | Read As |
|------|-------------------|---------|
| Boolean | `kPropertyTypeBoolean` | `value<bool>()` |
| Integer | `kPropertyTypeInteger` | `value<int>()` |
| String | `kPropertyTypeString` | `value<std::string>()` |

No arguments:

```cpp
PropertyList()
```

Required string:

```cpp
PropertyList({
    Property("mode", kPropertyTypeString)
})
```

Required integer with range:

```cpp
PropertyList({
    Property("brightness", kPropertyTypeInteger, 0, 100)
})
```

Optional integer with default and range:

```cpp
PropertyList({
    Property("brightness", kPropertyTypeInteger, 50, 0, 100)
})
```

Optional boolean with default:

```cpp
PropertyList({
    Property("enabled", kPropertyTypeBoolean, true)
})
```

A property without a default value is required. A property with a default value is optional.

## Step 7: Implement the Callback

Callbacks use this signature:

```cpp
[](const PropertyList& properties) -> ReturnValue {
    return true;
}
```

If the callback needs board members, capture `this`:

```cpp
[this](const PropertyList& properties) -> ReturnValue {
    SetLedColor(255, 255, 255);
    return true;
}
```

Read arguments by name:

```cpp
int brightness = properties["brightness"].value<int>();
std::string mode = properties["mode"].value<std::string>();
bool enabled = properties["enabled"].value<bool>();
```

Supported return values:

| Return Type | Example |
|-------------|---------|
| `bool` | `return true;` |
| `int` | `return 42;` |
| `std::string` | `return "ok";` |
| `cJSON*` | `return json;` |
| `ImageContent*` | `return new ImageContent("image/jpeg", data);` |

Use `bool` for simple success or failure. Use `std::string` for short messages. Use `cJSON*` for structured state.

## Step 8: Register a Regular AI-Callable Tool

Example: set RGB color.

```cpp
void InitializeTools() {
    auto& mcp_server = McpServer::GetInstance();

    mcp_server.AddTool("self.light.set_rgb",
        "Set the RGB color of the board light. Values must be between 0 and 255.",
        PropertyList({
            Property("r", kPropertyTypeInteger, 0, 255),
            Property("g", kPropertyTypeInteger, 0, 255),
            Property("b", kPropertyTypeInteger, 0, 255)
        }),
        [this](const PropertyList& properties) -> ReturnValue {
            int r = properties["r"].value<int>();
            int g = properties["g"].value<int>();
            int b = properties["b"].value<int>();

            SetLedColor(r, g, b);
            return true;
        });
}
```

Use `AddTool()` when it is safe for the AI model to call the tool after user intent is understood.

Good candidates:

- Light control.
- Volume or brightness control.
- Robot movement.
- Reading sensor status.
- Switching a harmless local mode.

## Step 9: Register a User-Only Tool When Needed

Use `AddUserOnlyTool()` for privileged or app-only operations.

Example:

```cpp
void InitializeTools() {
    auto& mcp_server = McpServer::GetInstance();

    mcp_server.AddUserOnlyTool("self.storage.clear_cache",
        "Clear local cached files. User-only action.",
        PropertyList(),
        [](const PropertyList&) -> ReturnValue {
            ClearLocalCache();
            return true;
        });
}
```

Use user-only tools for actions like:

- Reboot.
- Firmware upgrade.
- Factory reset.
- Credential or URL changes.
- File deletion.
- Actions that should only happen from a companion app or explicit local UI.

User-only tools are hidden from the default AI-facing `tools/list`. The backend must request user tools explicitly.

## Step 10: Validate Input and Report Errors

Integer min/max ranges are validated by `Property`.

For string enums or business rules, validate manually:

```cpp
mcp_server.AddTool("self.button.set_mode",
    "Set the button mode. The mode must be `press_to_talk` or `click_to_talk`.",
    PropertyList({
        Property("mode", kPropertyTypeString)
    }),
    [this](const PropertyList& properties) -> ReturnValue {
        auto mode = properties["mode"].value<std::string>();

        if (mode == "press_to_talk") {
            SetPressToTalkEnabled(true);
            return true;
        }

        if (mode == "click_to_talk") {
            SetPressToTalkEnabled(false);
            return true;
        }

        throw std::runtime_error("Invalid mode: " + mode);
    });
```

Throw `std::runtime_error` when the caller passed invalid input or the action cannot be completed.

## Step 11: Return Structured JSON When Useful

Use `cJSON*` when the caller needs multiple fields.

Example:

```cpp
mcp_server.AddTool("self.sensor.get_status",
    "Get current sensor status.",
    PropertyList(),
    [this](const PropertyList&) -> ReturnValue {
        cJSON* json = cJSON_CreateObject();
        cJSON_AddNumberToObject(json, "temperature", GetTemperature());
        cJSON_AddNumberToObject(json, "humidity", GetHumidity());
        cJSON_AddBoolToObject(json, "ready", IsSensorReady());
        return json;
    });
```

The MCP server takes ownership of the returned `cJSON*` and deletes it after converting it to text.

## Step 12: Use `Application::Schedule()` for Slow or Disruptive Work

Do not block the MCP callback for long operations.

Use scheduling for reboot, firmware upgrade, long hardware actions, or operations that must run on the application task:

```cpp
mcp_server.AddUserOnlyTool("self.device.restart_service",
    "Restart a local service. User-only action.",
    PropertyList(),
    [](const PropertyList&) -> ReturnValue {
        auto& app = Application::GetInstance();
        app.Schedule([]() {
            RestartLocalService();
        });
        return true;
    });
```

If the action is quick, direct execution inside the callback is fine.

## Step 13: Reusable Tool Class Pattern

Use this pattern when more than one board should share the tool.

Header:

```cpp
#ifndef MY_MCP_TOOL_H
#define MY_MCP_TOOL_H

#include "mcp_server.h"

class MyMcpTool {
public:
    void Initialize();

private:
    ReturnValue HandleSetMode(const PropertyList& properties);
};

#endif
```

Implementation:

```cpp
#include "my_mcp_tool.h"

void MyMcpTool::Initialize() {
    auto& mcp_server = McpServer::GetInstance();

    mcp_server.AddTool("self.my_tool.set_mode",
        "Set custom mode.",
        PropertyList({
            Property("mode", kPropertyTypeString)
        }),
        [this](const PropertyList& properties) -> ReturnValue {
            return HandleSetMode(properties);
        });
}

ReturnValue MyMcpTool::HandleSetMode(const PropertyList& properties) {
    auto mode = properties["mode"].value<std::string>();
    return mode;
}
```

Board usage:

```cpp
#include "my_mcp_tool.h"

class MyBoard : public WifiBoard {
private:
    MyMcpTool* my_mcp_tool_ = nullptr;

    void InitializeTools() {
        my_mcp_tool_ = new MyMcpTool();
        my_mcp_tool_->Initialize();
    }
};
```

Follow the existing `PressToTalkMcpTool` implementation for a complete example.

## Step 14: Build

Build the firmware:

```bash
idf.py build
```

Fix any compile errors before flashing or pushing.

Common errors:

| Error | Cause | Fix |
|-------|-------|-----|
| `McpServer` not declared | Missing include | Add `#include "mcp_server.h"` |
| `Property` not declared | Missing include | Add `#include "mcp_server.h"` |
| `std::string` not available | Missing include in standalone file | Add `#include <string>` |
| Link error for helper method | Declared a method but did not define it | Add the method implementation |
| Duplicate tool warning | Tool registered more than once | Use a unique name or register only once |

## Step 15: Test With JSON-RPC

After the device connects to the backend, list tools:

```json
{
  "jsonrpc": "2.0",
  "method": "tools/list",
  "params": {
    "cursor": "",
    "withUserTools": false
  },
  "id": 1
}
```

Call a regular tool:

```json
{
  "jsonrpc": "2.0",
  "method": "tools/call",
  "params": {
    "name": "self.light.set_rgb",
    "arguments": {
      "r": 255,
      "g": 128,
      "b": 0
    }
  },
  "id": 2
}
```

List user-only tools:

```json
{
  "jsonrpc": "2.0",
  "method": "tools/list",
  "params": {
    "cursor": "",
    "withUserTools": true
  },
  "id": 3
}
```

## Step 16: Implementation Checklist

- Pick the target board file.
- Add `#include "mcp_server.h"` if missing.
- Add or reuse `InitializeTools()`.
- Register with `AddTool()` or `AddUserOnlyTool()`.
- Use a stable `self.<module>.<action>` name.
- Write a clear description.
- Define all parameters with `PropertyList`.
- Validate string enum values manually.
- Return `bool`, `int`, `std::string`, `cJSON*`, or `ImageContent*`.
- Avoid long blocking work inside the callback.
- Call `InitializeTools()` from the board constructor.
- Run `idf.py build`.
- Test `tools/list`.
- Test `tools/call`.

## Minimal Copy-Paste Template

```cpp
#include "mcp_server.h"

void InitializeTools() {
    auto& mcp_server = McpServer::GetInstance();

    mcp_server.AddTool("self.example.set_value",
        "Set an example integer value from 0 to 100.",
        PropertyList({
            Property("value", kPropertyTypeInteger, 0, 100)
        }),
        [this](const PropertyList& properties) -> ReturnValue {
            int value = properties["value"].value<int>();

            SetExampleValue(value);
            return true;
        });
}
```

Constructor:

```cpp
MyBoard() {
    InitializeHardware();
    InitializeTools();
}
```

Replace `self.example.set_value`, the description, parameters, and `SetExampleValue(value)` with your board-specific behavior.
