# Xingzhi 1.54 METAL (wifi)

## Giới thiệu
Xingzhi 1.54 METAL (wifi) là bản nâng cấp của mẫu mở khuôn Xingzhi 1.54, trang bị màn hình LCD 1.54 inch và chip cảm ứng CST816. Thiết bị dùng tương tác cảm ứng thay cho nút bấm vật lý và nâng cấp vỏ sang chất liệu hợp kim nhôm, đồng thời tối ưu trải nghiệm tương tác cũng như cảm giác cầm nắm và chất lượng hoàn thiện.

>### Thao tác nút
>- **Bật máy**: Ở trạng thái tắt nguồn, nhấn giữ nút nguồn 3 giây để tự động bật máy (phần cứng đời cũ chỉ cần nhấn giữ nút nguồn 1 giây)
>- **Tắt máy**: Ở trạng thái bật máy, nhấn giữ nút nguồn 5 giây để tự động tắt máy (phần cứng đời cũ sẽ không tự tắt khi cắm USB)
>- **Đánh thức/ngắt**: Trong môi trường đàm thoại bình thường, nhấn một lần vào nút cảm ứng ở giữa
>- **Kết nối lại Wi-Fi**: Sau khi bật máy, nhấn một lần vào nút cảm ứng ở giữa trong vòng 1 giây, thiết bị sẽ tự khởi động lại và vào giao diện cấu hình mạng
>- **Tăng âm lượng**: Khi đang bật máy, nhấn một lần vào nút cảm ứng bên phải để tăng âm lượng. Nhấn giữ nút cảm ứng bên phải 2 giây để tăng dần âm lượng.
>- **Giảm âm lượng**: Khi đang bật máy, nhấn một lần vào nút cảm ứng bên trái để giảm âm lượng. Nhấn giữ nút cảm ứng bên trái 2 giây để giảm dần âm lượng.

>### Thao tác ngủ
>- **Ngủ nông**: Sau khi bật máy và giữ trạng thái chờ 60 giây, thiết bị chuyển sang ngủ nông (độ sáng màn hình giảm xuống 1%)
>- **Ngủ sâu**: Sau khi bật máy và giữ trạng thái chờ 300 giây, thiết bị chuyển sang ngủ sâu (tự động tắt nguồn)
>- **Đánh thức**: Ở trạng thái ngủ nông, nhấn một lần vào nút cảm ứng ở giữa để đánh thức thiết bị (độ sáng màn hình trở lại)

# Lệnh cấu hình biên dịch

**Clone dự án**

```bash
git clone https://github.com/78/xiaozhi-esp32.git
```

**Vào thư mục dự án**

```bash
cd xiaozhi-esp32
```

**Thiết lập mục tiêu biên dịch là ESP32S3**

```bash
idf.py set-target esp32s3
```

**Mở menuconfig**

```bash
idf.py menuconfig
```

**Chọn bo mạch**

```bash
- `Xiaozhi Assistant` → `Board Type` → chọn `Xingzhi 1.54 METAL(wifi)`
```

**Biên dịch**

```bash
idf.py build
```

**Nạp và mở terminal serial**

```bash
idf.py build flash monitor
```
