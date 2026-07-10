#include "wifi_setup_workflow.h"

#include "ssid_manager.h"
#include "wifi_manager.h"

#include <algorithm>
#include <cstring>
#include <esp_log.h>
#include <esp_wifi.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#define TAG "WifiSetupWorkflow"

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

esp_err_t EnsureWifiStartedForScan() {
    auto mode_ret = esp_wifi_set_mode(WIFI_MODE_STA);
    if (mode_ret != ESP_OK) {
        ESP_LOGW(TAG, "esp_wifi_set_mode(STA) before scan: %s", esp_err_to_name(mode_ret));
    }

    auto start_ret = esp_wifi_start();
    if (start_ret != ESP_OK) {
        ESP_LOGW(TAG, "esp_wifi_start before scan: %s", esp_err_to_name(start_ret));
    }

    return (mode_ret == ESP_OK || start_ret == ESP_OK) ? ESP_OK : start_ret;
}
}

esp_err_t WifiSetupWorkflow::ScanSsids(std::vector<std::string>& ssids) {
    ssids.clear();

    auto wifi_ret = EnsureWifiStartedForScan();
    if (wifi_ret != ESP_OK) {
        return wifi_ret;
    }

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

    esp_wifi_scan_stop();
    auto stop_ret = esp_wifi_stop();
    if (stop_ret != ESP_OK) {
        ESP_LOGW(TAG, "esp_wifi_stop before station reconnect: %s", esp_err_to_name(stop_ret));
    }
    vTaskDelay(pdMS_TO_TICKS(150));
    wifi_manager.StartStation();
}
