# Lệnh cấu hình biên dịch

**Thiết lập mục tiêu biên dịch là ESP32S3:**

```bash
idf.py set-target esp32s3
```

**Mở menuconfig:**

```bash
idf.py menuconfig
```

**Chọn bo mạch:**

```text
Xiaozhi Assistant -> Board Type -> M5Stack AtomS3R + Echo Pyramid
```

**Chỉnh kích thước flash:**

```text
Serial flasher config -> Flash size -> 8 MB
```

**Chỉnh bảng phân vùng:**

```text
Partition Table -> Custom partition CSV file -> partitions/v2/8m.csv
```

**Chỉnh cấu hình PSRAM:**

```text
Component config -> ESP PSRAM -> SPI RAM config -> Mode (QUAD/OCT) -> Octal Mode PSRAM
```

**Biên dịch:**

```bash
idf.py build
```

## Hướng dẫn sử dụng

Khi Echo Pyramid hoạt động bình thường, hãy cấp nguồn qua cổng USB-C của đế Pyramid; cổng USB-C của AtomS3R chủ yếu dùng để nạp firmware.

# Tài liệu tham khảo

https://github.com/m5stack/M5Echo-Pyramid
