# Cung cấp BluFi (với `esp-wifi-connect`)

Tài liệu này giải thích cách bật và sử dụng BluFi (cung cấp WiFi dựa trên BLE) trong firmware XiaoZhi, cùng với thành phần `esp-wifi-connect` trong cây dự án xử lý kết nối WiFi và lưu trữ thông tin xác thực. Xem [tài liệu BluFi chính thức của Espressif](https://docs.espressif.com/projects/esp-idf/en/stable/esp32/api-guides/ble/blufi.html) để biết chi tiết về giao thức.

## Điều kiện tiên quyết

- Một chip và cấu hình firmware hỗ trợ BLE.
- Trong `idf.py menuconfig`, bật `WiFi Configuration Method -> Esp Blufi` (`CONFIG_USE_ESP_BLUFI_WIFI_PROVISIONING=y`). Nếu bạn muốn sử dụng BluFi, hãy tắt tùy chọn Hotspot trong cùng menu; nếu không, việc cung cấp hotspot sẽ thắng mặc định.
- Giữ nguyên việc khởi tạo NVS và event-loop mặc định do `app_main` của dự án cung cấp.
- Chính xác một trong `CONFIG_BT_BLUEDROID_ENABLED` / `CONFIG_BT_NIMBLE_ENABLED` phải được chọn; chúng loại trừ lẫn nhau.

## Quy trình làm việc

1. Một điện thoại (sử dụng ứng dụng EspBlufi chính thức hoặc một client BluFi khác) kết nối với thiết bị qua BLE và gửi SSID / mật khẩu WiFi mục tiêu. Điện thoại cũng có thể yêu cầu danh sách các mạng WiFi được thiết bị quét thông qua giao thức BluFi.
2. Trong `ESP_BLUFI_EVENT_REQ_CONNECT_TO_AP`, thiết bị lưu trữ thông tin xác thực vào `SsidManager` (được lưu trữ trong NVS bởi thành phần `esp-wifi-connect`).
3. Thiết bị sau đó khởi chạy `WifiStation` để quét và kết nối; tiến trình được báo cáo lại qua BluFi.
4. Nếu việc cung cấp thành công, thiết bị tự động kết nối với WiFi mới. Nếu thất bại, một trạng thái lỗi sẽ được gửi lại.

## Các bước

1. **Cấu hình**: bật `Esp Blufi` trong menuconfig, sau đó build và flash firmware.
2. **Kích hoạt cung cấp**: ở lần khởi động đầu tiên mà không có thông tin xác thực WiFi được lưu trữ, thiết bị tự động vào chế độ cung cấp.
3. **Phía điện thoại**: mở ứng dụng EspBlufi (hoặc client BluFi khác), quét và kết nối với thiết bị, tùy chọn bật mã hóa, sau đó nhập SSID / mật khẩu WiFi và gửi chúng.
4. **Quan sát kết quả**:
   - Thành công: BluFi báo cáo thành công và thiết bị kết nối với WiFi.
   - Thất bại: BluFi báo cáo thất bại; thử lại hoặc kiểm tra bộ định tuyến.

## Lưu ý

- BluFi không thể được sử dụng đồng thời với việc cung cấp hotspot. Nếu việc cung cấp hotspot đã được bật, thiết bị sẽ sử dụng nó. Chỉ giữ một phương thức cung cấp trong menuconfig.
- Khi chạy các bài kiểm tra lặp lại, hãy xóa hoặc ghi đè SSID đã lưu (`wifi` namespace NVS) để tránh thông tin xác thực cũ can thiệp vào lần chạy tiếp theo.
- Nếu bạn tự viết client BluFi của mình, hãy làm theo định dạng khung giao thức chính thức được liên kết ở trên.
- Các liên kết tải xuống ứng dụng EspBlufi được liệt kê trong tài liệu chính thức.
- Vì API BluFi đã thay đổi trong IDF 5.5.2, firmware được build với 5.5.2 quảng cáo tên Bluetooth là `"Xiaozhi-Blufi"`, trong khi 5.5.1 sử dụng `"BLUFI_DEVICE"`.
