# Liên kết sản phẩm

[Waveshare ESP32-C6-Touch-AMOLED-1.32](https://www.waveshare.net/shop/ESP32-C6-Touch-AMOLED-1.32.htm)

# Lệnh cấu hình build

**Clone dự án**

```bash
git clone https://github.com/78/xiaozhi-esp32.git
```

**Vào thư mục dự án**

```bash
cd xiaozhi-esp32
```

**Chọn target ESP32C6**

```bash
idf.py set-target esp32c6
```

**Mở menuconfig**

```bash
idf.py menuconfig
```

**Chọn board**

```bash
Xiaozhi Assistant -> Board Type -> Waveshare ESP32-C6-Touch-AMOLED-1.32
```

**Build**

```bash
idf.py build
```

**Nạp và mở terminal serial**

```bash
idf.py build flash monitor
```
