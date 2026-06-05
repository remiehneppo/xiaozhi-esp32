# Hướng dẫn board tùy chỉnh

Tài liệu này mô tả cách tùy biến một bộ khởi tạo board mới cho dự án chatbot giọng nói XiaoZhi AI. XiaoZhi AI hỗ trợ hơn 70 loại board ESP32, và mã khởi tạo của từng board nằm trong thư mục tương ứng.

## Lưu ý quan trọng

> **Cảnh báo**: Với board tùy chỉnh, nếu cấu hình IO khác với board gốc thì tuyệt đối không được dùng cấu hình của board gốc để build và ghi đè firmware. Phải tạo một loại board mới, hoặc tách biệt bằng các `name` và macro `sdkconfig` khác nhau trong phần `builds` của file `config.json`. Dùng `python scripts/release.py [tên thư mục board]` để build và đóng gói firmware.
>
> Nếu ghi đè trực tiếp cấu hình gốc, khi OTA nâng cấp sau này firmware tùy biến của bạn có thể bị firmware chuẩn của board gốc thay thế, khiến thiết bị không hoạt động bình thường. Mỗi board có mã định danh riêng và kênh nâng cấp firmware riêng, nên việc giữ ID của board là rất quan trọng.

## Cấu trúc thư mục

Mỗi board thường có các file sau:

- `xxx_board.cc` - mã khởi tạo board chính, chứa toàn bộ logic khởi tạo và chức năng của board
- `config.h` - file cấu hình board, định nghĩa ánh xạ chân và các cấu hình phần cứng khác
- `config.json` - cấu hình build, chỉ định chip đích và các tùy chọn build đặc biệt
- `README.md` - tài liệu mô tả dành riêng cho board đó

## Các bước tùy biến board

### 1. Tạo thư mục board mới

Trước hết hãy tạo một thư mục mới dưới `boards/`, đặt tên theo dạng `[tên thương hiệu]-[loại board]`, ví dụ `m5stack-tab5`:

```bash
mkdir main/boards/my-custom-board
```

### 2. Tạo file cấu hình

#### config.h

Trong `config.h`, định nghĩa toàn bộ cấu hình phần cứng, gồm:

- Tần số lấy mẫu âm thanh và cấu hình chân I2S
- Địa chỉ chip codec âm thanh và cấu hình chân I2C
- Cấu hình chân nút bấm và LED
- Thông số màn hình và cấu hình chân

Ví dụ tham khảo (lấy từ `lichuang-c3-dev`):

```c
#ifndef _BOARD_CONFIG_H_
#define _BOARD_CONFIG_H_

#include <driver/gpio.h>

// Cấu hình âm thanh
#define AUDIO_INPUT_SAMPLE_RATE  24000
#define AUDIO_OUTPUT_SAMPLE_RATE 24000

#define AUDIO_I2S_GPIO_MCLK GPIO_NUM_10
#define AUDIO_I2S_GPIO_WS   GPIO_NUM_12
#define AUDIO_I2S_GPIO_BCLK GPIO_NUM_8
#define AUDIO_I2S_GPIO_DIN  GPIO_NUM_7
#define AUDIO_I2S_GPIO_DOUT GPIO_NUM_11

#define AUDIO_CODEC_PA_PIN       GPIO_NUM_13
#define AUDIO_CODEC_I2C_SDA_PIN  GPIO_NUM_0
#define AUDIO_CODEC_I2C_SCL_PIN  GPIO_NUM_1
#define AUDIO_CODEC_ES8311_ADDR  ES8311_CODEC_DEFAULT_ADDR

// Cấu hình nút bấm
#define BOOT_BUTTON_GPIO        GPIO_NUM_9

// Cấu hình màn hình
#define DISPLAY_SPI_SCK_PIN     GPIO_NUM_3
#define DISPLAY_SPI_MOSI_PIN    GPIO_NUM_5
#define DISPLAY_DC_PIN          GPIO_NUM_6
#define DISPLAY_SPI_CS_PIN      GPIO_NUM_4

#define DISPLAY_WIDTH   320
#define DISPLAY_HEIGHT  240
#define DISPLAY_MIRROR_X true
#define DISPLAY_MIRROR_Y false
#define DISPLAY_SWAP_XY true

#define DISPLAY_OFFSET_X  0
#define DISPLAY_OFFSET_Y  0

#define DISPLAY_BACKLIGHT_PIN GPIO_NUM_2
#define DISPLAY_BACKLIGHT_OUTPUT_INVERT true

#endif // _BOARD_CONFIG_H_
```

