# jiuchuan-xiaozhi-sound
Loa AI XiaoZhi của Jiuchuan Technology

## 🛠️ Hướng dẫn biên dịch
**Môi trường phát triển**: ESP-IDF v5.4.1

### Các bước biên dịch:
> ⚠️ **Gợi ý**: Nếu việc truy cập thư viện trực tuyến thất bại trong quá trình biên dịch, bạn có thể thử chuyển đổi trạng thái tăng tốc, hoặc sửa đổi tệp [idf_component.yml] để thay thế bằng nguồn gương trong nước.

1. Mở thư mục dự án bằng VSCode;
2. Xóa dự án (Clean Project);
3. Đặt phiên bản ESP-IDF là `v5.4.1`;
4. Nhấp vào gợi ý ở góc dưới bên phải của VSCode để tạo tệp [compile_commands.json];
5. Đặt thiết bị mục tiêu là `[esp32s3] -> [JTAG]`;
6. Mở **SDK Configuration Editor**;
7. Đặt **Board Type** là **Jiuchuan Technology**;
8. Lưu cấu hình và bắt đầu biên dịch.

## 🔌 Các bước nạp (Flashing)
1. Sử dụng cáp dữ liệu để kết nối máy tính với loa;
2. Sau khi tắt nguồn thiết bị, nhấn và giữ nút nguồn;
3. Chọn cổng nối tiếp (COM Port) tương ứng trong công cụ nạp;
4. Nhấp vào nút nạp, chọn chế độ UART;
5. Vui lòng không nhả nút nguồn trước khi quá trình nạp hoàn tất.


## Chân (Pins)
- 1-9:
  - 1. DAT2     : NC
  - 2. CD/DAT3  : Chip Select, mức thấp hoạt động. (Chưa rõ)
  - 3. CMD      : IO48 (Command/Response Line), máy chủ gửi lệnh và dữ liệu đến thẻ TF qua đường này
  - 4. VDD      : Cung cấp nguồn
  - 5. CLX      : IO47 (Clock), được máy chủ tạo ra, đồng bộ hóa giao tiếp dữ liệu
  - 6. VSS      : GND
  - 7. DAT0     : IO21, SPI_MISO, thẻ TF trả về phản hồi và dữ liệu cho máy chủ qua đường này
  - 8. DAT1     : NC


  Theo thứ tự từ phải sang trái là 1-9
