# ESP-Hi

## Giới thiệu

<div align="center">
    <a href="https://oshwhub.com/esp-college/esp-hi"><b> Nền tảng mã nguồn mở Lichuang </b></a>
    |
    <a href="https://www.bilibili.com/video/BV1BHJtz6E2S"><b> Bilibili </b></a>
</div>

ESP-Hi là một robot trò chuyện AI siêu **giá thành thấp** mã nguồn mở do ESP Friends phát triển, dựa trên ESP32C3. ESP-Hi tích hợp một màn hình màu 0.96 inch để hiển thị biểu cảm, và **chó robot đã thực hiện hàng chục động tác**. Bằng cách khai thác triệt để các ngoại vi của ESP32-C3, chỉ cần phần cứng cấp bo mạch tối thiểu là có thể thực hiện thu âm và phát âm, đồng thời tối ưu hóa phần mềm để giảm mức sử dụng bộ nhớ và Flash, đạt được đồng thời **phát hiện từ khóa đánh thức** và nhiều trình điều khiển ngoại vi trong điều kiện tài nguyên hạn chế. Chi tiết phần cứng có thể xem tại [Dự án mã nguồn mở Lichuang](https://oshwhub.com/esp-college/esp-hi).

## WebUI

ESP-Hi x XiaoZhi tích hợp một WebUI để điều khiển chuyển động của cơ thể. Vui lòng kết nối điện thoại của bạn với ESP-Hi trong cùng một mạng Wi-Fi, sau đó truy cập `http://esp-hi.local/` để sử dụng.

Nếu bạn muốn vô hiệu hóa, hãy bỏ chọn `ESP_HI_WEB_CONTROL_ENABLED`, tức là bỏ chọn `Component config` → `Servo Dog Configuration` → `Web Control` → `Enable ESP-HI Web Control`.

## Lệnh cấu hình, biên dịch

Vì ESP-Hi cần cấu hình khá nhiều tùy chọn sdkconfig, nên khuyến nghị sử dụng script biên dịch.

**Biên dịch**

```bash
python ./scripts/release.py esp-hi
