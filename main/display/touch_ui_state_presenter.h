#ifndef TOUCH_UI_STATE_PRESENTER_H
#define TOUCH_UI_STATE_PRESENTER_H

#include "device_state.h"

#include <string>

enum class TouchUiPageTarget {
    kNone,
    kMainGrid,
    kWifiSetup,
    kWifiConnect,
};

enum class TouchUiMicVisual {
    kIdleMicrophone,
    kIdleAi,
    kListening,
    kSpeaking,
    kWaiting,
};

TouchUiPageTarget GetTouchUiPageTarget(DeviceState old_state, DeviceState new_state);
int GetNextWifiConnectAttempts(int current_attempts, DeviceState new_state);
TouchUiMicVisual GetTouchUiMicVisual(DeviceState state, const std::string& emotion);
bool ShouldOpenTouchUiChatForRole(const char* role);
bool IsUsableWifiSsid(const std::string& ssid);
bool CanConnectToWifiSsid(bool scan_has_results, const std::string& ssid);

#endif // TOUCH_UI_STATE_PRESENTER_H
