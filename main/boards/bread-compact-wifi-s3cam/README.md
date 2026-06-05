Phần cứng dựa trên bo mạch phát triển ESP32S3CAM, mã nguồn được sửa đổi dựa trên bread-compact-wifi-lcd.
Camera được sử dụng là OV2640.
Lưu ý rằng vì camera chiếm nhiều IO, nên nó đã chiếm hai chân USB 19 và 20 của ESP32S3.
Tham khảo định nghĩa chân trong tệp config.h về cách đấu dây.

 
# Lệnh cấu hình biên dịch

**Cấu hình mục tiêu biên dịch là ESP32S3:**

```bash
idf.py set-target esp32s3
