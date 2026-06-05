# Xingzhi ABS 2.0

## Giới thiệu
Xingzhi ABS 2.0 là một bo mạch phát triển tương tác giọng nói AI có chi phí hợp lý. Thiết bị trang bị màn hình LCD 1.54 inch, có các nút vật lý riêng và dùng **mô-đun liên lạc ML307R 4G**, giúp bạn trò chuyện với mô hình lớn mọi lúc mọi nơi ngay cả khi không có Wi-Fi.

## Tính năng chính
- Kết nối hai mạng: hỗ trợ chuyển đổi giữa Wi-Fi và ML307R Cat.1 4G, phù hợp cho nhiều kịch bản sử dụng
- Hệ thống hiển thị: màn hình LCD 1.54 inch 240×240, bố cục UI tùy biến để tối ưu cho màn hình vuông 1.54 inch
- Tương tác bằng nút vật lý: phím Boot riêng, phím tăng/giảm âm lượng, hỗ trợ nhấn một lần, nhấn đúp, nhấn giữ và nhấn năm lần
- Khả năng mở rộng: có khe cắm Micro SD tích hợp, hỗ trợ mở rộng lưu trữ cục bộ; có chỗ chừa cho motor rung để phản hồi xúc giác khi thao tác nút
- Quản lý nguồn hoàn chỉnh: hỗ trợ đo ADC mức pin, giám sát trạng thái sạc theo thời gian thực, ngủ tự động và chế độ deep sleep tiết kiệm điện
- Tương thích hệ sinh thái: tương thích đầy đủ với firmware dự án Xiaozhi ESP32, hỗ trợ các mô hình lớn như Qwen/DeepSeek và tương thích điều khiển thiết bị qua giao thức MCP
- Do khác biệt phần cứng, vị trí hiển thị của biểu tượng cảm xúc và văn bản ở phần dưới có thay đổi nhẹ

## Khác biệt cốt lõi so với bản nhôm (XINGZHI_METAL_1_54_WIFI)
| Tính năng | xingzhi-abs-2.0 | Bản nhôm |
|----------|------------------|------------|
| Cách tương tác | Phím vật lý riêng (Boot/Tăng âm lượng/Giảm âm lượng) | Chip cảm ứng CST816 + tương tác cảm ứng |
| Vật liệu vỏ | Nhựa kỹ thuật ABS | Hợp kim nhôm |

>### Thao tác nút
>- **Bật máy**: Ở trạng thái tắt nguồn, nhấn giữ nút nguồn 3 giây để tự động bật máy
>- **Tắt máy**: Ở trạng thái bật máy, nhấn giữ nút nguồn 5 giây để tự động tắt máy
>- **Đánh thức/ngắt**: Trong trạng thái đàm thoại hoặc chờ bình thường, nhấn một lần phím Boot để đánh thức thiết bị hoặc ngắt cuộc hội thoại đang diễn ra
>- **Kết nối lại Wi-Fi**: Trong 1 giây sau khi bật máy, nhấn một lần phím Boot, thiết bị sẽ tự khởi động lại và vào giao diện cấu hình mạng
>- **Chuyển chế độ mạng**: Khi đang bật máy, nhấn đúp phím Boot để chuyển giữa chế độ Wi-Fi và 4G
>- **Kiểm tra trạng thái SD**: Khi đang bật máy, nhấn năm lần phím Boot để kiểm tra và hiển thị trạng thái gắn thẻ SD trên màn hình
>- **Tăng âm lượng**: Khi đang bật máy, nhấn một lần phím tăng âm lượng để tăng 10%; nhấn giữ 2 giây để tăng thẳng lên mức tối đa 100%
>- **Giảm âm lượng**: Khi đang bật máy, nhấn một lần phím giảm âm lượng để giảm 10%; nhấn giữ 2 giây để giảm thẳng xuống 0% (tắt tiếng)

>### Thao tác ngủ
>- **Ngủ nông**: Sau khi bật máy và giữ trạng thái chờ 60 giây, thiết bị chuyển sang ngủ nông (độ sáng màn hình giảm xuống 1%)
>- **Ngủ sâu**: Sau khi bật máy và giữ trạng thái chờ 300 giây, thiết bị sẽ tự động tắt nguồn
>- **Đánh thức**: Ở trạng thái ngủ nông, nhấn bất kỳ phím nào để đánh thức thiết bị (độ sáng màn hình trở lại)

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
- `Xiaozhi Assistant` → `Board Type` → chọn `Xingzhi ABS 2.0`
```

**Biên dịch**

```bash
idf.py build
```

**Nạp và mở terminal serial**

```bash
idf.py build flash monitor
```
