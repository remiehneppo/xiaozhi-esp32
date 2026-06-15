# Bo mạch ESP-S3-BAO

Bo mạch phát triển ESP32-S3, được kế thừa từ bo mạch Quandong ESP32-S3 (quandong-s3-dev), hỗ trợ hình nền LVGL tùy chỉnh riêng cho từng chế độ sáng và tối.

## Thông số phần cứng

| Thành phần | Thông số |
|---|---|
| Vi xử lý | ESP32-S3 |
| Màn hình | ILI9341 240×320 SPI |
| Âm thanh | ES8311 + PCA9557 |
| Nút bấm | BOOT (GPIO0) |
| Đèn nền | GPIO45 |
| I2C SDA | GPIO16 |
| I2C SCL | GPIO15 |

## Xây dựng

```bash
idf.py set-target esp32s3
idf.py menuconfig → Component config → XiaoZhi → BOARD_TYPE → ESP-S3-BAO 开发板
idf.py build
```

## Hình nền tùy chỉnh

Bo mạch này hỗ trợ hình nền riêng cho **chế độ sáng** và **chế độ tối**, cho phép giao diện tối ưu cho từng điều kiện ánh sáng.

### 1. Chuẩn bị hình ảnh

- Kích thước: **240×320** (dọc)
- Định dạng: **PNG** (khuyến nghị)
- Tạo 2 file:
  - `background_light.png` — hình nền cho chế độ sáng (nền sáng, chữ đậm)
  - `background_dark.png` — hình nền cho chế độ tối (nền tối, chữ sáng)

### 2. Cài đặt công cụ chuyển đổi

```bash
pip3 install pypng lz4
```

### 3. Chuyển đổi hình ảnh

```bash
# Chuyển đổi hình nền chế độ sáng
python3 scripts/Image_Converter/LVGLImage.py \
    -i background_light.png \
    -o esps3_bao_dev_bg_light.bin \
    -f RGB565

# Chuyển đổi hình nền chế độ tối
python3 scripts/Image_Converter/LVGLImage.py \
    -i background_dark.png \
    -o esps3_bao_dev_bg_dark.bin \
    -f RGB565
```

### 4. Đặt file

Sao chép các file vừa tạo vào thư mục:

```
main/assets/common/esps3_bao_dev_bg_light.bin   # Chế độ sáng
main/assets/common/esps3_bao_dev_bg_dark.bin    # Chế độ tối
```

### 5. Xây dựng lại

```bash
idf.py build && idf.py flash
```

### Hoàn nguyên về màu nền mặc định

Xóa cả hai file `esps3_bao_dev_bg_light.bin` và `esps3_bao_dev_bg_dark.bin` để tự động hoàn nguyên về màu nền mặc định (chế độ sáng `#FFFFFF` / chế độ tối `#000000`).

### Thay đổi màu nền mặc định

Chỉnh sửa hàm `InitializeLcdThemes()` trong file `main/display/lcd_display.cc`:

```cpp
// Chế độ sáng
light_theme->set_background_color(lv_color_hex(0xFFFFFF));

// Chế độ tối
dark_theme->set_background_color(lv_color_hex(0x000000));
```
