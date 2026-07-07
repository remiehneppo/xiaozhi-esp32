#ifndef TOUCH_UI_H
#define TOUCH_UI_H

#include "display.h"
#include "device_state.h"
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>
#include <string>

class TouchUi : public Display {
protected:
    Display* display_;
    TaskHandle_t lock_owner_ = nullptr;
    int lock_depth_ = 0;
    portMUX_TYPE lock_mux_ = portMUX_INITIALIZER_UNLOCKED;

public:
    TouchUi(Display* display) : display_(display) {
        if (display_) {
            width_ = display_->width();
            height_ = display_->height();
            current_theme_ = display_->GetTheme();
        }
    }
    virtual ~TouchUi() = default;

    virtual void Initialize(const char* startup_message = nullptr) override = 0;

    // Notify UI of device state transitions
    virtual void HandleDeviceStateChange(DeviceState old_state, DeviceState new_state) override = 0;

    // Delegate locking to the hardware display driver
    virtual bool Lock(int timeout_ms = 0) override {
        auto current_task = xTaskGetCurrentTaskHandle();
        portENTER_CRITICAL(&lock_mux_);
        if (lock_owner_ == current_task) {
            lock_depth_++;
            portEXIT_CRITICAL(&lock_mux_);
            return true;
        }
        portEXIT_CRITICAL(&lock_mux_);

        if (display_ && !display_->Lock(timeout_ms)) {
            return false;
        }

        portENTER_CRITICAL(&lock_mux_);
        lock_owner_ = current_task;
        lock_depth_ = 1;
        portEXIT_CRITICAL(&lock_mux_);
        return true;
    }
    virtual void Unlock() override {
        auto current_task = xTaskGetCurrentTaskHandle();
        bool should_unlock_display = false;

        portENTER_CRITICAL(&lock_mux_);
        if (lock_owner_ != current_task || lock_depth_ == 0) {
            portEXIT_CRITICAL(&lock_mux_);
            return;
        }

        lock_depth_--;
        if (lock_depth_ == 0) {
            lock_owner_ = nullptr;
            should_unlock_display = true;
        }
        portEXIT_CRITICAL(&lock_mux_);

        if (should_unlock_display && display_) {
            display_->Unlock();
        }
    }
};

#endif // TOUCH_UI_H
