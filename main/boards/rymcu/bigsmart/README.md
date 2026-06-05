# RYMCU BigSmart

Thư mục này là bản thích ứng cho bo mạch phát triển `RYMCU BigSmart`, và được ánh xạ theo các tài nguyên phần cứng sau:

- Bộ điều khiển chính: ESP32-S3-WROOM-1-N16R8
- Màn hình: ST7789 (320x240, SPI)
- Cảm ứng: GT911 (I2C)
- Âm thanh: ES8311 + ES7210 (I2S + I2C)
- Mở rộng IO: PCA9557 (địa chỉ I2C `0x19`)
- Camera: GC0308 (DVP)

Tham khảo tài liệu phần cứng:

- https://github.com/rymcu/BigSmart-Open/blob/main/docs/rymcu-bigsmart-hardware.md

## Biên dịch

```bash
idf.py set-target esp32s3
idf.py menuconfig
