# ESP-SensairShuttle

## Giới thiệu

<div align="center">
    <a href="https://docs.espressif.com/projects/esp-dev-kits/zh_CN/latest/esp32c5/esp-sensairshuttle/index.html">
        <b> Tài liệu bảng phát triển </b>
    </a>
    |
    <a href="#support-sensor--shuttleboard-sub-board">
        <b> Tài liệu Cảm biến & <i>ShuttleBoard</i> </b>
    </a>
</div>

ESP-SensairShuttle là bảng phát triển được ra mắt chung bởi Espressif và Bosch Sensortec, hướng đến các kịch bản **nhận thức chuyển động** và **tương tác người-máy với mô hình lớn**.

ESP-SensairShuttle sử dụng module ESP32-C5-WROOM-1-N16R8 của Espressif, có khả năng giao tiếp không dây tần kép 2.4 & 5 GHz Wi-Fi 6 (802.11ax), Bluetooth® 5 (LE), Zigbee và Thread (802.15.4).

## Hỗ trợ bo mạch phụ Cảm biến & _ShuttleBoard_

Sắp ra mắt, xin hãy chờ đón.

## Cấu hình, lệnh biên dịch

Vì ESP-SensairShuttle yêu cầu cấu hình nhiều tùy chọn sdkconfig, nên khuyến nghị sử dụng script biên dịch.

**Biên dịch**

```bash
python ./scripts/release.py esp-sensairshuttle

