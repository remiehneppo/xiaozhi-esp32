#include "wifi_setup_workflow.h"

#include "ssid_manager.h"
#include "wifi_manager.h"

#include <algorithm>
#include <cstring>
#include <esp_wifi.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

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
    scan_config.scan_time.active.min = 120;
    scan_config.scan_time.active.max = 360;

    esp_err_t last_ret = ESP_FAIL;
    constexpr int kMaxScanAttempts = 3;
    for (int attempt = 0; attempt < kMaxScanAttempts; attempt++) {
        esp_wifi_scan_stop();
        vTaskDelay(pdMS_TO_TICKS(120 + attempt * 120));

        auto ret = esp_wifi_scan_start(&scan_config, true);
        if (ret != ESP_OK) {
            last_ret = ret;
            continue;
        }

        uint16_t ap_num = 0;
        ret = esp_wifi_scan_get_ap_num(&ap_num);
        if (ret != ESP_OK) {
            last_ret = ret;
            continue;
        }
        if (ap_num == 0) {
            last_ret = ESP_ERR_NOT_FOUND;
            continue;
        }

        std::vector<wifi_ap_record_t> ap_records(ap_num);
        ret = esp_wifi_scan_get_ap_records(&ap_num, ap_records.data());
        if (ret != ESP_OK) {
            last_ret = ret;
            continue;
        }

        std::sort(ap_records.begin(), ap_records.end(), [](const wifi_ap_record_t& a, const wifi_ap_record_t& b) {
            return a.rssi > b.rssi;
        });
        for (uint16_t i = 0; i < ap_num; i++) {
            AppendUniqueSsid(ssids, ExtractSsid(ap_records[i]));
        }

        if (!ssids.empty()) {
            return ESP_OK;
        }
        last_ret = ESP_ERR_NOT_FOUND;
    }

    return ssids.empty() ? last_ret : ESP_OK;
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