#### config.json

Trong `config.json`, định nghĩa cấu hình build. File này được script `scripts/release.py` dùng để tự động build:

```json
{
    "target": "esp32s3",  // Loại chip đích: esp32, esp32s3, esp32c3, esp32c6, esp32p4, v.v.
    "builds": [
        {
            "name": "my-custom-board",  // Tên board, dùng để tạo gói firmware
            "sdkconfig_append": [
                // Cấu hình kích thước Flash đặc biệt
                "CONFIG_ESPTOOLPY_FLASHSIZE_8MB=y",
                // Cấu hình bảng phân vùng đặc biệt
                "CONFIG_PARTITION_TABLE_CUSTOM_FILENAME=\"partitions/v2/8m.csv\""
            ]
        }
    ]
}
```

**Giải thích cấu hình:**
- `target`: loại chip đích, phải khớp với phần cứng
- `name`: tên gói firmware đầu ra, nên trùng với tên thư mục
- `sdkconfig_append`: mảng các tùy chọn `sdkconfig` bổ sung, sẽ được nối vào cấu hình mặc định

**Các cấu hình `sdkconfig_append` hay dùng:**
```json
// Kích thước Flash
"CONFIG_ESPTOOLPY_FLASHSIZE_4MB=y"   // Flash 4MB
"CONFIG_ESPTOOLPY_FLASHSIZE_8MB=y"   // Flash 8MB
"CONFIG_ESPTOOLPY_FLASHSIZE_16MB=y"  // Flash 16MB

// Bảng phân vùng
"CONFIG_PARTITION_TABLE_CUSTOM_FILENAME=\"partitions/v2/4m.csv\""  // Bảng phân vùng 4MB
"CONFIG_PARTITION_TABLE_CUSTOM_FILENAME=\"partitions/v2/8m.csv\""  // Bảng phân vùng 8MB
"CONFIG_PARTITION_TABLE_CUSTOM_FILENAME=\"partitions/v2/16m.csv\"" // Bảng phân vùng 16MB

// Cấu hình ngôn ngữ
"CONFIG_LANGUAGE_EN_US=y"  // Tiếng Anh
"CONFIG_LANGUAGE_ZH_CN=y"  // Tiếng Trung giản thể

// Cấu hình từ đánh thức
"CONFIG_USE_DEVICE_AEC=y"          // Bật AEC ở thiết bị
"CONFIG_WAKE_WORD_DISABLED=y"      // Tắt từ đánh thức
```

### 3. Viết mã khởi tạo board

Tạo file `my_custom_board.cc` để triển khai toàn bộ logic khởi tạo của board.

Một lớp board cơ bản thường gồm các phần sau:

1. **Khai báo lớp**: kế thừa từ `WifiBoard` hoặc `Ml307Board`
2. **Hàm khởi tạo**: gồm khởi tạo I2C, màn hình, nút bấm, IoT, v.v.
3. **Ghi đè hàm ảo**: như `GetAudioCodec()`, `GetDisplay()`, `GetBacklight()`, v.v.
4. **Đăng ký board**: dùng macro `DECLARE_BOARD`

