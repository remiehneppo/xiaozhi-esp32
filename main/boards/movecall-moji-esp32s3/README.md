# Lệnh cấu hình biên dịch

**Đặt mục tiêu biên dịch là ESP32S3:**

```bash
idf.py set-target esp32s3
```

**Mở menuconfig:**

```bash
idf.py menuconfig
```

**Chọn bo mạch:**

```
Xiaozhi Assistant -> Board Type -> Movecall Moji, bản phái sinh của XiaoZhi AI
```


**Biên dịch:**

```bash
idf.py build
```
