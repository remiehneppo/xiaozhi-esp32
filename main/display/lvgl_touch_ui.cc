#include "lvgl_touch_ui.h"
#include "lvgl_theme.h"
#include "font_awesome.h"
#include "touch_ui_state_presenter.h"
#include "board.h"
#include "wifi_manager.h"
#include "system_info.h"
#include "audio_codec.h"
#include "settings.h"
#include "application.h"
#include <esp_log.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <esp_err.h>
#include <time.h>
#include <vector>
#include <esp_app_desc.h>

#define TAG "LvglTouchUi"

LV_FONT_DECLARE(BUILTIN_TEXT_FONT);
LV_FONT_DECLARE(BUILTIN_ICON_FONT);

namespace {

struct WifiScanTaskPayload {
    LvglTouchUi* ui;
    uint32_t generation;
    std::vector<std::string> ssids;
    esp_err_t scan_ret;
};


void ConfigureWrapLabel(lv_obj_t* label, const lv_font_t* font, int32_t width, lv_text_align_t align = LV_TEXT_ALIGN_LEFT) {
    lv_obj_set_style_text_font(label, font, 0);
    lv_obj_set_width(label, width);
    lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
    lv_obj_set_style_text_align(label, align, 0);
}

void ConfigureDotLabel(lv_obj_t* label, const lv_font_t* font, int32_t width, lv_text_align_t align = LV_TEXT_ALIGN_CENTER) {
    lv_obj_set_style_text_font(label, font, 0);
    lv_obj_set_width(label, width);
    lv_label_set_long_mode(label, LV_LABEL_LONG_DOT);
    lv_obj_set_style_text_align(label, align, 0);
}

void ConfigureScrollLabel(lv_obj_t* label, const lv_font_t* font, int32_t width, lv_text_align_t align = LV_TEXT_ALIGN_CENTER) {
    lv_obj_set_style_text_font(label, font, 0);
    lv_obj_set_width(label, width);
    lv_label_set_long_mode(label, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_style_text_align(label, align, 0);
}

void ConfigureButtonLabel(lv_obj_t* label, const lv_font_t* font) {
    lv_obj_set_style_text_font(label, font, 0);
    lv_obj_set_width(label, LV_PCT(100));
    lv_obj_set_style_max_width(label, LV_PCT(100), 0);
    lv_label_set_long_mode(label, LV_LABEL_LONG_DOT);
    lv_obj_set_style_text_align(label, LV_TEXT_ALIGN_CENTER, 0);
}

lv_color_t GetTileBackgroundColor(int col, int row) {
    static const uint32_t kBackgrounds[2][3] = {
        {0x0EA5E9, 0x7C3AED, 0x16A34A},
        {0xD97706, 0xDB2777, 0x0891B2},
    };
    return lv_color_hex(kBackgrounds[row][col]);
}

void HandleMicActionClicked(lv_event_t* e) {
    if (lv_event_get_code(e) != LV_EVENT_CLICKED) {
        return;
    }

    auto& app = Application::GetInstance();
    if (app.GetDeviceState() == kDeviceStateListening ||
        app.GetDeviceState() == kDeviceStateAudioTesting) {
        app.StopListening();
        return;
    }

    app.StartListening();
}

lv_obj_tree_walk_res_t ApplyLabelTextColor(lv_obj_t* obj, void* user_data) {
    if (lv_obj_check_type(obj, &lv_label_class)) {
        auto* color = static_cast<lv_color_t*>(user_data);
        lv_obj_set_style_text_color(obj, *color, 0);
    }
    return LV_OBJ_TREE_WALK_NEXT;
}

} // namespace

LvglTouchUi::LvglTouchUi(Display* display) : TouchUi(display) {
    esp_timer_create_args_t notification_timer_args = {
        .callback = &LvglTouchUi::NotificationTimerCallback,
        .arg = this,
        .dispatch_method = ESP_TIMER_TASK,
        .name = "touch_ui_notification",
        .skip_unhandled_events = true,
    };
    auto ret = esp_timer_create(&notification_timer_args, &notification_timer_);
    if (ret != ESP_OK) {
        ESP_LOGW(TAG, "Failed to create notification timer: %s", esp_err_to_name(ret));
        notification_timer_ = nullptr;
    }
    ESP_LOGI(TAG, "LvglTouchUi created");
}

LvglTouchUi::~LvglTouchUi() {
    portENTER_CRITICAL(&wifi_scan_task_mux_);
    wifi_scan_shutdown_ = true;
    portEXIT_CRITICAL(&wifi_scan_task_mux_);

    while (true) {
        uint32_t tasks_in_flight = 0;
        portENTER_CRITICAL(&wifi_scan_task_mux_);
        tasks_in_flight = wifi_scan_tasks_in_flight_;
        portEXIT_CRITICAL(&wifi_scan_task_mux_);
        if (tasks_in_flight == 0) {
            break;
        }
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    if (notification_timer_ != nullptr) {
        esp_timer_stop(notification_timer_);
        esp_timer_delete(notification_timer_);
        notification_timer_ = nullptr;
    }
    ESP_LOGI(TAG, "LvglTouchUi destroyed");
}

void LvglTouchUi::Initialize(const char* startup_message) {
    ESP_LOGI(TAG, "Initializing Touch UI...");
    setup_ui_called_ = true;

    DisplayLockGuard lock(this);

    // Create master container spanning the entire screen
    master_container = lv_obj_create(lv_screen_active());
    lv_obj_set_size(master_container, width_, height_);
    lv_obj_set_style_radius(master_container, 0, 0);
    lv_obj_set_style_pad_all(master_container, 0, 0);
    lv_obj_set_style_border_width(master_container, 0, 0);
    lv_obj_set_flex_flow(master_container, LV_FLEX_FLOW_COLUMN);

    // Load default theme background/text color if available
    auto* theme = dynamic_cast<LvglTheme*>(current_theme_);
    if (theme) {
        lv_obj_set_style_bg_color(master_container, theme->background_color(), 0);
        lv_obj_set_style_text_color(master_container, theme->text_color(), 0);
    } else {
        lv_obj_set_style_bg_color(master_container, lv_color_hex(0x1F1F1F), 0); // Default dark
        lv_obj_set_style_text_color(master_container, lv_color_hex(0xFFFFFF), 0);
    }

    CreateStatusBar();

    // Content container (takes up remaining vertical space)
    page_container = lv_obj_create(master_container);
    lv_obj_set_width(page_container, LV_PCT(100));
    lv_obj_set_flex_grow(page_container, 1);
    lv_obj_set_style_radius(page_container, 0, 0);
    lv_obj_set_style_pad_all(page_container, 8, 0);
    lv_obj_set_style_border_width(page_container, 0, 0);
    lv_obj_set_style_bg_opa(page_container, LV_OPA_TRANSP, 0); // Transparent content area

    // Initially show Main Grid page
    ShowMainGridPage();

    UpdateStatusBar(true);

    if (startup_message != nullptr && startup_message[0] != '\0') {
        ShowNotification(startup_message, 3000);
    }
}

void LvglTouchUi::CreateStatusBar() {
    auto* theme = dynamic_cast<LvglTheme*>(current_theme_);

    top_bar = lv_obj_create(master_container);
    lv_obj_set_size(top_bar, LV_PCT(100), 24);
    lv_obj_set_style_radius(top_bar, 0, 0);
    lv_obj_set_style_pad_top(top_bar, 2, 0);
    lv_obj_set_style_pad_bottom(top_bar, 2, 0);
    lv_obj_set_style_pad_left(top_bar, 8, 0);
    lv_obj_set_style_pad_right(top_bar, 8, 0);
    lv_obj_set_style_border_width(top_bar, 0, 0);

    if (theme) {
        lv_obj_set_style_bg_color(top_bar, theme->background_color(), 0);
        lv_obj_set_style_bg_opa(top_bar, LV_OPA_80, 0);
        lv_obj_set_style_text_color(top_bar, theme->text_color(), 0);
    } else {
        lv_obj_set_style_bg_color(top_bar, lv_color_hex(0x2D2D2D), 0);
        lv_obj_set_style_text_color(top_bar, lv_color_hex(0xFFFFFF), 0);
    }

    lv_obj_set_flex_flow(top_bar, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(top_bar, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    // Left: WiFi Strength
    wifi_icon = lv_label_create(top_bar);
    lv_obj_set_width(wifi_icon, 28);
    lv_obj_set_style_text_align(wifi_icon, LV_TEXT_ALIGN_LEFT, 0);
    lv_label_set_text(wifi_icon, FONT_AWESOME_WIFI_SLASH);

    // Center: System clock
    time_label = lv_label_create(top_bar);
    lv_obj_set_flex_grow(time_label, 1);
    lv_label_set_long_mode(time_label, LV_LABEL_LONG_SCROLL_CIRCULAR);
    lv_obj_set_style_text_align(time_label, LV_TEXT_ALIGN_CENTER, 0);
    lv_label_set_text(time_label, "--:--");

    // Right: Battery
    battery_icon = lv_label_create(top_bar);
    lv_obj_set_width(battery_icon, 82);
    lv_label_set_long_mode(battery_icon, LV_LABEL_LONG_DOT);
    lv_obj_set_style_text_align(battery_icon, LV_TEXT_ALIGN_RIGHT, 0);
    lv_label_set_text(battery_icon, FONT_AWESOME_BATTERY_HALF " --%");

    ApplyStatusBarTheme();
}

void LvglTouchUi::UpdateStatusBar(bool update_all) {
    (void)update_all;
    if (!setup_ui_called_) return;

    DisplayLockGuard lock(this);

    // 1. Time clock update
    time_t now;
    struct tm timeinfo;
    time(&now);
    localtime_r(&now, &timeinfo);

    char time_str[64];
    if (!status_text_.empty()) {
        snprintf(time_str, sizeof(time_str), "%s", status_text_.c_str());
    } else if (timeinfo.tm_year > (1970 - 1900)) {
        snprintf(time_str, sizeof(time_str), "%02d:%02d", timeinfo.tm_hour, timeinfo.tm_min);
    } else {
        snprintf(time_str, sizeof(time_str), "--:--");
    }
    lv_label_set_text(time_label, time_str);

    // 2. Battery update
    int battery_level = 0;
    bool charging = false;
    bool discharging = false;
    if (Board::GetInstance().GetBatteryLevel(battery_level, charging, discharging)) {
        const char* bat_symbol = FONT_AWESOME_BATTERY_HALF;
        if (charging) {
            bat_symbol = FONT_AWESOME_BATTERY_BOLT;
        } else if (battery_level > 85) {
            bat_symbol = FONT_AWESOME_BATTERY_FULL;
        } else if (battery_level > 60) {
            bat_symbol = FONT_AWESOME_BATTERY_THREE_QUARTERS;
        } else if (battery_level > 35) {
            bat_symbol = FONT_AWESOME_BATTERY_HALF;
        } else if (battery_level > 15) {
            bat_symbol = FONT_AWESOME_BATTERY_QUARTER;
        } else {
            bat_symbol = FONT_AWESOME_BATTERY_EMPTY;
        }

        char bat_str[32];
        snprintf(bat_str, sizeof(bat_str), "%s %d%%", bat_symbol, battery_level);
        lv_label_set_text(battery_icon, bat_str);
    } else {
        lv_label_set_text(battery_icon, "");
    }

    // 3. WiFi strength update
    auto& wifi = WifiManager::GetInstance();
    if (wifi.IsConnected()) {
        int rssi = wifi.GetRssi();
        const char* wifi_symbol = FONT_AWESOME_WIFI;
        if (rssi < -85) {
            wifi_symbol = FONT_AWESOME_WIFI_WEAK;
        } else if (rssi < -70) {
            wifi_symbol = FONT_AWESOME_WIFI_FAIR;
        }
        lv_label_set_text(wifi_icon, wifi_symbol);
    } else {
        lv_label_set_text(wifi_icon, FONT_AWESOME_WIFI_SLASH);
    }
}


void LvglTouchUi::ShowMainGridPage() {
    DisplayLockGuard lock(this);
    ResetPageWidgets();
    active_page_ = PageType::kPageMainGrid;

    lv_obj_clean(page_container);

    auto text_font = GetTextFont();
    auto icon_font = GetLargeIconFont();

    lv_obj_t* home_layout = lv_obj_create(page_container);
    lv_obj_set_size(home_layout, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_radius(home_layout, 0, 0);
    lv_obj_set_style_pad_all(home_layout, 2, 0);
    lv_obj_set_style_pad_row(home_layout, 0, 0);
    lv_obj_set_style_pad_column(home_layout, 0, 0);
    lv_obj_set_style_border_width(home_layout, 0, 0);
    lv_obj_set_style_bg_opa(home_layout, LV_OPA_TRANSP, 0);
    lv_obj_set_flex_flow(home_layout, LV_FLEX_FLOW_COLUMN);

    lv_obj_t* tile_grid = lv_obj_create(home_layout);
    lv_obj_set_width(tile_grid, LV_PCT(100));
    lv_obj_set_flex_grow(tile_grid, 1);
    lv_obj_set_style_radius(tile_grid, 0, 0);
    lv_obj_set_style_pad_all(tile_grid, 0, 0);
    lv_obj_set_style_border_width(tile_grid, 0, 0);
    lv_obj_set_style_bg_opa(tile_grid, LV_OPA_TRANSP, 0);
    lv_obj_set_layout(tile_grid, LV_LAYOUT_GRID);

    static int32_t col_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    static int32_t row_dsc[] = {LV_GRID_FR(1), LV_GRID_FR(1), LV_GRID_TEMPLATE_LAST};
    lv_obj_set_style_grid_column_dsc_array(tile_grid, col_dsc, 0);
    lv_obj_set_style_grid_row_dsc_array(tile_grid, row_dsc, 0);

    auto create_tile = [&](int col, int row, const char* icon, const char* label, lv_event_cb_t cb) {
        lv_color_t tile_color = GetTileBackgroundColor(col, row);
        lv_obj_t* tile = lv_button_create(tile_grid);
        lv_obj_set_grid_cell(tile, LV_GRID_ALIGN_STRETCH, col, 1, LV_GRID_ALIGN_STRETCH, row, 1);
        lv_obj_set_style_radius(tile, 8, 0);
        lv_obj_set_style_pad_all(tile, 4, 0);
        lv_obj_set_style_pad_row(tile, 2, 0);
        lv_obj_set_style_border_width(tile, 0, 0);
        lv_obj_set_flex_flow(tile, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_flex_align(tile, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        lv_obj_set_style_bg_color(tile, tile_color, 0);
        lv_obj_set_style_bg_opa(tile, LV_OPA_COVER, 0);
        lv_obj_add_event_cb(tile, cb, LV_EVENT_CLICKED, this);

        lv_obj_t* icon_label = lv_label_create(tile);
        lv_obj_set_style_text_font(icon_label, icon_font, 0);
        lv_label_set_text(icon_label, icon);
        lv_obj_set_style_text_color(icon_label, lv_color_hex(0xFFFFFF), 0);

        lv_obj_t* txt_label = lv_label_create(tile);
        ConfigureButtonLabel(txt_label, text_font);
        lv_label_set_text(txt_label, label);
        lv_obj_set_style_text_color(txt_label, lv_color_hex(0xFFFFFF), 0);
    };

    create_tile(0, 0, FONT_AWESOME_MICROCHIP_AI, "AI Agent", [](lv_event_t* e) {
        auto* ui = static_cast<LvglTouchUi*>(lv_event_get_user_data(e));
        ui->ShowChatPage();
    });

    create_tile(1, 0, FONT_AWESOME_MUSIC, "Music", [](lv_event_t* e) {
        auto* ui = static_cast<LvglTouchUi*>(lv_event_get_user_data(e));
        ui->ShowMusicPlayerPage();
    });


    create_tile(2, 0, FONT_AWESOME_PLAY, "Video", [](lv_event_t* e) {
        auto* ui = static_cast<LvglTouchUi*>(lv_event_get_user_data(e));
        ui->ShowVideoPlayerPage();
    });

    create_tile(0, 1, FONT_AWESOME_SD_CARD, "File", [](lv_event_t* e) {
        auto* ui = static_cast<LvglTouchUi*>(lv_event_get_user_data(e));
        ui->ShowFileManagerPage();
    });

    create_tile(1, 1, FONT_AWESOME_GEAR, "Cài đặt", [](lv_event_t* e) {
        auto* ui = static_cast<LvglTouchUi*>(lv_event_get_user_data(e));
        ui->ShowSettingsPage();
    });

    create_tile(2, 1, FONT_AWESOME_CIRCLE_INFO, "About", [](lv_event_t* e) {
        auto* ui = static_cast<LvglTouchUi*>(lv_event_get_user_data(e));
        ui->ShowAboutPage();
    });

    ApplyReadableTextColors();
}

void LvglTouchUi::ShowChatPage() {
    DisplayLockGuard lock(this);
    ResetPageWidgets();
    active_page_ = PageType::kPageChat;

    lv_obj_clean(page_container);

    auto text_font = GetTextFont();
    auto large_icon_font = GetLargeIconFont();

    // Main vertical layout inside page_container
    lv_obj_t* chat_layout = lv_obj_create(page_container);
    lv_obj_set_size(chat_layout, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_radius(chat_layout, 0, 0);
    lv_obj_set_style_pad_all(chat_layout, 2, 0);
    lv_obj_set_style_border_width(chat_layout, 0, 0);
    lv_obj_set_style_bg_opa(chat_layout, LV_OPA_TRANSP, 0);
    lv_obj_set_flex_flow(chat_layout, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(chat_layout, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    // 1. Chat Box (scrollable container for message bubbles)
    chat_box = lv_obj_create(chat_layout);
    lv_obj_set_width(chat_box, LV_PCT(100));
    lv_obj_set_flex_grow(chat_box, 1);
    lv_obj_set_style_radius(chat_box, 0, 0);
    lv_obj_set_style_pad_all(chat_box, 4, 0);
    lv_obj_set_style_border_width(chat_box, 0, 0);
    lv_obj_set_style_bg_opa(chat_box, LV_OPA_TRANSP, 0);
    lv_obj_set_flex_flow(chat_box, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(chat_box, 6, 0); // Spacing between messages
    lv_obj_set_scroll_dir(chat_box, LV_DIR_VER);
    lv_obj_set_scrollbar_mode(chat_box, LV_SCROLLBAR_MODE_AUTO);

    // 2. Bottom Mic State & Navigation Panel
    lv_obj_t* bottom_panel = lv_obj_create(chat_layout);
    lv_obj_set_size(bottom_panel, LV_PCT(100), 38);
    lv_obj_set_style_radius(bottom_panel, 0, 0);
    lv_obj_set_style_pad_all(bottom_panel, 0, 0);
    lv_obj_set_style_border_width(bottom_panel, 0, 0);
    lv_obj_set_style_bg_opa(bottom_panel, LV_OPA_TRANSP, 0);
    lv_obj_set_flex_flow(bottom_panel, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(bottom_panel, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    // Back Button (left)
    lv_obj_t* back_btn = lv_button_create(bottom_panel);
    lv_obj_set_size(back_btn, 96, 30);
    lv_obj_t* back_label = lv_label_create(back_btn);
    ConfigureButtonLabel(back_label, text_font);
    lv_label_set_text(back_label, FONT_AWESOME_ARROW_LEFT " Trở về");
    lv_obj_center(back_label);
    lv_obj_add_event_cb(back_btn, [](lv_event_t* e) {
        auto* ui = static_cast<LvglTouchUi*>(lv_event_get_user_data(e));
        ui->ShowMainGridPage();
    }, LV_EVENT_CLICKED, this);

    // Mic Status Container (center/right)
    lv_obj_t* status_container = lv_obj_create(bottom_panel);
    lv_obj_set_height(status_container, 30);
    lv_obj_set_flex_grow(status_container, 1);
    lv_obj_set_style_radius(status_container, 0, 0);
    lv_obj_set_style_pad_all(status_container, 0, 0);
    lv_obj_set_style_border_width(status_container, 0, 0);
    lv_obj_set_style_bg_opa(status_container, LV_OPA_TRANSP, 0);
    lv_obj_set_flex_flow(status_container, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(status_container, LV_FLEX_ALIGN_END, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    lv_obj_add_flag(status_container, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(status_container, HandleMicActionClicked, LV_EVENT_CLICKED, this);

    // Mic Status Icon
    mic_status_icon = lv_label_create(status_container);
    lv_obj_set_style_text_font(mic_status_icon, large_icon_font, 0);
    lv_label_set_text(mic_status_icon, FONT_AWESOME_MICROPHONE);
    lv_obj_set_style_pad_right(mic_status_icon, 4, 0);
    lv_obj_add_flag(mic_status_icon, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(mic_status_icon, HandleMicActionClicked, LV_EVENT_CLICKED, this);

    // Mic Status Label
    mic_status_label = lv_label_create(status_container);
    lv_obj_set_flex_grow(mic_status_label, 1);
    ConfigureDotLabel(mic_status_label, text_font, 1, LV_TEXT_ALIGN_RIGHT);
    lv_label_set_text(mic_status_label, "Đang chờ...");
    lv_obj_add_flag(mic_status_label, LV_OBJ_FLAG_CLICKABLE);
    lv_obj_add_event_cb(mic_status_label, HandleMicActionClicked, LV_EVENT_CLICKED, this);

    RenderChatHistory();
    ApplyReadableTextColors();
    UpdateMicStatusIndicator();
}

void LvglTouchUi::AppendChatMessageToView(const std::string& role, const std::string& content) {
    if (chat_box == nullptr || content.empty()) {
        return;
    }

    auto* theme = dynamic_cast<LvglTheme*>(current_theme_);
    auto text_font = GetTextFont();

    bool is_user = role == "user";

    lv_obj_t* row = lv_obj_create(chat_box);
    lv_obj_set_size(row, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_style_radius(row, 0, 0);
    lv_obj_set_style_pad_all(row, 0, 0);
    lv_obj_set_style_border_width(row, 0, 0);
    lv_obj_set_style_bg_opa(row, LV_OPA_TRANSP, 0);
    lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(row, is_user ? LV_FLEX_ALIGN_END : LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    const int32_t bubble_max_width = width_ * 3 / 4;
    const int32_t label_width = bubble_max_width - 20;

    lv_obj_t* bubble = lv_obj_create(row);
    lv_obj_set_size(bubble, bubble_max_width, LV_SIZE_CONTENT);
    lv_obj_set_style_radius(bubble, 10, 0);
    lv_obj_set_style_pad_left(bubble, 10, 0);
    lv_obj_set_style_pad_right(bubble, 10, 0);
    lv_obj_set_style_pad_top(bubble, 6, 0);
    lv_obj_set_style_pad_bottom(bubble, 6, 0);
    lv_obj_set_style_border_width(bubble, 0, 0);

    if (is_user) {
        if (theme) {
            lv_obj_set_style_bg_color(bubble, theme->user_bubble_color(), 0);
        } else {
            lv_obj_set_style_bg_color(bubble, lv_color_hex(0x007ACC), 0);
        }
        lv_obj_set_style_text_color(bubble, lv_color_hex(0xFFFFFF), 0);
    } else {
        if (theme) {
            lv_obj_set_style_bg_color(bubble, theme->assistant_bubble_color(), 0);
            lv_obj_set_style_text_color(bubble, theme->text_color(), 0);
        } else {
            lv_obj_set_style_bg_color(bubble, lv_color_hex(0x2A2A2A), 0);
            lv_obj_set_style_text_color(bubble, lv_color_hex(0xFFFFFF), 0);
        }
    }

    lv_obj_t* label = lv_label_create(bubble);
    lv_obj_set_style_text_font(label, text_font, 0);
    lv_label_set_long_mode(label, LV_LABEL_LONG_WRAP);
    lv_obj_set_width(label, label_width);
    lv_label_set_text(label, content.c_str());
    lv_obj_set_style_text_color(label, is_user ? lv_color_hex(0xFFFFFF)
                                               : (theme ? theme->text_color() : lv_color_hex(0xFFFFFF)), 0);

    lv_obj_scroll_to_view(row, LV_ANIM_ON);
}

void LvglTouchUi::RenderChatHistory() {
    if (chat_box == nullptr) {
        return;
    }

    lv_obj_clean(chat_box);
    for (const auto& message : chat_messages_) {
        AppendChatMessageToView(message.role, message.content);
    }
}

void LvglTouchUi::UpdateMicStatusIndicator() {
    if (!mic_status_icon || !mic_status_label) return;

    DisplayLockGuard lock(this);
    auto* theme = dynamic_cast<LvglTheme*>(current_theme_);

    // Default colors
    lv_color_t normal_color = theme ? theme->text_color() : lv_color_hex(0xFFFFFF);
    lv_color_t active_color = lv_color_hex(0x2ECC71); // Green
    lv_color_t speaking_color = lv_color_hex(0x3498DB); // Blue
    lv_color_t warning_color = lv_color_hex(0xE74C3C); // Red

    // Clean animation if any
    lv_anim_del(mic_status_icon, nullptr);
    lv_obj_set_style_text_opa(mic_status_icon, LV_OPA_COVER, 0);

    switch (GetTouchUiMicVisual(current_state_, current_emotion_)) {
        case TouchUiMicVisual::kIdleAi:
            lv_label_set_text(mic_status_icon, FONT_AWESOME_MICROCHIP_AI);
            lv_obj_set_style_text_color(mic_status_icon, normal_color, 0);
            lv_label_set_text(mic_status_label, "Sẵn sàng");
            break;

        case TouchUiMicVisual::kIdleMicrophone:
            lv_label_set_text(mic_status_icon, FONT_AWESOME_MICROPHONE);
            lv_obj_set_style_text_color(mic_status_icon, normal_color, 0);
            lv_label_set_text(mic_status_label, "Sẵn sàng");
            break;

        case TouchUiMicVisual::kListening:
            lv_label_set_text(mic_status_icon, FONT_AWESOME_MICROPHONE);
            lv_obj_set_style_text_color(mic_status_icon, active_color, 0);
            lv_label_set_text(mic_status_label, "Đang nghe...");

            // Flashing animation for mic icon during listening
            {
                lv_anim_t a;
                lv_anim_init(&a);
                lv_anim_set_var(&a, mic_status_icon);
                lv_anim_set_values(&a, LV_OPA_30, LV_OPA_COVER);
                lv_anim_set_time(&a, 600);
                lv_anim_set_playback_time(&a, 600);
                lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
                lv_anim_set_exec_cb(&a, [](void* var, int32_t val) {
                    lv_obj_set_style_text_opa((lv_obj_t*)var, val, 0);
                });
                lv_anim_start(&a);
            }
            break;

        case TouchUiMicVisual::kSpeaking:
            lv_label_set_text(mic_status_icon, FONT_AWESOME_VOLUME_HIGH);
            lv_obj_set_style_text_color(mic_status_icon, speaking_color, 0);
            lv_label_set_text(mic_status_label, "Đang nói...");

            // Flashing animation for speaker icon during speaking
            {
                lv_anim_t a;
                lv_anim_init(&a);
                lv_anim_set_var(&a, mic_status_icon);
                lv_anim_set_values(&a, LV_OPA_50, LV_OPA_COVER);
                lv_anim_set_time(&a, 400);
                lv_anim_set_playback_time(&a, 400);
                lv_anim_set_repeat_count(&a, LV_ANIM_REPEAT_INFINITE);
                lv_anim_set_exec_cb(&a, [](void* var, int32_t val) {
                    lv_obj_set_style_text_opa((lv_obj_t*)var, val, 0);
                });
                lv_anim_start(&a);
            }
            break;

        case TouchUiMicVisual::kWaiting:
            lv_label_set_text(mic_status_icon, FONT_AWESOME_SPINNER);
            lv_obj_set_style_text_color(mic_status_icon, warning_color, 0);
            lv_label_set_text(mic_status_label, "Đang chờ...");
            break;
    }
}

void LvglTouchUi::ShowSettingsPage() {

    DisplayLockGuard lock(this);
    ResetPageWidgets();

    active_page_ = PageType::kPageSettings;
    lv_obj_clean(page_container);

    auto* theme = dynamic_cast<LvglTheme*>(current_theme_);
    auto text_font = GetTextFont();

    // Create a scrollable flex container for settings items
    lv_obj_t* settings_layout = lv_obj_create(page_container);
    lv_obj_set_size(settings_layout, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_radius(settings_layout, 0, 0);
    lv_obj_set_style_pad_all(settings_layout, 8, 0);
    lv_obj_set_style_border_width(settings_layout, 0, 0);
    lv_obj_set_style_bg_opa(settings_layout, LV_OPA_TRANSP, 0);
    lv_obj_set_flex_flow(settings_layout, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(settings_layout, 12, 0);
    lv_obj_set_scroll_dir(settings_layout, LV_DIR_VER);
    lv_obj_set_scrollbar_mode(settings_layout, LV_SCROLLBAR_MODE_AUTO);

    // Row helper: Creates a flex row container
    auto create_settings_row = [&](lv_obj_t* parent) {
        lv_obj_t* row = lv_obj_create(parent);
        lv_obj_set_size(row, LV_PCT(100), LV_SIZE_CONTENT);
        lv_obj_set_style_min_height(row, 40, 0);
        lv_obj_set_style_radius(row, 6, 0);
        lv_obj_set_style_pad_left(row, 8, 0);
        lv_obj_set_style_pad_right(row, 8, 0);
        lv_obj_set_style_pad_top(row, 4, 0);
        lv_obj_set_style_pad_bottom(row, 4, 0);

        if (theme) {
            lv_obj_set_style_bg_color(row, theme->assistant_bubble_color(), 0);
            lv_obj_set_style_bg_opa(row, LV_OPA_60, 0);
            lv_obj_set_style_border_color(row, theme->border_color(), 0);
            lv_obj_set_style_border_width(row, 1, 0);
        } else {
            lv_obj_set_style_bg_color(row, lv_color_hex(0x2D2D2D), 0);
            lv_obj_set_style_border_width(row, 0, 0);
        }

        lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
        lv_obj_set_flex_align(row, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        return row;
    };

    // Item 1: Volume
    lv_obj_t* vol_row = create_settings_row(settings_layout);

    lv_obj_t* vol_label = lv_label_create(vol_row);
    lv_obj_set_flex_grow(vol_label, 1);
    ConfigureWrapLabel(vol_label, text_font, 1);
    int current_volume = Board::GetInstance().GetAudioCodec()->output_volume();
    char vol_str[32];
    snprintf(vol_str, sizeof(vol_str), "Âm lượng: %d%%", current_volume);
    lv_label_set_text(vol_label, vol_str);

    lv_obj_t* vol_slider = lv_slider_create(vol_row);
    lv_obj_set_width(vol_slider, 100);
    lv_slider_set_range(vol_slider, 0, 100);
    lv_slider_set_value(vol_slider, current_volume, LV_ANIM_OFF);
    lv_obj_add_event_cb(vol_slider, [](lv_event_t* e) {
        lv_obj_t* slider_obj = (lv_obj_t*)lv_event_get_target(e);
        lv_obj_t* parent_row = lv_obj_get_parent(slider_obj);
        lv_obj_t* label_obj = lv_obj_get_child(parent_row, 0);

        int val = lv_slider_get_value(slider_obj);
        char label_str[32];
        snprintf(label_str, sizeof(label_str), "Âm lượng: %d%%", val);
        lv_label_set_text(label_obj, label_str);

        Board::GetInstance().GetAudioCodec()->SetOutputVolume(val);
    }, LV_EVENT_VALUE_CHANGED, nullptr);

    // Item 2: Theme
    lv_obj_t* theme_row = create_settings_row(settings_layout);

    lv_obj_t* theme_label = lv_label_create(theme_row);
    lv_obj_set_flex_grow(theme_label, 1);
    ConfigureWrapLabel(theme_label, text_font, 1);
    lv_label_set_text(theme_label, "Giao diện tối:");

    lv_obj_t* theme_sw = lv_switch_create(theme_row);
    if (theme && theme->name() == "dark") {
        lv_obj_add_state(theme_sw, LV_STATE_CHECKED);
    }
    lv_obj_add_event_cb(theme_sw, [](lv_event_t* e) {
        auto* ui = static_cast<LvglTouchUi*>(lv_event_get_user_data(e));
        lv_obj_t* sw = (lv_obj_t*)lv_event_get_target(e);
        bool is_dark = lv_obj_has_state(sw, LV_STATE_CHECKED);

        auto* new_theme = LvglThemeManager::GetInstance().GetTheme(is_dark ? "dark" : "light");
        if (new_theme) {
            ui->SetTheme(new_theme);
        }
    }, LV_EVENT_VALUE_CHANGED, this);

    // Item 3: OTA Config
    lv_obj_t* ota_row = create_settings_row(settings_layout);

    lv_obj_t* ota_lbl = lv_label_create(ota_row);
    lv_obj_set_flex_grow(ota_lbl, 1);
    ConfigureWrapLabel(ota_lbl, text_font, 1);
    lv_label_set_text(ota_lbl, "Cấu hình OTA:");

    lv_obj_t* ota_btn = lv_button_create(ota_row);
    lv_obj_set_size(ota_btn, 72, 30);
    lv_obj_t* ota_btn_lbl = lv_label_create(ota_btn);
    ConfigureButtonLabel(ota_btn_lbl, text_font);
    lv_label_set_text(ota_btn_lbl, "Sửa");
    lv_obj_center(ota_btn_lbl);

    lv_obj_add_event_cb(ota_btn, [](lv_event_t* e) {
        auto* ui = static_cast<LvglTouchUi*>(lv_event_get_user_data(e));
        auto* current_theme = dynamic_cast<LvglTheme*>(ui->current_theme_);
        ui->DismissModalOverlay();

        // 1. Overlay
        lv_obj_t* overlay = lv_obj_create(lv_screen_active());
        ui->modal_overlay_ = overlay;
        lv_obj_set_size(overlay, LV_PCT(100), LV_PCT(100));
        lv_obj_set_style_bg_color(overlay, lv_color_hex(0x000000), 0);
        lv_obj_set_style_bg_opa(overlay, LV_OPA_50, 0);
        lv_obj_set_style_radius(overlay, 0, 0);
        lv_obj_set_style_pad_all(overlay, 0, 0);
        lv_obj_set_style_border_width(overlay, 0, 0);
        lv_obj_add_flag(overlay, LV_OBJ_FLAG_CLICKABLE);

        // 2. Dialog box
        lv_obj_t* dialog = lv_obj_create(overlay);
        lv_obj_set_size(dialog, LV_PCT(92), LV_SIZE_CONTENT);
        lv_obj_set_style_max_width(dialog, 300, 0);
        lv_obj_set_style_min_height(dialog, 116, 0);
        lv_obj_align(dialog, LV_ALIGN_TOP_MID, 0, 10);
        lv_obj_set_style_radius(dialog, 8, 0);
        lv_obj_set_style_pad_all(dialog, 6, 0);

        if (current_theme) {
            lv_obj_set_style_bg_color(dialog, current_theme->background_color(), 0);
            lv_obj_set_style_text_color(dialog, current_theme->text_color(), 0);
            lv_obj_set_style_border_color(dialog, current_theme->border_color(), 0);
            lv_obj_set_style_border_width(dialog, 1, 0);
        } else {
            lv_obj_set_style_bg_color(dialog, lv_color_hex(0x1F1F1F), 0);
        }

        lv_obj_set_flex_flow(dialog, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_flex_align(dialog, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

        // Title
        auto modal_text_font = ui->GetTextFont();
        lv_obj_t* title_lbl = lv_label_create(dialog);
        ConfigureWrapLabel(title_lbl, modal_text_font, LV_PCT(100), LV_TEXT_ALIGN_CENTER);
        lv_label_set_text(title_lbl, "Nhập OTA URL:");

        // Text area
        lv_obj_t* ta = lv_textarea_create(dialog);
        lv_obj_set_width(ta, LV_PCT(100));
        lv_textarea_set_one_line(ta, true);

        Settings wifi_settings("wifi", false);
        std::string ota_url = wifi_settings.GetString("ota_url", "");
        lv_textarea_set_text(ta, ota_url.c_str());

        // Buttons row
        lv_obj_t* btn_row = lv_obj_create(dialog);
        lv_obj_set_size(btn_row, LV_PCT(100), 32);
        lv_obj_set_style_pad_all(btn_row, 0, 0);
        lv_obj_set_style_border_width(btn_row, 0, 0);
        lv_obj_set_style_bg_opa(btn_row, LV_OPA_TRANSP, 0);
        lv_obj_set_flex_flow(btn_row, LV_FLEX_FLOW_ROW);
        lv_obj_set_flex_align(btn_row, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

        // Cancel button
        lv_obj_t* cancel = lv_button_create(btn_row);
        lv_obj_set_size(cancel, 75, 28);
        lv_obj_t* cancel_lbl = lv_label_create(cancel);
        ConfigureButtonLabel(cancel_lbl, modal_text_font);
        lv_label_set_text(cancel_lbl, "Hủy");
        lv_obj_center(cancel_lbl);
        lv_obj_add_event_cb(cancel, [](lv_event_t* ev) {
            auto* touch_ui = static_cast<LvglTouchUi*>(lv_event_get_user_data(ev));
            touch_ui->DismissModalOverlay();
        }, LV_EVENT_CLICKED, ui);

        // Save button
        lv_obj_t* save = lv_button_create(btn_row);
        lv_obj_set_size(save, 75, 28);
        lv_obj_t* save_lbl = lv_label_create(save);
        ConfigureButtonLabel(save_lbl, modal_text_font);
        lv_label_set_text(save_lbl, "Lưu");
        lv_obj_center(save_lbl);
        lv_obj_add_event_cb(save, [](lv_event_t* ev) {
            auto* touch_ui = static_cast<LvglTouchUi*>(lv_event_get_user_data(ev));
            lv_obj_t* b = (lv_obj_t*)lv_event_get_target(ev);
            lv_obj_t* r = lv_obj_get_parent(b);
            lv_obj_t* d = lv_obj_get_parent(r);
            lv_obj_t* target_ta = lv_obj_get_child(d, 1);

            const char* val_url = lv_textarea_get_text(target_ta);
            Settings settings_rw("wifi", true);
            settings_rw.SetString("ota_url", val_url);

            touch_ui->DismissModalOverlay();

            touch_ui->ShowNotification("Đã lưu OTA URL");
        }, LV_EVENT_CLICKED, ui);

        // 3. Keyboard overlay
        lv_obj_t* keyboard_obj = lv_keyboard_create(lv_screen_active());
        ui->modal_keyboard_ = keyboard_obj;
        lv_keyboard_set_textarea(keyboard_obj, ta);
        ui->ApplyReadableTextColors();
    }, LV_EVENT_CLICKED, this);

    // Item 4: WiFi Setup
    lv_obj_t* wifi_row = create_settings_row(settings_layout);

    lv_obj_t* wifi_lbl = lv_label_create(wifi_row);
    lv_obj_set_flex_grow(wifi_lbl, 1);
    ConfigureWrapLabel(wifi_lbl, text_font, 1);
    lv_label_set_text(wifi_lbl, "Thiết lập WiFi:");

    lv_obj_t* wifi_btn = lv_button_create(wifi_row);
    lv_obj_set_size(wifi_btn, 72, 30);
    lv_obj_t* wifi_btn_lbl = lv_label_create(wifi_btn);
    ConfigureButtonLabel(wifi_btn_lbl, text_font);
    lv_label_set_text(wifi_btn_lbl, "Quét");
    lv_obj_center(wifi_btn_lbl);
    lv_obj_add_event_cb(wifi_btn, [](lv_event_t* e) {
        auto* ui = static_cast<LvglTouchUi*>(lv_event_get_user_data(e));
        ui->ShowWifiSetupPage();
    }, LV_EVENT_CLICKED, this);

    // Back Button at the bottom
    lv_obj_t* back_row = lv_obj_create(settings_layout);
    lv_obj_set_size(back_row, LV_PCT(100), 35);
    lv_obj_set_style_pad_all(back_row, 0, 0);
    lv_obj_set_style_border_width(back_row, 0, 0);
    lv_obj_set_style_bg_opa(back_row, LV_OPA_TRANSP, 0);
    lv_obj_set_flex_flow(back_row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(back_row, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_t* back_btn = lv_button_create(back_row);
    lv_obj_set_size(back_btn, 112, 30);
    lv_obj_t* back_lbl = lv_label_create(back_btn);
    ConfigureButtonLabel(back_lbl, text_font);
    lv_label_set_text(back_lbl, FONT_AWESOME_ARROW_LEFT " Trở về");
    lv_obj_center(back_lbl);
    lv_obj_add_event_cb(back_btn, [](lv_event_t* e) {
        auto* ui = static_cast<LvglTouchUi*>(lv_event_get_user_data(e));
        ui->ShowMainGridPage();
    }, LV_EVENT_CLICKED, this);

    ApplyReadableTextColors();
}


void LvglTouchUi::ShowAboutPage() {
    DisplayLockGuard lock(this);
    ResetPageWidgets();

    active_page_ = PageType::kPageAbout;
    lv_obj_clean(page_container);

    auto* theme = dynamic_cast<LvglTheme*>(current_theme_);
    auto text_font = GetTextFont();

    lv_obj_t* about_layout = lv_obj_create(page_container);
    lv_obj_set_size(about_layout, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_radius(about_layout, 0, 0);
    lv_obj_set_style_pad_all(about_layout, 6, 0);
    lv_obj_set_style_border_width(about_layout, 0, 0);
    lv_obj_set_style_bg_opa(about_layout, LV_OPA_TRANSP, 0);
    lv_obj_set_flex_flow(about_layout, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(about_layout, 8, 0);
    lv_obj_set_scroll_dir(about_layout, LV_DIR_VER);
    lv_obj_set_scrollbar_mode(about_layout, LV_SCROLLBAR_MODE_AUTO);

    auto create_about_row = [&](const char* label_text, const std::string& value_text) {
        lv_obj_t* row = lv_obj_create(about_layout);
        lv_obj_set_size(row, LV_PCT(100), LV_SIZE_CONTENT);
        lv_obj_set_style_min_height(row, 32, 0);
        lv_obj_set_style_radius(row, 4, 0);
        lv_obj_set_style_pad_left(row, 6, 0);
        lv_obj_set_style_pad_right(row, 6, 0);
        lv_obj_set_style_pad_top(row, 2, 0);
        lv_obj_set_style_pad_bottom(row, 2, 0);

        if (theme) {
            lv_obj_set_style_bg_color(row, theme->assistant_bubble_color(), 0);
            lv_obj_set_style_bg_opa(row, LV_OPA_40, 0);
            lv_obj_set_style_border_color(row, theme->border_color(), 0);
            lv_obj_set_style_border_width(row, 1, 0);
        } else {
            lv_obj_set_style_bg_color(row, lv_color_hex(0x2D2D2D), 0);
            lv_obj_set_style_border_width(row, 0, 0);
        }

        lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
        lv_obj_set_flex_align(row, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

        lv_obj_t* lbl = lv_label_create(row);
        ConfigureWrapLabel(lbl, text_font, LV_PCT(40));
        lv_label_set_text(lbl, label_text);

        lv_obj_t* val = lv_label_create(row);
        ConfigureScrollLabel(val, text_font, LV_PCT(56), LV_TEXT_ALIGN_RIGHT);
        lv_label_set_text(val, value_text.c_str());

        return row;
    };

    std::string board_type = Board::GetInstance().GetBoardType();
    std::string uuid = Board::GetInstance().GetUuid();
    std::string mac = SystemInfo::GetMacAddress();

    std::string ip = "Chưa kết nối";
    if (WifiManager::GetInstance().IsConnected()) {
        ip = WifiManager::GetInstance().GetIpAddress();
    }

    float cpu_temp = 0.0f;
    std::string temp_str = "N/A";
    if (Board::GetInstance().GetTemperature(cpu_temp)) {
        char buf[32];
        snprintf(buf, sizeof(buf), "%.1f C", cpu_temp);
        temp_str = buf;
    }

    const esp_app_desc_t* app_desc = esp_app_get_description();
    std::string fw_version = app_desc ? app_desc->version : "Unknown";
    const char* storage_status = Board::GetInstance().GetStorageStatusText();

    create_about_row("Thiết bị:", board_type);
    create_about_row("UUID:", uuid);
    create_about_row("Địa chỉ MAC:", mac);
    create_about_row("Địa chỉ IP:", ip);
    create_about_row("Nhiệt độ CPU:", temp_str);
    create_about_row("Firmware:", fw_version);
    if (storage_status != nullptr) {
        create_about_row("Lưu trữ:", storage_status);
    }

    lv_obj_t* back_row = lv_obj_create(about_layout);
    lv_obj_set_size(back_row, LV_PCT(100), 35);
    lv_obj_set_style_pad_all(back_row, 0, 0);
    lv_obj_set_style_border_width(back_row, 0, 0);
    lv_obj_set_style_bg_opa(back_row, LV_OPA_TRANSP, 0);
    lv_obj_set_flex_flow(back_row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(back_row, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_t* back_btn = lv_button_create(back_row);
    lv_obj_set_size(back_btn, 112, 30);
    lv_obj_t* back_lbl = lv_label_create(back_btn);
    ConfigureButtonLabel(back_lbl, text_font);
    lv_label_set_text(back_lbl, FONT_AWESOME_ARROW_LEFT " Trở về");
    lv_obj_center(back_lbl);
    lv_obj_add_event_cb(back_btn, [](lv_event_t* e) {
        auto* ui = static_cast<LvglTouchUi*>(lv_event_get_user_data(e));
        ui->ShowMainGridPage();
    }, LV_EVENT_CLICKED, this);

    ApplyReadableTextColors();
}

void LvglTouchUi::ShowFileManagerPage() {
    DisplayLockGuard lock(this);
    ResetPageWidgets();
    active_page_ = PageType::kPageFileManager;
    lv_obj_clean(page_container);

    auto* theme = dynamic_cast<LvglTheme*>(current_theme_);
    auto text_font = GetTextFont();
    auto large_icon_font = GetLargeIconFont();

    lv_obj_t* layout = lv_obj_create(page_container);
    lv_obj_set_size(layout, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_radius(layout, 0, 0);
    lv_obj_set_style_pad_all(layout, 6, 0);
    lv_obj_set_style_border_width(layout, 0, 0);
    lv_obj_set_style_bg_opa(layout, LV_OPA_TRANSP, 0);
    lv_obj_set_flex_flow(layout, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(layout, 6, 0);

    lv_obj_t* header = lv_obj_create(layout);
    lv_obj_set_size(header, LV_PCT(100), 34);
    lv_obj_set_style_radius(header, 0, 0);
    lv_obj_set_style_pad_all(header, 0, 0);
    lv_obj_set_style_border_width(header, 0, 0);
    lv_obj_set_style_bg_opa(header, LV_OPA_TRANSP, 0);
    lv_obj_set_flex_flow(header, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(header, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_t* title = lv_label_create(header);
    ConfigureWrapLabel(title, text_font, 150, LV_TEXT_ALIGN_LEFT);
    lv_label_set_text(title, "Quản lý tệp");

    lv_obj_t* back_btn = lv_button_create(header);
    lv_obj_set_size(back_btn, 72, 30);
    lv_obj_t* back_lbl = lv_label_create(back_btn);
    ConfigureButtonLabel(back_lbl, text_font);
    lv_label_set_text(back_lbl, FONT_AWESOME_ARROW_LEFT " Trở về");
    lv_obj_center(back_lbl);
    lv_obj_add_event_cb(back_btn, [](lv_event_t* e) {
        auto* ui = static_cast<LvglTouchUi*>(lv_event_get_user_data(e));
        ui->ShowMainGridPage();
    }, LV_EVENT_CLICKED, this);

    lv_obj_t* status_card = lv_obj_create(layout);
    lv_obj_set_size(status_card, LV_PCT(100), 44);
    lv_obj_set_style_radius(status_card, 8, 0);
    lv_obj_set_style_pad_all(status_card, 6, 0);
    lv_obj_set_style_border_width(status_card, 1, 0);
    lv_obj_set_flex_flow(status_card, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(status_card, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    if (theme) {
        lv_obj_set_style_bg_color(status_card, theme->assistant_bubble_color(), 0);
        lv_obj_set_style_bg_opa(status_card, LV_OPA_70, 0);
        lv_obj_set_style_border_color(status_card, theme->border_color(), 0);
    }

    lv_obj_t* status_icon = lv_label_create(status_card);
    lv_obj_set_style_text_font(status_icon, large_icon_font, 0);
    lv_label_set_text(status_icon, FONT_AWESOME_SD_CARD);

    lv_obj_t* status_text = lv_label_create(status_card);
    ConfigureWrapLabel(status_text, text_font, LV_PCT(100), LV_TEXT_ALIGN_LEFT);
    lv_label_set_text(status_text, "Giao diện tệp mẫu - chưa gắn danh sách SD thật");

    lv_obj_t* list = lv_obj_create(layout);
    lv_obj_set_width(list, LV_PCT(100));
    lv_obj_set_flex_grow(list, 1);
    lv_obj_set_style_radius(list, 0, 0);
    lv_obj_set_style_pad_all(list, 0, 0);
    lv_obj_set_style_border_width(list, 0, 0);
    lv_obj_set_style_bg_opa(list, LV_OPA_TRANSP, 0);
    lv_obj_set_flex_flow(list, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(list, 6, 0);

    auto create_file_row = [&](const char* icon, const char* name, const char* meta) {
        lv_obj_t* row = lv_obj_create(list);
        lv_obj_set_size(row, LV_PCT(100), 48);
        lv_obj_set_style_radius(row, 8, 0);
        lv_obj_set_style_pad_all(row, 6, 0);
        lv_obj_set_style_border_width(row, 1, 0);
        lv_obj_set_flex_flow(row, LV_FLEX_FLOW_ROW);
        lv_obj_set_flex_align(row, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        if (theme) {
            lv_obj_set_style_bg_color(row, theme->assistant_bubble_color(), 0);
            lv_obj_set_style_bg_opa(row, LV_OPA_50, 0);
            lv_obj_set_style_border_color(row, theme->border_color(), 0);
        }

        lv_obj_t* row_icon = lv_label_create(row);
        lv_obj_set_style_text_font(row_icon, large_icon_font, 0);
        lv_label_set_text(row_icon, icon);

        lv_obj_t* row_texts = lv_obj_create(row);
        lv_obj_set_flex_grow(row_texts, 1);
        lv_obj_set_style_radius(row_texts, 0, 0);
        lv_obj_set_style_pad_all(row_texts, 0, 0);
        lv_obj_set_style_border_width(row_texts, 0, 0);
        lv_obj_set_style_bg_opa(row_texts, LV_OPA_TRANSP, 0);
        lv_obj_set_flex_flow(row_texts, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_style_pad_row(row_texts, 0, 0);

        lv_obj_t* row_name = lv_label_create(row_texts);
        ConfigureWrapLabel(row_name, text_font, LV_PCT(100), LV_TEXT_ALIGN_LEFT);
        lv_label_set_text(row_name, name);

        lv_obj_t* row_meta = lv_label_create(row_texts);
        ConfigureWrapLabel(row_meta, text_font, LV_PCT(100), LV_TEXT_ALIGN_LEFT);
        lv_label_set_text(row_meta, meta);
    };

    create_file_row(FONT_AWESOME_MUSIC, "music/", "Danh sách mẫu - chờ nối SD");
    create_file_row(FONT_AWESOME_IMAGE, "video/", "Danh sách mẫu - chờ nối SD");
    create_file_row(FONT_AWESOME_COMMENT, "recordings/", "Danh sách mẫu - chờ nối SD");

    ApplyReadableTextColors();
}

void LvglTouchUi::ShowMusicPlayerPage() {
    DisplayLockGuard lock(this);
    ResetPageWidgets();
    active_page_ = PageType::kPageMusicPlayer;
    lv_obj_clean(page_container);

    auto* theme = dynamic_cast<LvglTheme*>(current_theme_);
    auto text_font = GetTextFont();
    auto large_icon_font = GetLargeIconFont();

    lv_obj_t* layout = lv_obj_create(page_container);
    lv_obj_set_size(layout, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_radius(layout, 0, 0);
    lv_obj_set_style_pad_all(layout, 6, 0);
    lv_obj_set_style_border_width(layout, 0, 0);
    lv_obj_set_style_bg_opa(layout, LV_OPA_TRANSP, 0);
    lv_obj_set_flex_flow(layout, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(layout, 6, 0);

    lv_obj_t* header = lv_obj_create(layout);
    lv_obj_set_size(header, LV_PCT(100), 34);
    lv_obj_set_style_radius(header, 0, 0);
    lv_obj_set_style_pad_all(header, 0, 0);
    lv_obj_set_style_border_width(header, 0, 0);
    lv_obj_set_style_bg_opa(header, LV_OPA_TRANSP, 0);
    lv_obj_set_flex_flow(header, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(header, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_t* title = lv_label_create(header);
    ConfigureWrapLabel(title, text_font, 150, LV_TEXT_ALIGN_LEFT);
    lv_label_set_text(title, "Phát nhạc");

    lv_obj_t* back_btn = lv_button_create(header);
    lv_obj_set_size(back_btn, 72, 30);
    lv_obj_t* back_lbl = lv_label_create(back_btn);
    ConfigureButtonLabel(back_lbl, text_font);
    lv_label_set_text(back_lbl, FONT_AWESOME_ARROW_LEFT " Trở về");
    lv_obj_center(back_lbl);
    lv_obj_add_event_cb(back_btn, [](lv_event_t* e) {
        auto* ui = static_cast<LvglTouchUi*>(lv_event_get_user_data(e));
        ui->ShowMainGridPage();
    }, LV_EVENT_CLICKED, this);

    lv_obj_t* player_card = lv_obj_create(layout);
    lv_obj_set_width(player_card, LV_PCT(100));
    lv_obj_set_flex_grow(player_card, 1);
    lv_obj_set_style_radius(player_card, 8, 0);
    lv_obj_set_style_pad_all(player_card, 8, 0);
    lv_obj_set_style_border_width(player_card, 1, 0);
    lv_obj_set_flex_flow(player_card, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(player_card, 6, 0);
    if (theme) {
        lv_obj_set_style_bg_color(player_card, theme->assistant_bubble_color(), 0);
        lv_obj_set_style_bg_opa(player_card, LV_OPA_70, 0);
        lv_obj_set_style_border_color(player_card, theme->border_color(), 0);
    }

    lv_obj_t* art = lv_obj_create(player_card);
    lv_obj_set_size(art, LV_PCT(100), 72);
    lv_obj_set_style_radius(art, 8, 0);
    lv_obj_set_style_pad_all(art, 0, 0);
    lv_obj_set_style_border_width(art, 0, 0);
    lv_obj_set_style_bg_opa(art, LV_OPA_40, 0);
    lv_obj_set_flex_flow(art, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(art, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_t* art_icon = lv_label_create(art);
    lv_obj_set_style_text_font(art_icon, large_icon_font, 0);
    lv_label_set_text(art_icon, FONT_AWESOME_MUSIC);

    lv_obj_t* song_name = lv_label_create(player_card);
    ConfigureWrapLabel(song_name, text_font, LV_PCT(100), LV_TEXT_ALIGN_CENTER);
    lv_label_set_text(song_name, "Bài hát mẫu.wav");

    lv_obj_t* song_meta = lv_label_create(player_card);
    ConfigureWrapLabel(song_meta, text_font, LV_PCT(100), LV_TEXT_ALIGN_CENTER);
    lv_label_set_text(song_meta, "MP3 / WAV từ thẻ SD");

    lv_obj_t* progress = lv_bar_create(player_card);
    lv_obj_set_width(progress, LV_PCT(100));
    lv_bar_set_range(progress, 0, 100);
    lv_bar_set_value(progress, 34, LV_ANIM_OFF);

    lv_obj_t* time_row = lv_obj_create(player_card);
    lv_obj_set_size(time_row, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_style_radius(time_row, 0, 0);
    lv_obj_set_style_pad_all(time_row, 0, 0);
    lv_obj_set_style_border_width(time_row, 0, 0);
    lv_obj_set_style_bg_opa(time_row, LV_OPA_TRANSP, 0);
    lv_obj_set_flex_flow(time_row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(time_row, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_t* time_left = lv_label_create(time_row);
    ConfigureWrapLabel(time_left, text_font, 60, LV_TEXT_ALIGN_LEFT);
    lv_label_set_text(time_left, "00:34");

    lv_obj_t* time_right = lv_label_create(time_row);
    ConfigureWrapLabel(time_right, text_font, 60, LV_TEXT_ALIGN_RIGHT);
    lv_label_set_text(time_right, "03:12");

    lv_obj_t* controls = lv_obj_create(player_card);
    lv_obj_set_size(controls, LV_PCT(100), 42);
    lv_obj_set_style_radius(controls, 0, 0);
    lv_obj_set_style_pad_all(controls, 0, 0);
    lv_obj_set_style_border_width(controls, 0, 0);
    lv_obj_set_style_bg_opa(controls, LV_OPA_TRANSP, 0);
    lv_obj_set_flex_flow(controls, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(controls, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    auto create_control = [&](const char* icon, int width) {
        lv_obj_t* btn = lv_button_create(controls);
        lv_obj_set_size(btn, width, 36);
        lv_obj_t* lbl = lv_label_create(btn);
        ConfigureButtonLabel(lbl, text_font);
        lv_label_set_text(lbl, icon);
        lv_obj_center(lbl);
        lv_obj_add_state(btn, LV_STATE_DISABLED);
        return btn;
    };

    create_control(FONT_AWESOME_BACKWARD_STEP, 60);
    lv_obj_t* play_btn = create_control(FONT_AWESOME_PLAY, 72);
    create_control(FONT_AWESOME_FORWARD_STEP, 60);

    lv_obj_t* volume_row = lv_obj_create(player_card);
    lv_obj_set_size(volume_row, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_style_radius(volume_row, 0, 0);
    lv_obj_set_style_pad_all(volume_row, 0, 0);
    lv_obj_set_style_border_width(volume_row, 0, 0);
    lv_obj_set_style_bg_opa(volume_row, LV_OPA_TRANSP, 0);
    lv_obj_set_flex_flow(volume_row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(volume_row, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_t* volume_icon = lv_label_create(volume_row);
    lv_obj_set_style_text_font(volume_icon, large_icon_font, 0);
    lv_label_set_text(volume_icon, FONT_AWESOME_VOLUME_HIGH);

    lv_obj_t* volume_bar = lv_bar_create(volume_row);
    lv_obj_set_flex_grow(volume_bar, 1);
    lv_bar_set_range(volume_bar, 0, 100);
    lv_bar_set_value(volume_bar, 70, LV_ANIM_OFF);

    lv_obj_t* volume_value = lv_label_create(volume_row);
    ConfigureWrapLabel(volume_value, text_font, 32, LV_TEXT_ALIGN_RIGHT);
    lv_label_set_text(volume_value, "70");

    lv_obj_add_event_cb(play_btn, [](lv_event_t* e) {
        auto* ui = static_cast<LvglTouchUi*>(lv_event_get_user_data(e));
        ui->ShowNotification("Bản dựng giao diện chưa phát nhạc thật");
    }, LV_EVENT_CLICKED, this);

    ApplyReadableTextColors();
}

void LvglTouchUi::ShowVideoPlayerPage() {
    DisplayLockGuard lock(this);
    ResetPageWidgets();
    active_page_ = PageType::kPageVideoPlayer;
    lv_obj_clean(page_container);

    auto* theme = dynamic_cast<LvglTheme*>(current_theme_);
    auto text_font = GetTextFont();
    auto large_icon_font = GetLargeIconFont();

    lv_obj_t* layout = lv_obj_create(page_container);
    lv_obj_set_size(layout, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_radius(layout, 0, 0);
    lv_obj_set_style_pad_all(layout, 6, 0);
    lv_obj_set_style_border_width(layout, 0, 0);
    lv_obj_set_style_bg_opa(layout, LV_OPA_TRANSP, 0);
    lv_obj_set_flex_flow(layout, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_style_pad_row(layout, 6, 0);

    lv_obj_t* header = lv_obj_create(layout);
    lv_obj_set_size(header, LV_PCT(100), 34);
    lv_obj_set_style_radius(header, 0, 0);
    lv_obj_set_style_pad_all(header, 0, 0);
    lv_obj_set_style_border_width(header, 0, 0);
    lv_obj_set_style_bg_opa(header, LV_OPA_TRANSP, 0);
    lv_obj_set_flex_flow(header, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(header, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_t* title = lv_label_create(header);
    ConfigureWrapLabel(title, text_font, 150, LV_TEXT_ALIGN_LEFT);
    lv_label_set_text(title, "Phát video");

    lv_obj_t* back_btn = lv_button_create(header);
    lv_obj_set_size(back_btn, 72, 30);
    lv_obj_t* back_lbl = lv_label_create(back_btn);
    ConfigureButtonLabel(back_lbl, text_font);
    lv_label_set_text(back_lbl, FONT_AWESOME_ARROW_LEFT " Trở về");
    lv_obj_center(back_lbl);
    lv_obj_add_event_cb(back_btn, [](lv_event_t* e) {
        auto* ui = static_cast<LvglTouchUi*>(lv_event_get_user_data(e));
        ui->ShowMainGridPage();
    }, LV_EVENT_CLICKED, this);

    lv_obj_t* preview = lv_obj_create(layout);
    lv_obj_set_width(preview, LV_PCT(100));
    lv_obj_set_height(preview, 88);
    lv_obj_set_style_radius(preview, 8, 0);
    lv_obj_set_style_pad_all(preview, 6, 0);
    lv_obj_set_style_border_width(preview, 1, 0);
    lv_obj_set_flex_flow(preview, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(preview, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
    if (theme) {
        lv_obj_set_style_bg_color(preview, theme->assistant_bubble_color(), 0);
        lv_obj_set_style_bg_opa(preview, LV_OPA_70, 0);
        lv_obj_set_style_border_color(preview, theme->border_color(), 0);
    }

    lv_obj_t* preview_icon = lv_label_create(preview);
    lv_obj_set_style_text_font(preview_icon, large_icon_font, 0);
    lv_label_set_text(preview_icon, FONT_AWESOME_IMAGE);

    lv_obj_t* preview_text = lv_label_create(preview);
    ConfigureWrapLabel(preview_text, text_font, LV_PCT(100), LV_TEXT_ALIGN_CENTER);
    lv_label_set_text(preview_text, "video-sample.mp4");

    lv_obj_t* video_meta = lv_label_create(layout);
    ConfigureWrapLabel(video_meta, text_font, LV_PCT(100), LV_TEXT_ALIGN_CENTER);
    lv_label_set_text(video_meta, "MP4 / H.264 từ thẻ SD");

    lv_obj_t* progress = lv_bar_create(layout);
    lv_obj_set_width(progress, LV_PCT(100));
    lv_bar_set_range(progress, 0, 100);
    lv_bar_set_value(progress, 18, LV_ANIM_OFF);

    lv_obj_t* time_row = lv_obj_create(layout);
    lv_obj_set_size(time_row, LV_PCT(100), LV_SIZE_CONTENT);
    lv_obj_set_style_radius(time_row, 0, 0);
    lv_obj_set_style_pad_all(time_row, 0, 0);
    lv_obj_set_style_border_width(time_row, 0, 0);
    lv_obj_set_style_bg_opa(time_row, LV_OPA_TRANSP, 0);
    lv_obj_set_flex_flow(time_row, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(time_row, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    lv_obj_t* time_left = lv_label_create(time_row);
    ConfigureWrapLabel(time_left, text_font, 60, LV_TEXT_ALIGN_LEFT);
    lv_label_set_text(time_left, "00:18");

    lv_obj_t* time_right = lv_label_create(time_row);
    ConfigureWrapLabel(time_right, text_font, 60, LV_TEXT_ALIGN_RIGHT);
    lv_label_set_text(time_right, "05:00");

    lv_obj_t* controls = lv_obj_create(layout);
    lv_obj_set_size(controls, LV_PCT(100), 42);
    lv_obj_set_style_radius(controls, 0, 0);
    lv_obj_set_style_pad_all(controls, 0, 0);
    lv_obj_set_style_border_width(controls, 0, 0);
    lv_obj_set_style_bg_opa(controls, LV_OPA_TRANSP, 0);
    lv_obj_set_flex_flow(controls, LV_FLEX_FLOW_ROW);
    lv_obj_set_flex_align(controls, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    auto create_control = [&](const char* icon, int width) {
        lv_obj_t* btn = lv_button_create(controls);
        lv_obj_set_size(btn, width, 36);
        lv_obj_t* lbl = lv_label_create(btn);
        ConfigureButtonLabel(lbl, text_font);
        lv_label_set_text(lbl, icon);
        lv_obj_center(lbl);
        lv_obj_add_state(btn, LV_STATE_DISABLED);
        return btn;
    };

    create_control(FONT_AWESOME_BACKWARD_STEP, 60);
    create_control(FONT_AWESOME_PLAY, 72);
    create_control(FONT_AWESOME_FORWARD_STEP, 60);

    ApplyReadableTextColors();
}

void LvglTouchUi::ShowWifiSetupPage() {

    {
        DisplayLockGuard lock(this);
        ResetPageWidgets();

        active_page_ = PageType::kPageWifiSetup;
        wifi_scan_has_results_ = false;
        selected_wifi_ssid_.clear();

        lv_obj_clean(page_container);

        auto text_font = GetTextFont();

        lv_obj_t* setup_layout = lv_obj_create(page_container);
        lv_obj_set_size(setup_layout, LV_PCT(100), LV_PCT(100));
        lv_obj_set_style_radius(setup_layout, 0, 0);
        lv_obj_set_style_pad_all(setup_layout, 4, 0);
        lv_obj_set_style_border_width(setup_layout, 0, 0);
        lv_obj_set_style_bg_opa(setup_layout, LV_OPA_TRANSP, 0);
        lv_obj_set_flex_flow(setup_layout, LV_FLEX_FLOW_COLUMN);
        lv_obj_set_flex_align(setup_layout, LV_FLEX_ALIGN_START, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);
        lv_obj_set_style_pad_row(setup_layout, 4, 0);
        lv_obj_set_scroll_dir(setup_layout, LV_DIR_VER);
        lv_obj_set_scrollbar_mode(setup_layout, LV_SCROLLBAR_MODE_AUTO);

        lv_obj_t* select_label = lv_label_create(setup_layout);
        ConfigureWrapLabel(select_label, text_font, LV_PCT(92), LV_TEXT_ALIGN_CENTER);
        lv_label_set_text(select_label, "Chọn WiFi");

        wifi_roller = lv_roller_create(setup_layout);
        lv_obj_set_width(wifi_roller, LV_PCT(92));
        lv_roller_set_options(wifi_roller, "Đang quét mạng...", LV_ROLLER_MODE_NORMAL);
        lv_roller_set_visible_row_count(wifi_roller, 4);
        lv_obj_set_style_text_font(wifi_roller, text_font, 0);

        wifi_password_label_ = lv_label_create(setup_layout);
        ConfigureWrapLabel(wifi_password_label_, text_font, LV_PCT(92), LV_TEXT_ALIGN_CENTER);
        lv_label_set_text(wifi_password_label_, "Mật khẩu");
        lv_obj_add_flag(wifi_password_label_, LV_OBJ_FLAG_HIDDEN);

        password_textarea = lv_textarea_create(setup_layout);
        lv_obj_set_width(password_textarea, LV_PCT(92));
        lv_textarea_set_password_mode(password_textarea, true);
        lv_textarea_set_one_line(password_textarea, true);
        lv_textarea_set_placeholder_text(password_textarea, "Mật khẩu");
        lv_obj_set_style_text_font(password_textarea, text_font, 0);
        lv_obj_add_flag(password_textarea, LV_OBJ_FLAG_HIDDEN);

        lv_obj_t* btn_row = lv_obj_create(setup_layout);
        lv_obj_set_size(btn_row, LV_PCT(92), 40);
        lv_obj_set_style_pad_all(btn_row, 0, 0);
        lv_obj_set_style_border_width(btn_row, 0, 0);
        lv_obj_set_style_bg_opa(btn_row, LV_OPA_TRANSP, 0);
        lv_obj_set_flex_flow(btn_row, LV_FLEX_FLOW_ROW);
        lv_obj_set_flex_align(btn_row, LV_FLEX_ALIGN_SPACE_BETWEEN, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

        lv_obj_t* back_btn = lv_button_create(btn_row);
        lv_obj_set_size(back_btn, 72, 32);
        lv_obj_t* back_label = lv_label_create(back_btn);
        ConfigureButtonLabel(back_label, text_font);
        lv_label_set_text(back_label, "Hủy");
        lv_obj_center(back_label);
        lv_obj_add_event_cb(back_btn, [](lv_event_t* e) {
            auto* ui = static_cast<LvglTouchUi*>(lv_event_get_user_data(e));
            if (ui->keyboard) {
                lv_obj_add_flag(ui->keyboard, LV_OBJ_FLAG_HIDDEN);
            }
            ui->ShowMainGridPage();
        }, LV_EVENT_CLICKED, this);

        wifi_select_btn_ = lv_button_create(btn_row);
        lv_obj_set_size(wifi_select_btn_, 72, 32);
        lv_obj_add_state(wifi_select_btn_, LV_STATE_DISABLED);
        lv_obj_t* select_btn_label = lv_label_create(wifi_select_btn_);
        ConfigureButtonLabel(select_btn_label, text_font);
        lv_label_set_text(select_btn_label, "Tiếp");
        lv_obj_center(select_btn_label);
        lv_obj_add_event_cb(wifi_select_btn_, [](lv_event_t* e) {
            auto* ui = static_cast<LvglTouchUi*>(lv_event_get_user_data(e));
            char ssid[64];
            lv_roller_get_selected_str(ui->wifi_roller, ssid, sizeof(ssid));
            if (!CanConnectToWifiSsid(ui->wifi_scan_has_results_, ssid)) {
                ui->ShowNotification("Chọn mạng WiFi hợp lệ");
                return;
            }
            ui->ShowWifiPasswordStep(ssid);
        }, LV_EVENT_CLICKED, this);

        wifi_connect_btn_ = lv_button_create(btn_row);
        lv_obj_set_size(wifi_connect_btn_, 88, 32);
        lv_obj_add_flag(wifi_connect_btn_, LV_OBJ_FLAG_HIDDEN);
        lv_obj_t* conn_label = lv_label_create(wifi_connect_btn_);
        ConfigureButtonLabel(conn_label, text_font);
        lv_label_set_text(conn_label, "Kết nối");
        lv_obj_center(conn_label);
        lv_obj_add_event_cb(wifi_connect_btn_, [](lv_event_t* e) {
            auto* ui = static_cast<LvglTouchUi*>(lv_event_get_user_data(e));
            std::string ssid = ui->selected_wifi_ssid_;
            std::string password = lv_textarea_get_text(ui->password_textarea);

            if (!CanConnectToWifiSsid(ui->wifi_scan_has_results_, ssid.c_str())) {
                ui->ShowNotification("Chọn mạng WiFi hợp lệ");
                return;
            }

            ESP_LOGI(TAG, "Connecting to AP: %s (Password length: %d)", ssid.c_str(), (int)password.size());
            if (ui->keyboard) {
                lv_obj_add_flag(ui->keyboard, LV_OBJ_FLAG_HIDDEN);
            }

            ui->LeaveWifiSetupPage();
            ui->ShowWifiConnectPage();
            ui->wifi_setup_workflow_.SaveCredentialsAndReconnect(ssid.c_str(), password.c_str());
        }, LV_EVENT_CLICKED, this);

        keyboard = lv_keyboard_create(lv_screen_active());
        lv_obj_add_flag(keyboard, LV_OBJ_FLAG_HIDDEN);

        lv_obj_add_event_cb(password_textarea, [](lv_event_t* e) {
            auto* ui = static_cast<LvglTouchUi*>(lv_event_get_user_data(e));
            lv_event_code_t code = lv_event_get_code(e);
            if (code == LV_EVENT_FOCUSED) {
                lv_keyboard_set_textarea(ui->keyboard, ui->password_textarea);
                lv_obj_remove_flag(ui->keyboard, LV_OBJ_FLAG_HIDDEN);
                lv_obj_scroll_to_view(ui->password_textarea, LV_ANIM_ON);
            } else if (code == LV_EVENT_DEFOCUSED) {
                lv_obj_add_flag(ui->keyboard, LV_OBJ_FLAG_HIDDEN);
            }
        }, LV_EVENT_ALL, this);

        lv_obj_add_event_cb(keyboard, [](lv_event_t* e) {
            lv_event_code_t code = lv_event_get_code(e);
            if (code == LV_EVENT_READY || code == LV_EVENT_CANCEL) {
                auto* target_kb = (lv_obj_t*)lv_event_get_target(e);
                lv_obj_add_flag(target_kb, LV_OBJ_FLAG_HIDDEN);
            }
        }, LV_EVENT_ALL, nullptr);

        ApplyReadableTextColors();
    }

    const uint32_t scan_generation = ++wifi_scan_generation_;
    StartWifiScanTask(scan_generation);
}

void LvglTouchUi::ShowWifiConnectPage() {
    DisplayLockGuard lock(this);
    ResetPageWidgets();

    active_page_ = PageType::kPageWifiConnect;
    lv_obj_clean(page_container);

    auto text_font = GetTextFont();
    auto large_icon_font = GetLargeIconFont();

    lv_obj_t* connect_layout = lv_obj_create(page_container);
    lv_obj_set_size(connect_layout, LV_PCT(100), LV_PCT(100));
    lv_obj_set_style_pad_all(connect_layout, 8, 0);
    lv_obj_set_style_border_width(connect_layout, 0, 0);
    lv_obj_set_style_bg_opa(connect_layout, LV_OPA_TRANSP, 0);
    lv_obj_set_flex_flow(connect_layout, LV_FLEX_FLOW_COLUMN);
    lv_obj_set_flex_align(connect_layout, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER, LV_FLEX_ALIGN_CENTER);

    // Large WiFi logo
    lv_obj_t* wifi_logo = lv_label_create(connect_layout);
    lv_obj_set_style_text_font(wifi_logo, large_icon_font, 0);
    lv_label_set_text(wifi_logo, FONT_AWESOME_WIFI);

    // Status text
    lv_obj_t* status_lbl = lv_label_create(connect_layout);
    ConfigureWrapLabel(status_lbl, text_font, LV_PCT(92), LV_TEXT_ALIGN_CENTER);

    char status_str[128];
    if (wifi_connect_attempts_ > 0) {
        snprintf(status_str, sizeof(status_str), "Đang kết nối đến WiFi...\nLần thử %d/5", wifi_connect_attempts_);
    } else {
        snprintf(status_str, sizeof(status_str), "Đang kết nối đến WiFi...");
    }
    lv_label_set_text(status_lbl, status_str);

    // Spinner
    lv_obj_t* spinner = lv_spinner_create(connect_layout);
    lv_obj_set_size(spinner, 30, 30);
    lv_obj_set_style_arc_width(spinner, 3, LV_PART_MAIN);
    lv_obj_set_style_arc_width(spinner, 3, LV_PART_INDICATOR);

    ApplyReadableTextColors();
}

void LvglTouchUi::SetStatus(const char* status) {
    status_text_ = status ? status : "";
    if (!setup_ui_called_ || time_label == nullptr) {
        return;
    }

    DisplayLockGuard lock(this);
    if (!status_text_.empty()) {
        lv_label_set_text(time_label, status_text_.c_str());
    } else {
        lv_label_set_text(time_label, "--:--");
    }
}

void LvglTouchUi::SetEmotion(const char* emotion) {
    current_emotion_ = emotion ? emotion : "";
    if (!setup_ui_called_ || mic_status_icon == nullptr || current_state_ != kDeviceStateIdle) {
        return;
    }

    UpdateMicStatusIndicator();
}

void LvglTouchUi::SetChatMessage(const char* role, const char* content) {
    ESP_LOGI(TAG, "SetChatMessage: [%s] %s", role ? role : "", content ? content : "");
    if (!content || strlen(content) == 0) return;

    DisplayLockGuard lock(this);

    bool is_chat_active = active_page_ == PageType::kPageChat && chat_box != nullptr;
    bool should_open_chat = ShouldOpenTouchUiChatForRole(role);
    if (!is_chat_active && !should_open_chat) {
        return;
    }

    chat_messages_.push_back({role ? role : "", content});
    constexpr size_t kMaxChatMessages = 50;
    if (chat_messages_.size() > kMaxChatMessages) {
        chat_messages_.erase(chat_messages_.begin(), chat_messages_.begin() + (chat_messages_.size() - kMaxChatMessages));
    }

    if (!is_chat_active) {
        ShowChatPage();
        return;
    }

    AppendChatMessageToView(role ? role : "", content);
}

void LvglTouchUi::ClearChatMessages() {
    ESP_LOGI(TAG, "ClearChatMessages called");
    DisplayLockGuard lock(this);
    chat_messages_.clear();
    if (chat_box) {
        lv_obj_clean(chat_box);
    }
}

void LvglTouchUi::SetTheme(Theme* theme) {
    Display::SetTheme(theme);
    if (display_) {
        display_->SetTheme(theme);
    }

    auto* lvgl_theme = dynamic_cast<LvglTheme*>(current_theme_);
    if (lvgl_theme && master_container) {
        DisplayLockGuard lock(this);
        lv_obj_set_style_bg_color(master_container, lvgl_theme->background_color(), 0);
        lv_obj_set_style_text_color(master_container, lvgl_theme->text_color(), 0);
        if (top_bar) {
            lv_obj_set_style_bg_color(top_bar, lvgl_theme->background_color(), 0);
            lv_obj_set_style_text_color(top_bar, lvgl_theme->text_color(), 0);
        }
        ApplyStatusBarTheme();
    }

    RebuildActivePage();
}

const lv_font_t* LvglTouchUi::GetTextFont() const {
    auto* theme = dynamic_cast<LvglTheme*>(current_theme_);
    auto font = theme ? theme->text_font() : nullptr;
    if (font != nullptr && font->font() != nullptr) {
        return font->font();
    }
    return &BUILTIN_TEXT_FONT;
}

const lv_font_t* LvglTouchUi::GetIconFont() const {
    auto* theme = dynamic_cast<LvglTheme*>(current_theme_);
    auto font = theme ? theme->icon_font() : nullptr;
    if (font != nullptr && font->font() != nullptr) {
        return font->font();
    }
    return &BUILTIN_ICON_FONT;
}

const lv_font_t* LvglTouchUi::GetLargeIconFont() const {
    auto* theme = dynamic_cast<LvglTheme*>(current_theme_);
    auto font = theme ? theme->large_icon_font() : nullptr;
    if (font != nullptr && font->font() != nullptr) {
        return font->font();
    }
    return GetIconFont();
}

void LvglTouchUi::ApplyStatusBarTheme() {
    if (wifi_icon != nullptr) {
        lv_obj_set_style_text_font(wifi_icon, GetIconFont(), 0);
    }
    if (time_label != nullptr) {
        lv_obj_set_style_text_font(time_label, GetTextFont(), 0);
    }
    if (battery_icon != nullptr) {
        lv_obj_set_style_text_font(battery_icon, GetIconFont(), 0);
    }
}

void LvglTouchUi::ApplyReadableTextColors() {
    auto* theme = dynamic_cast<LvglTheme*>(current_theme_);
    if (theme == nullptr || theme->name() != "dark") {
        return;
    }

    lv_color_t text_color = theme->text_color();
    if (master_container != nullptr) {
        lv_obj_tree_walk(master_container, ApplyLabelTextColor, &text_color);
    }
    if (modal_overlay_ != nullptr) {
        lv_obj_tree_walk(modal_overlay_, ApplyLabelTextColor, &text_color);
    }
}

void LvglTouchUi::DismissModalOverlay() {
    if (modal_keyboard_ != nullptr) {
        lv_obj_del(modal_keyboard_);
        modal_keyboard_ = nullptr;
    }
    if (modal_overlay_ != nullptr) {
        lv_obj_del(modal_overlay_);
        modal_overlay_ = nullptr;
    }
}

void LvglTouchUi::LeaveWifiSetupPage() {
    if (active_page_ == PageType::kPageWifiSetup) {
        ++wifi_scan_generation_;
        wifi_scan_has_results_ = false;
        selected_wifi_ssid_.clear();
    }
}

void LvglTouchUi::ResetPageWidgets() {
    DismissModalOverlay();
    LeaveWifiSetupPage();

    if (keyboard) {
        lv_obj_del(keyboard);
        keyboard = nullptr;
    }

    wifi_roller = nullptr;
    password_textarea = nullptr;
    wifi_password_label_ = nullptr;
    wifi_select_btn_ = nullptr;
    wifi_connect_btn_ = nullptr;
    wifi_scan_has_results_ = false;
    selected_wifi_ssid_.clear();
    chat_box = nullptr;
    mic_status_label = nullptr;
    mic_status_icon = nullptr;
}

void LvglTouchUi::RebuildActivePage() {
    switch (active_page_) {
        case PageType::kPageMainGrid: ShowMainGridPage(); break;
        case PageType::kPageChat: ShowChatPage(); break;
        case PageType::kPageSettings: ShowSettingsPage(); break;
        case PageType::kPageAbout: ShowAboutPage(); break;
        case PageType::kPageFileManager: ShowFileManagerPage(); break;
        case PageType::kPageMusicPlayer: ShowMusicPlayerPage(); break;
        case PageType::kPageVideoPlayer: ShowVideoPlayerPage(); break;
        case PageType::kPageWifiSetup: ShowWifiSetupPage(); break;
        case PageType::kPageWifiConnect: ShowWifiConnectPage(); break;
    }
}

void LvglTouchUi::ShowNotification(const char* notification, int duration_ms) {
    SetStatus(notification);
    if (notification_timer_ != nullptr) {
        esp_timer_stop(notification_timer_);
        if (duration_ms > 0) {
            auto ret = esp_timer_start_once(notification_timer_, duration_ms * 1000);
            if (ret != ESP_OK) {
                ESP_LOGW(TAG, "Failed to start notification timer: %s", esp_err_to_name(ret));
            }
        }
    }
}

void LvglTouchUi::NotificationTimerCallback(void* arg) {
    auto* ui = static_cast<LvglTouchUi*>(arg);
    ui->SetStatus("");
    ui->UpdateStatusBar(true);
}

void LvglTouchUi::HandleDeviceStateChange(DeviceState old_state, DeviceState new_state) {
    ESP_LOGI(TAG, "HandleDeviceStateChange: %d -> %d", old_state, new_state);
    current_state_ = new_state;
    wifi_connect_attempts_ = GetNextWifiConnectAttempts(wifi_connect_attempts_, new_state);

    switch (GetTouchUiPageTarget(old_state, new_state)) {
        case TouchUiPageTarget::kWifiConnect:
            ShowWifiConnectPage();
            break;
        case TouchUiPageTarget::kWifiSetup:
            ShowWifiSetupPage();
            break;
        case TouchUiPageTarget::kMainGrid:
            ShowMainGridPage();
            break;
        case TouchUiPageTarget::kNone:
            break;
    }

    UpdateMicStatusIndicator();
}

void LvglTouchUi::StartWifiScanTask(uint32_t generation) {
    auto* payload = new WifiScanTaskPayload();
    payload->ui = this;
    payload->generation = generation;
    payload->scan_ret = ESP_FAIL;

    portENTER_CRITICAL(&wifi_scan_task_mux_);
    wifi_scan_tasks_in_flight_++;
    portEXIT_CRITICAL(&wifi_scan_task_mux_);

    BaseType_t task_created = xTaskCreate([](void* arg) {
        LvglTouchUi::WifiScanTask(arg);
    }, "wifi_ui_scan", 4096, payload, 3, nullptr);

    if (task_created != pdPASS) {
        ESP_LOGE(TAG, "Failed to start WiFi scan task");
        delete payload;

        portENTER_CRITICAL(&wifi_scan_task_mux_);
        if (wifi_scan_tasks_in_flight_ > 0) {
            wifi_scan_tasks_in_flight_--;
        }
        portEXIT_CRITICAL(&wifi_scan_task_mux_);

        DisplayLockGuard lock(this);
        UpdateWifiScanResults({}, ESP_ERR_NO_MEM);
        ApplyReadableTextColors();
        return;
    }
}

void LvglTouchUi::WifiScanTask(void* arg) {
    auto* payload = static_cast<WifiScanTaskPayload*>(arg);
    auto* ui = payload != nullptr ? payload->ui : nullptr;

    if (payload == nullptr || ui == nullptr) {
        delete payload;
        vTaskDelete(nullptr);
        return;
    }

    payload->scan_ret = ui->wifi_setup_workflow_.ScanSsids(payload->ssids);

    bool should_shutdown = false;
    portENTER_CRITICAL(&ui->wifi_scan_task_mux_);
    should_shutdown = ui->wifi_scan_shutdown_;
    portEXIT_CRITICAL(&ui->wifi_scan_task_mux_);

    if (!should_shutdown) {
        DisplayLockGuard lock(ui);
        if (ui->active_page_ == PageType::kPageWifiSetup && payload->generation == ui->wifi_scan_generation_) {
            ui->UpdateWifiScanResults(payload->ssids, payload->scan_ret);
            ui->ApplyReadableTextColors();
        }
    }

    portENTER_CRITICAL(&ui->wifi_scan_task_mux_);
    if (ui->wifi_scan_tasks_in_flight_ > 0) {
        ui->wifi_scan_tasks_in_flight_--;
    }
    portEXIT_CRITICAL(&ui->wifi_scan_task_mux_);

    delete payload;
    vTaskDelete(nullptr);
}

void LvglTouchUi::ShowWifiPasswordStep(const char* ssid) {
    selected_wifi_ssid_ = ssid ? ssid : "";
    if (selected_wifi_ssid_.empty()) {
        return;
    }

    if (wifi_select_btn_ != nullptr) {
        lv_obj_add_flag(wifi_select_btn_, LV_OBJ_FLAG_HIDDEN);
    }
    if (wifi_password_label_ != nullptr) {
        lv_obj_remove_flag(wifi_password_label_, LV_OBJ_FLAG_HIDDEN);
    }
    if (password_textarea != nullptr) {
        lv_obj_remove_flag(password_textarea, LV_OBJ_FLAG_HIDDEN);
        lv_obj_scroll_to_view(password_textarea, LV_ANIM_ON);
    }
    if (wifi_connect_btn_ != nullptr) {
        lv_obj_remove_flag(wifi_connect_btn_, LV_OBJ_FLAG_HIDDEN);
    }
}

void LvglTouchUi::UpdateWifiScanResults(const std::vector<std::string>& ssids, esp_err_t scan_ret) {
    if (active_page_ != PageType::kPageWifiSetup || wifi_roller == nullptr) {
        return;
    }

    if (scan_ret != ESP_OK || ssids.empty()) {
        ESP_LOGW(TAG, "WiFi scan returned no APs: %s", esp_err_to_name(scan_ret));
        wifi_scan_has_results_ = false;
        lv_roller_set_options(wifi_roller, "Không tìm thấy mạng", LV_ROLLER_MODE_NORMAL);
        if (wifi_select_btn_ != nullptr) {
            lv_obj_add_state(wifi_select_btn_, LV_STATE_DISABLED);
        }
        return;
    }

    std::string options;
    for (const auto& ssid : ssids) {
        options += ssid + "\n";
    }
    if (!options.empty()) {
        options.pop_back();
    }

    wifi_scan_has_results_ = true;
    lv_roller_set_options(wifi_roller, options.c_str(), LV_ROLLER_MODE_NORMAL);
    if (wifi_select_btn_ != nullptr) {
        lv_obj_clear_state(wifi_select_btn_, LV_STATE_DISABLED);
    }
}
