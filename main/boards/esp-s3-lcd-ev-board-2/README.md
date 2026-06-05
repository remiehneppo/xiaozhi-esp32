Vui lòng xác nhận phiên bản phần cứng của bo mạch phát triển của bạn. Nếu có phiên bản phần cứng, hãy chọn ev_board type trong cấu hình.
1.4 và 1.5 chỉ thay đổi ở IO.
Bạn có thể xem tài liệu chính thức để xác nhận các chi tiết cụ thể https://docs.espressif.com/projects/esp-dev-kits/en/latest/esp32s3/esp32-s3-lcd-ev-board/user_guide.html
Điều chỉnh cụ thể như sau:
I2C_SCL     IO18    ->     IO48
I2C_SDA     IO8     ->     IO47
LCD_DATA6   IO47    ->     IO8
LCD_DATA7   IO48    ->     IO18

Phiên bản này chỉ hỗ trợ màn hình 800x480.
