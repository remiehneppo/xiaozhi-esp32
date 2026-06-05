# BluFi cấu hình mạng (tích hợp esp-wifi-connect)

Tài liệu này mô tả cách bật và sử dụng BluFi (BLE Wi-Fi provisioning) trong firmware XiaoZhi, đồng thời kết hợp với thành phần `esp-wifi-connect` có sẵn trong dự án để hoàn tất việc kết nối và lưu cấu hình Wi-Fi. Tài liệu giao thức BluFi chính thức của Espressif xem tại [Espressif docs](https://docs.espressif.com/projects/esp-idf/zh_CN/stable/esp32/api-guides/ble/blufi.html).

## Điều kiện trước

- Cần chip và cấu hình firmware có hỗ trợ BLE.
- Trong `idf.py menuconfig`, bật `WiFi Configuration Method -> Esp Blufi` (`CONFIG_USE_ESP_BLUFI_WIFI_PROVISIONING=y`). Nếu muốn dùng BluFi, phải tắt tùy chọn Hotspot trong cùng menu, nếu không hệ thống sẽ mặc định dùng chế độ Hotspot.
- Giữ nguyên khởi tạo NVS và event loop mặc định, `app_main` của dự án đã xử lý sẵn.
- Hai macro `CONFIG_BT_BLUEDROID_ENABLED` và `CONFIG_BT_NIMBLE_ENABLED` chỉ được bật một trong hai, không được bật đồng thời.

## Luồng hoạt động

1. Điện thoại kết nối vào thiết bị qua BluFi, ví dụ bằng EspBlufi App chính thức hoặc client tự viết, rồi gửi SSID/mật khẩu Wi-Fi. Client cũng có thể lấy danh sách Wi-Fi mà thiết bị quét được qua giao thức BluFi.
2. Ở phía thiết bị, trong sự kiện `ESP_BLUFI_EVENT_REQ_CONNECT_TO_AP`, thông tin xác thực được ghi vào `SsidManager` và lưu xuống NVS, đây là phần thuộc `esp-wifi-connect`.
3. Sau đó `WifiStation` được khởi động để quét và kết nối; trạng thái sẽ được trả ngược về qua BluFi.
4. Khi cấu hình thành công, thiết bị sẽ tự động kết nối vào Wi-Fi mới; nếu thất bại, trạng thái lỗi sẽ được trả về.

## Cách sử dụng

1. Cấu hình: mở `menuconfig`, bật `Esp Blufi`, rồi build và nạp firmware.
2. Kích hoạt provisioning: khi thiết bị khởi động lần đầu và chưa có Wi-Fi đã lưu, nó sẽ tự động vào chế độ provisioning.
3. Thao tác trên điện thoại: mở EspBlufi App hoặc client BluFi khác, tìm và kết nối tới thiết bị. Có thể chọn mã hóa hoặc không, sau đó nhập SSID/mật khẩu Wi-Fi và gửi đi.
4. Quan sát kết quả:
   - Thành công: BluFi báo kết nối thành công, thiết bị sẽ tự kết nối Wi-Fi.
   - Thất bại: BluFi trả về trạng thái lỗi, có thể gửi lại hoặc kiểm tra router.

## Lưu ý

- BluFi không dùng đồng thời với chế độ hotspot provisioning. Nếu hotspot đã bật thì hệ thống sẽ ưu tiên hotspot. Hãy chỉ giữ lại một phương thức provisioning trong `menuconfig`.
- Nếu thử nhiều lần, nên xóa hoặc ghi đè SSID đã lưu trong namespace `wifi` để tránh cấu hình cũ gây nhiễu.
- Nếu dùng client BluFi tự viết, phải tuân thủ định dạng frame theo tài liệu chính thức.
- Tài liệu chính thức đã cung cấp link tải EspBlufi App.
- Do API blufi trong IDF 5.5.2 có thay đổi, tên Bluetooth sau khi build bằng 5.5.2 là `Xiaozhi-Blufi`, còn trong 5.5.1 là `BLUFI_DEVICE`.
