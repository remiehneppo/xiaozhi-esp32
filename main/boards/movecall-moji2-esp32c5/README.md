# Hướng dẫn biên dịch và cấu hình

Tài liệu này hướng dẫn cách cấu hình và biên dịch firmware cho **Movecall Moji2.0 (phiên bản Xiaozhi AI)**.

## 🛠 Điều kiện cần có
*   **ESP-IDF Version**: v5.5
*   **Target Chip**: ESP32-C5

## 🔗 Thông tin phần cứng
Dự án này dựa trên phần cứng mã nguồn mở sau:
*   **Liên kết OSHWHub**: [https://oshwhub.com/movecall/moji2](https://oshwhub.com/movecall/moji2)

---

## 🚀 Các bước biên dịch

### 1. Đặt mục tiêu biên dịch
Khởi tạo dự án để nhắm tới chip ESP32-C5:
```bash
idf.py set-target esp32c5
```

### 2. Cấu hình loại bo mạch
Mở menu cấu hình đồ họa:
```bash
idf.py menuconfig
```

**Đi tới đường dẫn sau để chọn bo mạch của bạn:**
> **Xiaozhi Assistant** -> **Board Type** -> **Movecall Moji2.0 bản phái sinh Xiaozhi AI**

*Lưu ý: Sau khi chọn, nhấn **S** để lưu (rồi Enter để xác nhận) và nhấn **Q** để thoát.*

### 3. Biên dịch dự án
Chạy lệnh sau để bắt đầu biên dịch:
```bash
idf.py build
```

---

## 🔧 Lệnh hữu ích

**Xóa file biên dịch cũ (khuyến nghị nếu gặp lỗi):**
```bash
idf.py fullclean
```

**Nạp firmware vào thiết bị:**
```bash
idf.py flash
```

**Theo dõi đầu ra serial:**
```bash
idf.py monitor
```
