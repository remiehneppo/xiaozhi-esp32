# DFRobot Haikong Board K10

## Cấu hình nút bấm
* A: nhấn ngắn - ngắt/đánh thức, nhấn giữ 1s - tăng âm lượng
* B: nhấn ngắn - ngắt/đánh thức, nhấn giữ 1s - giảm âm lượng

## Lệnh cấu hình biên dịch

**Thiết lập mục tiêu biên dịch là ESP32S3:**

```bash
idf.py set-target esp32s3
```

**Mở menuconfig:**

```bash
idf.py menuconfig
```

**Chọn bo mạch:**

```
Xiaozhi Assistant -> Board Type -> DFRobot Haikong Board K10
```

**Chỉnh cấu hình PSRAM:**

```
Component config -> ESP PSRAM -> SPI RAM config -> Mode (QUAD/OCT) -> Octal Mode PSRAM
```

**Bật hoán đổi endian cho bộ đệm camera:**

```
Xiaozhi Assistant -> Camera Configuration -> Enable software camera buffer endianness swapping
```

**Cấu hình camera:**
```
Component config -> Espressif Camera Sensors Configurations -> Camera Sensor Configuration -> Select and Set Camera Sensor -> GC2145 ->  Auto detect GC2145

```

```
Component config -> Espressif Camera Sensors Configurations -> Camera Sensor Configuration -> Select and Set Camera Sensor -> GC2145 ->  Select default output format for DVP interface (RGB565 800x600 20fps, DVP 8-bit, 20M input) -> RGB565 800x600 20fps, DVP 8-bit, 20M input

```

**Biên dịch:**

```bash
idf.py build
```



