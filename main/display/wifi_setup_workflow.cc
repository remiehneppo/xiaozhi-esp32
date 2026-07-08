#include "wifi_setup_workflow.h"

#include "ssid_manager.h"
#include "wifi_manager.h"

#include <algorithm>
#include <cstring>
#include <esp_wifi.h>

namespace {
std::string ExtractSsid(const wifi_ap_record_t& record) {
    const char* ssid = reinterpret_cast<const char*>(record.ssid);
    return std::string(ssid, strnlen(ssid, sizeof(record.ssid)));
}

void AppendUniqueSsid(std::vector<std::string>& ssids, const std::string& ssid) {
    if (!ssid.empty() && std::find(ssids.begin(), ssids.end(), ssid) == ssids.end()) {
        ssids.push_back(ssid);
    }
}
}

esp_err_t WifiSetupWorkflow::ScanSsids(std::vector<std::string>& ssids) {
    ssids.clear();

    wifi_scan_config_t scan_config = {};
    scan_config.show_hidden = false;
    scan_config.scan_type = WIFI_SCAN_TYPE_ACTIVE;
    scan_config.scan_time.active.min = 100;
    scan_config.scan_time.active.max = 300;

    auto ret = esp_wifi_scan_start(&scan_config, true);
    if (ret != ESP_OK) {
        return ret;
    }

    uint16_t ap_num = 0;
    ret = esp_wifi_scan_get_ap_num(&ap_num);
    if (ret != ESP_OK) {
        return ret;
    }
    if (ap_num == 0) {
        return ESP_ERR_NOT_FOUND;
    }

    std::vector<wifi_ap_record_t> ap_records(ap_num);
    ret = esp_wifi_scan_get_ap_records(&ap_num, ap_records.data());
    if (ret != ESP_OK) {
        return ret;
    }

    for (uint16_t i = 0; i < ap_num; i++) {
        AppendUniqueSsid(ssids, ExtractSsid(ap_records[i]));
    }

    return ssids.empty() ? ESP_ERR_NOT_FOUND : ESP_OK;
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
