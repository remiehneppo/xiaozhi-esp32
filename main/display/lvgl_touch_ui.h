#ifndef LVGL_TOUCH_UI_H
#define LVGL_TOUCH_UI_H

#include "touch_ui.h"
#include "wifi_setup_workflow.h"
#include <lvgl.h>
#include <esp_event.h>
#include <esp_timer.h>
#include <string>
#include <vector>

class LvglTouchUi : public TouchUi {
public:
    enum class PageType {
        kPageMainGrid,
        kPageChat,
        kPageSettings,
        kPageAbout,
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
    void ShowWifiSetupPage();
    void ShowWifiConnectPage();
    void ResetPageWidgets();
    void RebuildActivePage();
    void DismissModalOverlay();
    void LeaveWifiSetupPage();
    const lv_font_t* GetTextFont() const;
    const lv_font_t* GetIconFont() const;
    const lv_font_t* GetLargeIconFont() const;
    void ApplyStatusBarTheme();

    // WiFi setup widgets
    lv_obj_t* wifi_roller = nullptr;
    lv_obj_t* password_textarea = nullptr;
    lv_obj_t* keyboard = nullptr;
    bool wifi_scan_has_results_ = false;

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
    static void WifiScanDoneHandler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
    void PopulateWifiRoller();

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
