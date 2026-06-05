# DFRobot ESP32-S3 AI Camera Module

## Giới thiệu
ESP32-S3 AI CAM là một module camera thông minh dựa trên chip ESP32-S3, được thiết kế cho xử lý hình ảnh video và tương tác giọng nói, phù hợp cho giám sát video, nhận dạng ảnh ở biên và các dự án đối thoại AI.
![](https://ws.dfrobot.com.cn/FsTrGbrX2NZAwzWS8OSQGOGikuYA)

[Xem giới thiệu chi tiết](https://wiki.dfrobot.com.cn/SKU_DFR1154_ESP32_S3_AI_CAM)

[Xem demo chức năng thị giác](https://www.bilibili.com/video/BV1ktjSzNEUU/)

# Tính năng
* Dùng micro PDM
* Có sẵn camera OV3660 trên board

## Cấu hình phím
* BOOT: nhấn ngắn để ngắt / đánh thức

## Lệnh build

**Chọn target ESP32S3:**

```bash
idf.py set-target esp32s3
```

**Mở menuconfig:**

```bash
idf.py menuconfig
```

**Chọn board:**

```
Xiaozhi Assistant -> Board Type -> DFRobot ESP32-S3 AI Camera Module
```

**Điều chỉnh PSRAM:**

```
Component config -> ESP PSRAM -> SPI RAM config -> Mode (QUAD/OCT) -> Octal Mode PSRAM
```

**Đặt công suất phát Wi-Fi là 10:**

```
Component config -> PHY -> (10)Max WiFi TX power (dBm)
```

**Cấu hình camera:**

* **OV3660**
```
Component config -> Espressif Camera Sensors Configurations -> Camera Sensor Configuration -> Select and Set Camera Sensor -> OV3660 -> Auto detect OV3660
```

```
Component config -> Espressif Camera Sensors Configurations -> Camera Sensor Configuration -> Select and Set Camera Sensor -> OV3660 -> Select default output format for DVP interface (YUV422 240x240 24fps, DVP 8-bit, 20M input)
```

* **OV2640**
```
Component config -> Espressif Camera Sensors Configurations -> Camera Sensor Configuration -> Select and Set Camera Sensor -> OV2640 -> Auto detect OV2640
```

```
Component config -> Espressif Camera Sensors Configurations -> Camera Sensor Configuration -> Select and Set Camera Sensor -> OV2640 -> Select default output format for DVP interface (YUV422 240x240 25fps, DVP 8-bit, 20M input)
```

**Build:**

```bash
idf.py build
```
