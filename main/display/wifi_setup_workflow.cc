#include "wifi_setup_workflow.h"

#include "ssid_manager.h"
#include "wifi_manager.h"

#include <algorithm>
#include <cstring>
#include <esp_wifi.h>

WifiSetupWorkflow::~WifiSetupWorkflow() {
    UnregisterScanDoneHandler();
}

esp_err_t WifiSetupWorkflow::RegisterScanDoneHandler(esp_event_handler_t handler, void* arg) {
    if (scan_handler_registered_) {
        return ESP_OK;
    }

    auto ret = esp_event_handler_register(WIFI_EVENT, WIFI_EVENT_SCAN_DONE, handler, arg);
    if (ret == ESP_OK) {
        scan_handler_registered_ = true;
        scan_done_handler_ = handler;
    }
    return ret;
}

void WifiSetupWorkflow::UnregisterScanDoneHandler() {
    if (!scan_handler_registered_ || scan_done_handler_ == nullptr) {
        return;
    }

    esp_event_handler_unregister(WIFI_EVENT, WIFI_EVENT_SCAN_DONE, scan_done_handler_);
    scan_handler_registered_ = false;
    scan_done_handler_ = nullptr;
}

esp_err_t WifiSetupWorkflow::StartScan() {
    wifi_scan_config_t scan_config = {};
    scan_config.show_hidden = false;
    scan_config.scan_type = WIFI_SCAN_TYPE_ACTIVE;
    return esp_wifi_scan_start(&scan_config, false);
}

bool WifiSetupWorkflow::GetScannedSsids(std::vector<std::string>& ssids) {
    ssids.clear();

    uint16_t ap_num = 0;
    if (esp_wifi_scan_get_ap_num(&ap_num) != ESP_OK || ap_num == 0) {
        return false;
    }

    std::vector<wifi_ap_record_t> ap_records(ap_num);
    if (esp_wifi_scan_get_ap_records(&ap_num, ap_records.data()) != ESP_OK) {
        return false;
    }

    for (int i = 0; i < ap_num; i++) {
        std::string ssid = reinterpret_cast<char*>(ap_records[i].ssid);
        if (!ssid.empty() && std::find(ssids.begin(), ssids.end(), ssid) == ssids.end()) {
            ssids.push_back(ssid);
        }
    }

    return !ssids.empty();
}

void WifiSetupWorkflow::SaveCredentialsAndReconnect(const char* ssid, const char* password) {
    SsidManager::GetInstance().AddSsid(ssid ? ssid : "", password ? password : "");

    auto& wifi_manager = WifiManager::GetInstance();
    if (wifi_manager.IsConfigMode()) {
        wifi_manager.StopConfigAp();
        return;
    }

    wifi_manager.StartStation();
}