```cpp
#include "wifi_board.h"
#include "codecs/es8311_audio_codec.h"
#include "display/lcd_display.h"
#include "application.h"
#include "button.h"
#include "config.h"
#include "mcp_server.h"

#include <esp_log.h>
#include <driver/i2c_master.h>
#include <driver/spi_common.h>

#define TAG "MyCustomBoard"

class MyCustomBoard : public WifiBoard {
private:
    i2c_master_bus_handle_t codec_i2c_bus_;
    Button boot_button_;
    LcdDisplay* display_;

    // Khởi tạo I2C
    void InitializeI2c() {
        i2c_master_bus_config_t i2c_bus_cfg = {
            .i2c_port = I2C_NUM_0,
            .sda_io_num = AUDIO_CODEC_I2C_SDA_PIN,
            .scl_io_num = AUDIO_CODEC_I2C_SCL_PIN,
            .clk_source = I2C_CLK_SRC_DEFAULT,
            .glitch_ignore_cnt = 7,
            .intr_priority = 0,
            .trans_queue_depth = 0,
            .flags = {
                .enable_internal_pullup = 1,
            },
        };
        ESP_ERROR_CHECK(i2c_new_master_bus(&i2c_bus_cfg, &codec_i2c_bus_));
    }

    // Khởi tạo SPI (dùng cho màn hình)
    void InitializeSpi() {
        spi_bus_config_t buscfg = {};
        buscfg.mosi_io_num = DISPLAY_SPI_MOSI_PIN;
        buscfg.miso_io_num = GPIO_NUM_NC;
        buscfg.sclk_io_num = DISPLAY_SPI_SCK_PIN;
        buscfg.quadwp_io_num = GPIO_NUM_NC;
        buscfg.quadhd_io_num = GPIO_NUM_NC;
        buscfg.max_transfer_sz = DISPLAY_WIDTH * DISPLAY_HEIGHT * sizeof(uint16_t);
        ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO));
    }

    // Khởi tạo nút bấm
    void InitializeButtons() {
        boot_button_.OnClick([this]() {
            auto& app = Application::GetInstance();
            if (app.GetDeviceState() == kDeviceStateStarting) {
                EnterWifiConfigMode();
                return;
            }
            app.ToggleChatState();
        });
    }

    // Khởi tạo màn hình (ví dụ dùng ST7789)
    void InitializeDisplay() {
        esp_lcd_panel_io_handle_t panel_io = nullptr;
        esp_lcd_panel_handle_t panel = nullptr;
        
        esp_lcd_panel_io_spi_config_t io_config = {};
        io_config.cs_gpio_num = DISPLAY_SPI_CS_PIN;
        io_config.dc_gpio_num = DISPLAY_DC_PIN;
        io_config.spi_mode = 2;
        io_config.pclk_hz = 80 * 1000 * 1000;
        io_config.trans_queue_depth = 10;
        io_config.lcd_cmd_bits = 8;
        io_config.lcd_param_bits = 8;
        ESP_ERROR_CHECK(esp_lcd_new_panel_io_spi(SPI2_HOST, &io_config, &panel_io));

        esp_lcd_panel_dev_config_t panel_config = {};
        panel_config.reset_gpio_num = GPIO_NUM_NC;
        panel_config.rgb_ele_order = LCD_RGB_ELEMENT_ORDER_RGB;
        panel_config.bits_per_pixel = 16;
        ESP_ERROR_CHECK(esp_lcd_new_panel_st7789(panel_io, &panel_config, &panel));
        
        esp_lcd_panel_reset(panel);
        esp_lcd_panel_init(panel);
        esp_lcd_panel_invert_color(panel, true);
        esp_lcd_panel_swap_xy(panel, DISPLAY_SWAP_XY);
        esp_lcd_panel_mirror(panel, DISPLAY_MIRROR_X, DISPLAY_MIRROR_Y);
        
        // Tạo đối tượng màn hình
        display_ = new SpiLcdDisplay(panel_io, panel,
                                    DISPLAY_WIDTH, DISPLAY_HEIGHT, 
                                    DISPLAY_OFFSET_X, DISPLAY_OFFSET_Y, 
                                    DISPLAY_MIRROR_X, DISPLAY_MIRROR_Y, DISPLAY_SWAP_XY);
    }

    // Khởi tạo MCP tools
    void InitializeTools() {
        // Tham khảo tài liệu MCP
    }

public:
    // Hàm tạo
    MyCustomBoard() : boot_button_(BOOT_BUTTON_GPIO) {
        InitializeI2c();
        InitializeSpi();
        InitializeDisplay();
        InitializeButtons();
        InitializeTools();
        GetBacklight()->SetBrightness(100);
    }

    // Lấy codec âm thanh
    virtual AudioCodec* GetAudioCodec() override {
        static Es8311AudioCodec audio_codec(
            codec_i2c_bus_, 
            I2C_NUM_0, 
            AUDIO_INPUT_SAMPLE_RATE, 
            AUDIO_OUTPUT_SAMPLE_RATE,
            AUDIO_I2S_GPIO_MCLK, 
            AUDIO_I2S_GPIO_BCLK, 
            AUDIO_I2S_GPIO_WS, 
            AUDIO_I2S_GPIO_DOUT, 
            AUDIO_I2S_GPIO_DIN,
            AUDIO_CODEC_PA_PIN, 
            AUDIO_CODEC_ES8311_ADDR);
        return &audio_codec;
    }

    // Lấy màn hình
    virtual Display* GetDisplay() override {
        return display_;
    }
    
    // Lấy điều khiển đèn nền
    virtual Backlight* GetBacklight() override {
        static PwmBacklight backlight(DISPLAY_BACKLIGHT_PIN, DISPLAY_BACKLIGHT_OUTPUT_INVERT);
        return &backlight;
    }
};

// Đăng ký board
DECLARE_BOARD(MyCustomBoard);
```

