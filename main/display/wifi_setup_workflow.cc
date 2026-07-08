#include "wifi_setup_workflow.h"

#include "ssid_manager.h"
#include "wifi_manager.h"

#include <algorithm>
#include <cstring>
#include <esp_wifi.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

namespace {

bool ExtractUniqueSsids(const std::vector<wifi_ap_record_t>& ap_records, std::vector<std::string>& ssids) {
    ssids.clear();
    for (const auto& ap_record : ap_records) {
        auto ssid_len = strnlen(reinterpret_cast<const char*>(ap_record.ssid), sizeof(ap_record.ssid));
        std::string ssid(reinterpret_cast<const char*>(ap_record.ssid), ssid_len);
        if (!ssid.empty() && std::find(ssids.begin(), ssids.end(), ssid) == ssids.end()) {
            ssids.push_back(ssid);
        }
    }
    return !ssids.empty();
}

} // namespace

esp_err_t WifiSetupWorkflow::ScanSsids(std::vector<std::string>& ssids) {
    ssids.clear();

    auto& wifi_manager = WifiManager::GetInstance();
    auto ap_records = wifi_manager.GetAccessPoints();
    if (ExtractUniqueSsids(ap_records, ssids)) {
        return ESP_OK;
    }

    esp_wifi_scan_start(nullptr, false);

    constexpr int kMaxAttempts = 6;
    for (int attempt = 0; attempt < kMaxAttempts; ++attempt) {
        vTaskDelay(pdMS_TO_TICKS(250));
        ap_records = wifi_manager.GetAccessPoints();
        if (ExtractUniqueSsids(ap_records, ssids)) {
            return ESP_OK;
        }
    }

    if (wifi_manager.IsConfigMode() || wifi_manager.IsStationActive()) {
        return ESP_ERR_NOT_FOUND;
    }

    esp_wifi_scan_stop();

    wifi_scan_config_t scan_config = {};
    scan_config.show_hidden = false;
    scan_config.scan_type = WIFI_SCAN_TYPE_ACTIVE;
    scan_config.scan_time.active.min = 100;
    scan_config.scan_time.active.max = 300;

    auto ret = esp_wifi_scan_start(&scan_config, true);
    if (ret == ESP_ERR_WIFI_STATE) {
        esp_wifi_scan_stop();
        vTaskDelay(pdMS_TO_TICKS(100));
        ret = esp_wifi_scan_start(&scan_config, true);
    }
    if (ret != ESP_OK) {
        return ret;
    }

    if (!GetScannedSsids(ssids)) {
        return ESP_ERR_NOT_FOUND;
    }
    return ESP_OK;
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

    return ExtractUniqueSsids(ap_records, ssids);
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
