# Liên kết sản phẩm

[Waveshare ESP32-C6-Touch-LCD-1.69](https://www.waveshare.net/shop/ESP32-C6-Touch-LCD-1.69.htm)
[Waveshare ESP32-C6-LCD-1.69](https://www.waveshare.net/shop/ESP32-C6-LCD-1.69.htm)

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
Xiaozhi Assistant -> Board Type -> Waveshare ESP32-C6-LCD-1.69
```

**Build**

```bash
idf.py build
```

**Nạp và mở terminal serial**

```bash
idf.py build flash monitor
```

# Thao tác nút
## Nút BOOT
**Nhấn một lần khi chưa kết nối server: vào chế độ cấu hình mạng**
**Nhấn một lần sau khi đã kết nối server: đánh thức, ngắt lời**

## Nút PWR
**Nhấn đúp: tắt/mở màn hình**
**Nhấn giữ: bật/tắt nguồn**