### 4. Thêm cấu hình hệ thống build

#### Thêm lựa chọn board trong `Kconfig.projbuild`

Mở file `main/Kconfig.projbuild`, rồi thêm lựa chọn board mới trong phần `choice BOARD_TYPE`:

```kconfig
choice BOARD_TYPE
    prompt "Board Type"
    default BOARD_TYPE_BREAD_COMPACT_WIFI
    help
        Board type. Loại board
    
    # ... các lựa chọn board khác ...
    
    config BOARD_TYPE_MY_CUSTOM_BOARD
        bool "My Custom Board (Board tùy chỉnh của tôi)"
        depends on IDF_TARGET_ESP32S3  # sửa theo chip đích của bạn
endchoice
```

**Lưu ý:**
- `BOARD_TYPE_MY_CUSTOM_BOARD` là tên cấu hình, cần viết hoa toàn bộ và ngăn cách bằng dấu gạch dưới
- `depends on` chỉ định loại chip đích (như `IDF_TARGET_ESP32S3`, `IDF_TARGET_ESP32C3`, v.v.)
- Phần mô tả có thể viết bằng cả tiếng Anh và tiếng Việt

#### Thêm cấu hình board trong `CMakeLists.txt`

Mở file `main/CMakeLists.txt`, rồi thêm cấu hình mới vào chuỗi điều kiện chọn board:

```cmake
# Thêm cấu hình board của bạn vào chuỗi elseif
elseif(CONFIG_BOARD_TYPE_MY_CUSTOM_BOARD)
    set(BOARD_TYPE "my-custom-board")  # phải khớp với tên thư mục
    set(BUILTIN_TEXT_FONT font_puhui_basic_20_4)  # chọn font phù hợp với kích thước màn hình
    set(BUILTIN_ICON_FONT font_awesome_20_4)
    set(DEFAULT_EMOJI_COLLECTION twemoji_64)  # tùy chọn, nếu cần hiển thị biểu cảm
endif()
```

**Giải thích cấu hình font và emoji:**

Chọn cỡ font phù hợp theo độ phân giải màn hình:
- Màn hình nhỏ (128x64 OLED): `font_puhui_basic_14_1` / `font_awesome_14_1`
- Màn hình nhỏ-vừa (240x240): `font_puhui_basic_16_4` / `font_awesome_16_4`
- Màn hình vừa (240x320): `font_puhui_basic_20_4` / `font_awesome_20_4`
- Màn hình lớn (480x320+): `font_puhui_basic_30_4` / `font_awesome_30_4`

