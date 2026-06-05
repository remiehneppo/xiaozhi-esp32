# AtomS3R CAM/M12 + Echo Base

## Giới thiệu

<div align="center">
    <a href="https://docs.m5stack.com/zh_CN/core/AtomS3R%20Cam"><b> Trang chủ sản phẩm AtomS3R CAM </b></a>
    |
    <a href="https://docs.m5stack.com/zh_CN/core/AtomS3R-M12"><b> Trang chủ sản phẩm AtomS3R M12 </b></a>
    |
    <a href="https://docs.m5stack.com/zh_CN/atom/Atomic%20Echo%20Base"><b> Trang chủ sản phẩm Echo Base </b></a>
</div>

AtomS3R CAM và AtomS3R M12 là bộ điều khiển lập trình IoT do M5Stack phát hành, dựa trên ESP32-S3-PICO-1-N8R8 và được trang bị camera. Atomic Echo Base là một đế nhận dạng giọng nói được thiết kế đặc biệt cho dòng máy chủ M5 Atom, sử dụng giải pháp tích hợp bộ giải mã âm thanh đơn kênh ES8311, micrô MEMS và bộ khuếch đại công suất NS4150B.

Cả hai phiên bản phát triển này đều **không có màn hình, không có nút bấm bổ sung**, và cần sử dụng tính năng gọi bằng giọng nói. Khi cần thiết, bạn cần sử dụng `idf.py monitor` để xem log và xác định trạng thái hoạt động.

## Lệnh cấu hình và biên dịch

**Cấu hình mục tiêu biên dịch là ESP32S3**

```bash
idf.py set-target esp32s3
