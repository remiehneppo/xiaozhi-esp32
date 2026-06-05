# Liên kết sản phẩm

[Waveshare ESP32-S3-ePaper-1.54](https://www.waveshare.net/shop/ESP32-S3-ePaper-1.54.htm)

# Lệnh cấu hình build

**Clone dự án**

```bash
git clone https://github.com/78/xiaozhi-esp32.git
```

**Vào thư mục dự án**

```bash
cd xiaozhi-esp32
```

**Chọn target ESP32S3**

```bash
idf.py set-target esp32s3
```

**Mở menuconfig**

```bash
idf.py menuconfig
```

**Chọn board**

```bash
Xiaozhi Assistant -> Board Type -> Waveshare ESP32-S3-ePaper-1.54
```

**Build**

```bash
idf.py build
```

**Tải và mở terminal serial**

```bash
idf.py build flash monitor
```