Các tùy chọn bộ emoji:
- `twemoji_32` - emoji 32x32 pixel (màn hình nhỏ)
- `twemoji_64` - emoji 64x64 pixel (màn hình lớn)

### 5. Cấu hình và biên dịch

#### Cách 1: dùng `idf.py` cấu hình thủ công

1. **Đặt target chip** (khi cấu hình lần đầu hoặc đổi chip):
   ```bash
   # Với ESP32-S3
   idf.py set-target esp32s3
   
   # Với ESP32-C3
   idf.py set-target esp32c3
   
   # Với ESP32
   idf.py set-target esp32
   ```

2. **Xóa cấu hình cũ**:
   ```bash
   idf.py fullclean
   ```

3. **Mở menu cấu hình**:
   ```bash
   idf.py menuconfig
   ```
   
   Trong menu, đi tới `Xiaozhi Assistant` -> `Board Type`, rồi chọn board tùy chỉnh của bạn.

4. **Build và nạp**:
   ```bash
   idf.py build
   idf.py flash monitor
   ```

#### Cách 2: dùng script `release.py` (khuyến nghị)

Nếu thư mục board của bạn có file `config.json`, có thể dùng script này để tự động cấu hình và build:

```bash
python scripts/release.py my-custom-board
```

Script này sẽ tự động:
- Đọc cấu hình `target` trong `config.json` và đặt chip đích
- Áp dụng các tùy chọn build trong `sdkconfig_append`
- Hoàn tất build và đóng gói firmware

### 6. Tạo `README.md`

Trong `README.md`, mô tả đặc điểm của board, yêu cầu phần cứng, cách build và cách nạp:

## Các thành phần board thường gặp

### 1. Màn hình

Dự án hỗ trợ nhiều driver màn hình, gồm:
- ST7789 (SPI)
- ILI9341 (SPI)
- SH8601 (QSPI)
- v.v.

### 2. Codec âm thanh

Các codec được hỗ trợ gồm:
- ES8311 (thường dùng)
- ES7210 (mảng micro)
- AW88298 (power amplifier)
- v.v.

### 3. Quản lý nguồn

Một số board dùng chip quản lý nguồn:
- AXP2101
- các PMIC khác có thể dùng được

### 4. Điều khiển thiết bị MCP

Có thể thêm nhiều MCP tool để AI sử dụng:
- Speaker (điều khiển loa)
- Screen (điều chỉnh độ sáng màn hình)
- Battery (đọc mức pin)
- Light (điều khiển đèn)
- v.v.

## Quan hệ kế thừa của lớp board

- `Board` - lớp board cơ sở
  - `WifiBoard` - board kết nối Wi-Fi
  - `Ml307Board` - board dùng module 4G
  - `DualNetworkBoard` - board hỗ trợ chuyển đổi giữa Wi-Fi và 4G

## Kinh nghiệm phát triển

1. **Tham khảo board tương tự**: nếu board mới của bạn giống với board hiện có, hãy xem cách triển khai hiện tại
2. **Debug từng bước**: làm xong tính năng cơ bản trước (như hiển thị), rồi mới thêm tính năng phức tạp hơn (như âm thanh)
3. **Ánh xạ chân**: đảm bảo cấu hình đúng toàn bộ chân trong `config.h`
4. **Kiểm tra tương thích phần cứng**: xác nhận mọi chip và driver đều tương thích

## Vấn đề có thể gặp

1. **Màn hình không hoạt động đúng**: kiểm tra cấu hình SPI, mirror và invert màu
2. **Không có âm thanh đầu ra**: kiểm tra cấu hình I2S, chân bật PA và địa chỉ codec
3. **Không kết nối được mạng**: kiểm tra thông tin Wi-Fi và cấu hình mạng
4. **Không giao tiếp được với server**: kiểm tra cấu hình MQTT hoặc WebSocket

## Tài liệu tham khảo

- Tài liệu ESP-IDF: https://docs.espressif.com/projects/esp-idf/
- Tài liệu LVGL: https://docs.lvgl.io/
- Tài liệu ESP-SR: https://github.com/espressif/esp-sr
