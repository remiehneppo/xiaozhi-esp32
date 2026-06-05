# ESP-BOX-3

## Giới thiệu

<div align="center">
    <a href="https://github.com/espressif/esp-box"><b > ESP-BOX GitHub </b></a>
</div>

ESP-BOX-3 là bộ phát triển AIoT do Espressif phát triển chính thức, được trang bị module ESP32-S3-WROOM-1, màn hình IPS LCD 2.4 inch 320x240, mảng micro kép, hỗ trợ gọi thoại ngoại tuyến và chức năng khử tiếng vang tại thiết bị (AEC).

## Đặc tính phần cứng

- **Bộ điều khiển chính**: ESP32-S3-WROOM-1 (16MB Flash, 8MB PSRAM)
- **Màn hình**: IPS LCD 2.4 inch (320x240, ILI9341)
- **Âm thanh**: Codec âm thanh ES8311 + ADC micro kép ES7210
- **Chức năng âm thanh**: Hỗ trợ AEC tại thiết bị (khử tiếng vang)
- **Nút bấm**: Nút Boot (chức năng nhấn đơn/nhấn kép)
- **Khác**: Cấp nguồn và giao tiếp USB-C

## Lệnh cấu hình và biên dịch

**Cấu hình mục tiêu biên dịch là ESP32S3**

```bash
idf.py set-target esp32s3
