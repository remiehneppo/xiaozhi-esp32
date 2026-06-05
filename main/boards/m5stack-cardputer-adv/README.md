# M5Stack Cardputer Adv

M5Stack Cardputer Adv là một máy tính dạng thẻ dựa trên ESP32-S3FN8 (Stamp-S3A).

## Thông số phần cứng

| Thành phần | Thông số |
|------|------|
| MCU | ESP32-S3FN8 @ 240MHz |
| Flash | 8MB |
| Màn hình | ST7789V2 1.14" 240x135 |
| Mã hóa/giải mã âm thanh | ES8311 |
| Khuếch đại công suất | NS4150B |
| Micro | MEMS |
| Bàn phím | 56 phím (TCA8418) |
| IMU | BMI270 |
| Pin | 1750mAh |

## Định nghĩa chân

### Màn hình (ST7789V2)
| Chức năng | GPIO |
|------|------|
| MOSI | GPIO35 |
| SCLK | GPIO36 |
| CS | GPIO37 |
| DC | GPIO34 |
| RST | GPIO33 |
| BL | GPIO38 |

### Âm thanh (ES8311)
| Chức năng | GPIO |
|------|------|
| I2C SDA | GPIO8 |
| I2C SCL | GPIO9 |
| I2S BCLK | GPIO41 |
| I2S LRCK | GPIO43 |
| I2S DOUT | GPIO46 |
| I2S DIN | GPIO42 |

## Cách sử dụng

1. Nhấn nút BOOT để vào chế độ cấu hình mạng
2. Sau khi kết nối Wi-Fi, bạn có thể dùng chức năng trợ lý giọng nói

## Tham số nạp

Chip: ESP32-S3, Flash: 8MB, chế độ: DIO, tần số: 80MHz

| Địa chỉ | Tệp |
|------|------|
| 0x0 | bootloader/bootloader.bin |
| 0x8000 | partition_table/partition-table.bin |
| 0xd000 | ota_data_initial.bin |
| 0x20000 | xiaozhi.bin |
| 0x600000 | generated_assets.bin |

Lệnh nạp (thư mục build là `build-cardputer-adv`):

```bash
python -m esptool --chip esp32s3 -b 460800 -p PORT \
  --before default_reset --after hard_reset \
  write_flash --flash_mode dio --flash_size 8MB --flash_freq 80m \
  0x0 build-cardputer-adv/bootloader/bootloader.bin \
  0x8000 build-cardputer-adv/partition_table/partition-table.bin \
  0xd000 build-cardputer-adv/ota_data_initial.bin \
  0x20000 build-cardputer-adv/xiaozhi.bin \
  0x600000 build-cardputer-adv/generated_assets.bin
```

Thay `PORT` bằng đường dẫn thiết bị serial thực tế (ví dụ `/dev/cu.usbmodem21101`).

## Liên kết tham khảo

- [Tài liệu chính thức M5Stack Cardputer Adv](https://docs.m5stack.com/en/core/Cardputer-Adv)
