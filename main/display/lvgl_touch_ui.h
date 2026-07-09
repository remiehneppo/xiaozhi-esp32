#ifndef LVGL_TOUCH_UI_H
#define LVGL_TOUCH_UI_H

#include "touch_ui.h"
#include "wifi_setup_workflow.h"
#include <lvgl.h>
#include <esp_event.h>
#include <esp_timer.h>
#include <cstdint>
#include <freertos/FreeRTOS.h>
#include <string>
#include <vector>

class LvglTouchUi : public TouchUi {
public:
    enum class PageType {
        kPageMainGrid,
        kPageChat,
        kPageSettings,
        kPageAbout,
        kPageFileManager,
        kPageMusicPlayer,
        kPageVideoPlayer,
        kPageWifiSetup,
        kPageWifiConnect
    };

private:
    PageType active_page_ = PageType::kPageMainGrid;

    lv_obj_t* master_container = nullptr;
    lv_obj_t* top_bar = nullptr;
    lv_obj_t* page_container = nullptr;

    // Status bar widgets
    lv_obj_t* wifi_icon = nullptr;
    lv_obj_t* battery_icon = nullptr;
    lv_obj_t* time_label = nullptr;
    std::string status_text_;
    std::string current_emotion_;
    esp_timer_handle_t notification_timer_ = nullptr;

    void CreateStatusBar();
    void ShowMainGridPage();
    void ShowChatPage();
    void ShowSettingsPage();
    void ShowAboutPage();
    void ShowFileManagerPage();
    void ShowMusicPlayerPage();
    void ShowVideoPlayerPage();
    void ShowWifiSetupPage();
    void ShowWifiConnectPage();
    void ResetPageWidgets();
    void RebuildActivePage();
    void DismissModalOverlay();
    void LeaveWifiSetupPage();
    void ApplyReadableTextColors();
    const lv_font_t* GetTextFont() const;
    const lv_font_t* GetIconFont() const;
    const lv_font_t* GetLargeIconFont() const;
    void ApplyStatusBarTheme();

    // WiFi setup widgets
    lv_obj_t* wifi_roller = nullptr;
    lv_obj_t* password_textarea = nullptr;
    lv_obj_t* keyboard = nullptr;
    lv_obj_t* wifi_password_label_ = nullptr;
    lv_obj_t* wifi_select_btn_ = nullptr;
    lv_obj_t* wifi_connect_btn_ = nullptr;
    bool wifi_scan_has_results_ = false;
    uint32_t wifi_scan_generation_ = 0;
    std::string selected_wifi_ssid_;
    void ShowWifiPasswordStep(const char* ssid);
    void UpdateWifiScanResults(const std::vector<std::string>& ssids, esp_err_t scan_ret);
    void StartWifiScanTask(uint32_t generation);
    static void WifiScanTask(void* arg);
    portMUX_TYPE wifi_scan_task_mux_ = portMUX_INITIALIZER_UNLOCKED;
    uint32_t wifi_scan_tasks_in_flight_ = 0;
    bool wifi_scan_shutdown_ = false;

    // Modal widgets
    lv_obj_t* modal_overlay_ = nullptr;
    lv_obj_t* modal_keyboard_ = nullptr;

    // Chat widgets
    lv_obj_t* chat_box = nullptr;
    lv_obj_t* mic_status_label = nullptr;
    lv_obj_t* mic_status_icon = nullptr;
    struct ChatMessage {
        std::string role;
        std::string content;
    };
    std::vector<ChatMessage> chat_messages_;
    void AppendChatMessageToView(const std::string& role, const std::string& content);
    void RenderChatHistory();

    DeviceState current_state_ = kDeviceStateIdle;
    void UpdateMicStatusIndicator();

    int wifi_connect_attempts_ = 0;
    WifiSetupWorkflow wifi_setup_workflow_;

    static void NotificationTimerCallback(void* arg);

public:
    LvglTouchUi(Display* display);
    virtual ~LvglTouchUi();

    virtual void Initialize(const char* startup_message = nullptr) override;

    // Display overrides
    virtual void SetStatus(const char* status) override;
    virtual void SetEmotion(const char* emotion) override;
    virtual void SetChatMessage(const char* role, const char* content) override;
    virtual void ClearChatMessages() override;
    virtual void SetTheme(Theme* theme) override;
    virtual void UpdateStatusBar(bool update_all = false) override;
    virtual void ShowNotification(const char* notification, int duration_ms = 3000) override;

    // Device state change handler
    virtual void HandleDeviceStateChange(DeviceState old_state, DeviceState new_state) override;
};

#endif // LVGL_TOUCH_UI_H
