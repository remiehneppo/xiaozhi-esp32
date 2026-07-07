#include "touch_ui_state_presenter.h"

#include <algorithm>
#include <cctype>
#include <cstring>

TouchUiPageTarget GetTouchUiPageTarget(DeviceState old_state, DeviceState new_state) {
    if (new_state == kDeviceStateStarting || new_state == kDeviceStateConnecting) {
        return TouchUiPageTarget::kWifiConnect;
    }
    if (new_state == kDeviceStateWifiConfiguring) {
        return TouchUiPageTarget::kWifiSetup;
    }
    if ((old_state == kDeviceStateWifiConfiguring || old_state == kDeviceStateConnecting || old_state == kDeviceStateStarting) &&
        (new_state == kDeviceStateIdle || new_state == kDeviceStateListening || new_state == kDeviceStateSpeaking)) {
        return TouchUiPageTarget::kMainGrid;
    }
    return TouchUiPageTarget::kNone;
}

int GetNextWifiConnectAttempts(int current_attempts, DeviceState new_state) {
    if (new_state == kDeviceStateConnecting) {
        return current_attempts + 1;
    }
    if (new_state == kDeviceStateIdle || new_state == kDeviceStateSpeaking) {
        return 0;
    }
    return current_attempts;
}

TouchUiMicVisual GetTouchUiMicVisual(DeviceState state, const std::string& emotion) {
    switch (state) {
        case kDeviceStateIdle:
            return emotion == "microchip_ai" ? TouchUiMicVisual::kIdleAi : TouchUiMicVisual::kIdleMicrophone;
        case kDeviceStateListening:
            return TouchUiMicVisual::kListening;
        case kDeviceStateSpeaking:
            return TouchUiMicVisual::kSpeaking;
        default:
            return TouchUiMicVisual::kWaiting;
    }
}

bool ShouldOpenTouchUiChatForRole(const char* role) {
    return role != nullptr && (std::strcmp(role, "user") == 0 || std::strcmp(role, "assistant") == 0);
}

bool IsUsableWifiSsid(const std::string& ssid) {
    auto has_non_space = std::any_of(ssid.begin(), ssid.end(), [](unsigned char ch) {
        return !std::isspace(ch);
    });
    if (!has_non_space) {
        return false;
    }

    return true;
}

bool CanConnectToWifiSsid(bool scan_has_results, const std::string& ssid) {
    return scan_has_results && IsUsableWifiSsid(ssid);
}
