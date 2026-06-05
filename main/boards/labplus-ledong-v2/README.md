# Labplus LeDong Control V2

## Tài nguyên trên bo mạch
    Bộ điều khiển chính: ESP32-S3 ngoài 8MB psram 16MB flash	
    Cảm biến:
        Nút (Nút A B)	IO0 IO46
        Cảm biến ánh sáng	IIC
        6 trục	IIC
        Từ kế	IIC
        Kích hoạt bằng âm thanh	IO6
        Nút cảm ứng IIC P Y T H O N
        Camera	IIC
    Bộ truyền động:
        Còi	IO21
        Đèn RGB	IO16
        Phát lại âm thanh es8388	IIC	
        TFT LCD	jd9853 SPI
        Trình điều khiển động cơ	IIC


## Cấu hình biên dịch

### Cấu hình mục tiêu biên dịch là ESP32S3, tải xuống USB JTAG

```bash
idf.py set-target esp32s3
