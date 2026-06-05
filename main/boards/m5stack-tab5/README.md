# Hướng dẫn sử dụng 

* [M5Stack Tab5 docs](https://docs.m5stack.com/zh_CN/core/Tab5)

## Trải nghiệm nhanh

Vào [M5Burner](https://docs.m5stack.com/zh_CN/uiflow/m5burner/intro), chọn Tab5 và tìm Xiaozhi để tải firmware.

## Sử dụng cơ bản

* idf version: v5.5.2 or above (recommended: v6.0-dev)

* No dependency override needed — the project already specifies the correct `esp_video` and `esp_ipa` versions in `main/idf_component.yml`. Do NOT change the dependency versions unless you are also modifying the source code to match the older API.

Đối với người dùng ESP32-P4 Rev <3.0:
hãy নিশ্চিত đảm `sdkconfig.defaults` của bạn có:

CONFIG_ESP32P4_SELECTS_REV_LESS_V3=y

Nếu không, khi nạp sẽ xuất hiện lỗi: `'bootloader/bootloader.bin' requires chip revision in range [v3.0 - v3.99] (this chip is revision v1.x)`

1. Biên dịch bằng `release.py`

```shell
python ./scripts/release.py m5stack-tab5
```

Nếu cần biên dịch thủ công, hãy tham khảo `m5stack-tab5/config.json` để sửa các tùy chọn tương ứng trong `menuconfig`.

2. Biên dịch và nạp chương trình

```shell
idf.py flash monitor
```

> [!NOTE]
> Vào chế độ tải xuống bằng cách nhấn giữ nút reset khoảng 2 giây cho đến khi đèn LED xanh bên trong bắt đầu nhấp nháy nhanh, rồi thả nút.


## Log

@2025/05/17 Vấn đề kiểm thử

1. listening... cần đợi vài giây mới lấy được đầu vào giọng nói???
2. Điều chỉnh độ sáng không đúng
3. Điều chỉnh âm lượng không đúng
