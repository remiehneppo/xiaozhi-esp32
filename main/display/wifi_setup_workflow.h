#ifndef WIFI_SETUP_WORKFLOW_H
#define WIFI_SETUP_WORKFLOW_H

#include <esp_event.h>
#include <esp_err.h>
#include <string>
#include <vector>

class WifiSetupWorkflow {
public:
    ~WifiSetupWorkflow();

    esp_err_t RegisterScanDoneHandler(esp_event_handler_t handler, void* arg);
    void UnregisterScanDoneHandler();
    esp_err_t StartScan();
    bool GetScannedSsids(std::vector<std::string>& ssids);
    void SaveCredentialsAndReconnect(const char* ssid, const char* password);

private:
    bool scan_handler_registered_ = false;
    esp_event_handler_t scan_done_handler_ = nullptr;
};

#endif // WIFI_SETUP_WORKFLOW_H
