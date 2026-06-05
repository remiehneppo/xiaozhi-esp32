# ESP-Spot

## Giới thiệu

<div align="center">
    <a href="https://oshwhub.com/esp-college/esp-spot"><b> Nền tảng mã nguồn mở Lichuang </b></a>
    |
    <a href="https://www.bilibili.com/video/BV1ekRAYVEZ1/"><b> Bilibili Demo </b></a>
</div>

ESP-Spot là một hộp tương tác giọng nói thông minh mã nguồn mở của ESP Friends, tích hợp micro, loa và cảm biến quán tính IMU, có thể dùng pin để cấp nguồn. ESP-Spot không có màn hình, nhưng có một đèn báo RGB và hai nút bấm. Chi tiết phần cứng xem tại [dự án mã nguồn mở trên Lichuang](https://oshwhub.com/esp-college/esp-spot).

Phiên bản mã nguồn mở của ESP-Spot dùng mô-đun ESP32-S3-WROOM-1-N16R8 hoặc ESP32-C5-WROOM-1-N8R8. Nếu khi làm lại phần cứng bạn dùng dung lượng flash khác, cần chỉnh lại các tham số tương ứng.


## Lệnh cấu hình và biên dịch

**Thiết lập mục tiêu biên dịch**

```bash
idf.py set-target esp32s3 # Spot S3
# or
idf.py set-target esp32c5 # Spot C5
```

**Mở menuconfig và cấu hình**

```bash
idf.py menuconfig
```

Thiết lập tùy chọn sau:

- `Xiaozhi Assistant` → `Board Type` → chọn `ESP-Spot-S3` / `ESP-Spot-C5`

Nhấn `S` để lưu, nhấn `Q` để thoát.

**Biên dịch**

```bash
idf.py build
```

**Nạp firmware**

```bash
idf.py flash
```

> [!TIP]
>
> **Nếu máy tính luôn không tìm thấy cổng nối tiếp của ESP-Spot, hãy thử các bước sau**
> 1. Mở nắp trước;
> 2. Rút bo PCB có gắn mô-đun ra;
> 3. Giữ <kbd>BOOT</kbd> rồi cắm lại bo PCB, chú ý không lắp ngược;
> 
> Lúc này ESP-Spot phải đã vào chế độ tải xuống. Sau khi nạp xong, có thể cần cắm lại bo PCB một lần nữa.

## Nguồn thấp

ESP-Spot hỗ trợ chế độ tiết kiệm điện Deep Sleep.

Sau khi ở trạng thái idle 10 phút, ESP-Spot sẽ tự động vào Deep Sleep; nhấn nút Key hoặc lắc ESP-Spot để đánh thức.
