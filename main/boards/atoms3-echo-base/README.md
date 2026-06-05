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
Xiaozhi Assistant -> Board Type -> AtomS3 + Echo Base
```

**Tắt đánh thức bằng giọng nói:**

```
Xiaozhi Assistant -> [ ] Enable voice wake-up and audio processing -> Unselect
```

**Chỉnh kích thước flash:**

```text
Serial flasher config -> Flash size -> 8 MB
```

**Chỉnh bảng phân vùng:**

```text
Partition Table -> Custom partition CSV file -> partitions/v2/8m.csv
```

**Tắt PSRAM ngoài chip:**

```text
Component config -> ESP PSRAM -> [ ] Support for external, SPI-connected RAM -> Unselect
```

**Biên dịch:**

```bash
idf.py build
```
