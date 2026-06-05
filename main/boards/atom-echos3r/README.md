# AtomEchoS3R
## Giới thiệu

AtomEchoS3R là bộ điều khiển lập trình IoT do M5Stack phát hành, dựa trên ESP32-S3-PICO-1-N8R8. Thiết bị dùng giải pháp tích hợp gồm bộ giải mã âm thanh đơn kênh ES8311, micro MEMS và mạch khuếch đại công suất NS4150B.

Bo mạch **không có màn hình, cũng không có thêm nút bấm**, nên cần đánh thức bằng giọng nói. Khi cần, hãy dùng `idf.py monitor` để xem log và xác định trạng thái chạy.

## Cấu hình và lệnh biên dịch

**Thiết lập mục tiêu biên dịch là ESP32S3**

```bash
idf.py set-target esp32s3
```

**Mở menuconfig và cấu hình**

```bash
idf.py menuconfig
```

Thiết lập các tùy chọn sau:

- `Xiaozhi Assistant` → `Board Type` → chọn `AtomEchoS3R`
- `Partition Table` → `Custom partition CSV file` → xóa nội dung cũ, nhập `partitions/v2/8m.csv`
- `Serial flasher config` → `Flash size` → chọn `8 MB`
- `Component config` → `ESP PSRAM` → `Support for external, SPI-connected RAM` → `SPI RAM config` → chọn `Octal Mode PSRAM`

Nhấn `S` để lưu, nhấn `Q` để thoát.

**Biên dịch**

```bash
idf.py build
```

**Nạp firmware**

Kết nối AtomEchoS3R với máy tính, giữ nút RESET ở cạnh bên cho đến khi đèn xanh bên dưới nút RESET nhấp nháy.

```bash
idf.py flash
```

Sau khi nạp xong, nhấn nút RESET một lần để khởi động lại thiết bị.
