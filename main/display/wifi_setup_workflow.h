#ifndef WIFI_SETUP_WORKFLOW_H
#define WIFI_SETUP_WORKFLOW_H

#include <esp_err.h>
#include <string>
#include <vector>

class WifiSetupWorkflow {
public:
    esp_err_t ScanSsids(std::vector<std::string>& ssids);
    void SaveCredentialsAndReconnect(const char* ssid, const char* password);
};

#endif // WIFI_SETUP_WORKFLOW_H
