# NULLLAB-AI-VOX3

## Tổng quan

AI-VOX3 là phiên bản nâng cấp của AI VOX, một bo mạch phát triển nhúng hiệu năng cao được thiết kế đặc biệt cho các ứng dụng tương tác giọng nói AI. Lõi của nó sử dụng chip ESP32-S3-R8 và tích hợp bộ nhớ Flash 16MB trên bo mạch. Nó tích hợp các tài nguyên phần cứng phong phú, hỗ trợ phát triển nhanh chóng và mở rộng linh hoạt. Với chức năng tất cả trong một (Trò chuyện AI/Đồng hồ thời tiết/Bộ đàm không dây/Máy nghe nhạc MP3/Đài mạng), nó hỗ trợ đánh thức giọng nói cục bộ, nhận dạng lệnh và tổng hợp giọng nói, có thể được ứng dụng rộng rãi trong nhà thông minh, thiết bị giáo dục và các thiết bị đầu cuối IoT. Kích thước PCB của nó tương thích với các chốt Lego, có thể lắp trực tiếp vào khối C của Lego, thuận tiện cho việc lắp ráp DIY. Đồng thời, nó đi kèm với bo mạch mở rộng AI-VOX3 và bo mạch điều khiển động cơ MD40, giúp nhà phát triển nhanh chóng xây dựng nguyên mẫu dựa trên tài nguyên trên bo mạch và mở rộng chức năng cá nhân hóa thông qua các giao diện phong phú, giảm đáng kể chu kỳ phát triển.

## Tính năng nổi bật

- Trang bị bộ xử lý lõi kép Xtensa 32-bit LX7 hiệu năng cao ESP32-S3R8, tần số chính lên đến 240MHz
- Hỗ trợ Wi-Fi 2.4 GHz (802.11 b/g/n) và Bluetooth 5 (LE), có ăng-ten tích hợp trên bo mạch
- Chip ESP32-S3R8 tích hợp 512 KB SRAM và 384 KB ROM cùng 8MB PSRAM, chip lưu trữ Flash 16 MB trên bo mạch
- Sử dụng giao diện Type-C, hỗ trợ tải chương trình, cấp nguồn trên bo mạch và sạc pin lithium, tương thích với các môi trường phát triển chính, đơn giản hóa quy trình phát triển và quản lý nguồn điện
- Sử dụng nút nguồn/reset hai trong một, tích hợp chức năng reset hệ thống và bật/tắt nguồn vào nút Power. Nhấn ngắn để bật nguồn hoặc reset hệ thống, nhấn dài để tắt nguồn, đơn giản hóa thao tác
- Có thể kết nối LCD giao diện SPI 1.54 inch độ phân giải 240×240 (ST7789), cung cấp giao diện tương tác đồ họa trực quan
- Dự trữ đầu nối cáp LCD và cổng OLED, có thể chọn hiển thị bằng OLED hoặc màn hình màu LCD
- Bộ mã hóa/giải mã âm thanh ES8311 và bộ khuếch đại âm thanh 3W (NS4150B) tích hợp trên bo mạch, hỗ trợ đầu vào/đầu ra âm thanh chất lượng cao, cần kết nối loa ngoài
- Thiết kế micro kép, có micro analog tích hợp trên bo mạch, cũng có thể gắn micro analog ngoài, hỗ trợ ngắt lời bằng micro đơn
- Cổng SD Card tích hợp trên bo mạch, hỗ trợ mở rộng lưu trữ dung lượng lớn
- Nút BOOT tích hợp trên bo mạch, 2 nút (GPIO46/45) và đèn RGB WS2812B, thuận tiện cho việc gỡ lỗi tương tác và chỉ thị trạng thái
- Cung cấp một nhóm 8 chân cắm GPIO (GPIO43/44/42/48/4/3/2/1), hỗ trợ kết nối nhiều thiết bị ngoại vi
- Dự trữ một cổng PH2.0 4pin, có thể dễ dàng cấp nguồn qua PH2.0 hoặc giao tiếp với bộ điều khiển chính khác
- Đi kèm với bo mạch mở rộng AI-VOX3 ngoài, có thể mở rộng thêm nhiều chức năng thông qua các chân cắm của bo mạch mở rộng
- Đi kèm với bo mạch điều khiển động cơ MD40 ngoài, có thể chạy nhiều động cơ thông qua bo mạch điều khiển
- Mạch tích hợp trên bo mạch để tăng áp sạc 5V 2.4A, hỗ trợ cấp nguồn bằng pin lithium ngoài và kiểm tra điện lượng theo thời gian thực thông qua ADC IO18
- Hỗ trợ ESP-IDF, Arduino IDE, AilyBlockly

## Hướng dẫn nút Power

AI-VOX3 đã loại bỏ nút Reset truyền thống, thay vào đó sử dụng nút Power để thao tác thống nhất. Nhấn nút Power một lần sẽ bật nguồn hoặc reset hệ thống, nhấn giữ nút Power sẽ tắt nguồn.

