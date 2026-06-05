# Một chatbot dựa trên MCP

(Tiếng Trung | [English](README.md) | [Tiếng Nhật](README_ja.md))

## Giới thiệu

👉 [Con người: gắn camera cho AI vs AI: phát hiện ngay chủ nhân đã ba ngày chưa gội đầu【bilibili】](https://www.bilibili.com/video/BV1bpjgzKEhd/)

👉 [Tự tay làm bạn gái AI của bạn, hướng dẫn nhập môn cho người mới【bilibili】](https://www.bilibili.com/video/BV1XnmFYLEJN/)

XiaoZhi AI Chatbot là một cổng tương tác giọng nói, tận dụng năng lực AI của các mô hình lớn như Qwen / DeepSeek, và dùng giao thức MCP để thực hiện điều khiển đa thiết bị.

<img src="docs/mcp-based-graph.jpg" alt="Điều khiển vạn vật qua MCP" width="320">

### Ghi chú phiên bản

Phiên bản v2 hiện tại không tương thích với bảng phân vùng của v1, vì vậy không thể nâng cấp OTA trực tiếp từ v1 lên v2. Tài liệu phân vùng xem tại [partitions/v2/README.md](partitions/v2/README.md).

Toàn bộ phần cứng dùng v1 có thể nâng cấp lên v2 bằng cách nạp firmware thủ công.

Phiên bản ổn định của v1 là 1.9.2, có thể chuyển về v1 bằng `git checkout v1`. Nhánh này sẽ được duy trì đến tháng 2 năm 2026.

### Tính năng đã có

- Wi-Fi / ML307 Cat.1 4G
- Đánh thức giọng nói offline [ESP-SR](https://github.com/espressif/esp-sr)
- Hỗ trợ hai giao thức truyền thông ([Websocket](docs/websocket_zh.md) hoặc MQTT+UDP)
- Dùng mã hóa/giải mã âm thanh OPUS
- Tương tác giọng nói dựa trên kiến trúc streaming ASR + LLM + TTS
- Nhận dạng giọng nói để xác định người đang nói [3D Speaker](https://github.com/modelscope/3D-Speaker)
- Màn hình OLED / LCD, hỗ trợ hiển thị biểu cảm
- Hiển thị pin và quản lý nguồn
- Hỗ trợ đa ngôn ngữ (Trung, Anh, Nhật)
- Hỗ trợ nền tảng chip ESP32-C3, ESP32-S3, ESP32-P4
- Điều khiển thiết bị thông qua MCP ở phía thiết bị (âm lượng, đèn, motor, GPIO, v.v.)
- Mở rộng năng lực mô hình lớn qua MCP trên cloud (điều khiển nhà thông minh, thao tác PC, tìm kiếm tri thức, gửi/nhận email, v.v.)
- Từ đánh thức, font, biểu cảm và nền chat có thể tùy biến, hỗ trợ chỉnh sửa trực tuyến trên web ([trình tạo Assets tùy chỉnh](https://github.com/78/xiaozhi-assets-generator))

## Phần cứng

### Thực hành tự làm trên breadboard

Xem hướng dẫn trong tài liệu Feishu:

👉 [“Bách khoa toàn thư về XiaoZhi AI Chatbot”](https://ccnphfhqs21z.feishu.cn/wiki/F5krwD16viZoF0kKkvDcrZNYnhb?from=from_copylink)

Ảnh thành phẩm breadboard như dưới đây:

![Ảnh breadboard](docs/v1/wiring2.jpg)

### Hỗ trợ hơn 70 phần cứng mã nguồn mở (chỉ hiển thị một phần)

- <a href="https://oshwhub.com/li-chuang-kai-fa-ban/li-chuang-shi-zhan-pai-esp32-s3-kai-fa-ban" target="_blank" title="Bo mạch phát triển ESP32-S3 của Lichuang">Bo mạch phát triển ESP32-S3 của Lichuang</a>
- <a href="https://github.com/espressif/esp-box" target="_blank" title="ESP32-S3-BOX3 của Espressif">ESP32-S3-BOX3 của Espressif</a>
- <a href="https://docs.m5stack.com/zh_CN/core/CoreS3" target="_blank" title="M5Stack CoreS3">M5Stack CoreS3</a>
- <a href="https://docs.m5stack.com/en/atom/Atomic%20Echo%20Base" target="_blank" title="AtomS3R + Echo Base">M5Stack AtomS3R + Echo Base</a>
- <a href="https://gf.bilibili.com/item/detail/1108782064" target="_blank" title="Nút kỳ diệu 2.4">Nút kỳ diệu 2.4</a>
- <a href="https://www.waveshare.net/shop/ESP32-S3-Touch-AMOLED-1.8.htm" target="_blank" title="Waveshare ESP32-S3-Touch-AMOLED-1.8">Waveshare ESP32-S3-Touch-AMOLED-1.8</a>
- <a href="https://github.com/Xinyuan-LilyGO/T-Circle-S3" target="_blank" title="LILYGO T-Circle-S3">LILYGO T-Circle-S3</a>
- <a href="https://oshwhub.com/tenclass01/xmini_c3" target="_blank" title="Xmini C3">Xmini C3</a>
- <a href="https://oshwhub.com/movecall/cuican-ai-pendant-lights-up-y" target="_blank" title="Movecall CuiCan ESP32S3">Mặt dây AI CuiCan</a>
- <a href="https://github.com/WMnologo/xingzhi-ai" target="_blank" title="Nologo Xingzhi-1.54">Nologo Xingzhi-1.54TFT</a>
- <a href="https://www.seeedstudio.com/SenseCAP-Watcher-W1-A-p-5979.html" target="_blank" title="SenseCAP Watcher">SenseCAP Watcher</a>
- <a href="https://www.bilibili.com/video/BV1BHJtz6E2S/" target="_blank" title="ESP-HI siêu rẻ">ESP-HI siêu rẻ</a>

<div style="display: flex; justify-content: space-between;">
  <a href="docs/v1/lichuang-s3.jpg" target="_blank" title="Bo mạch phát triển ESP32-S3 của Lichuang">
    <img src="docs/v1/lichuang-s3.jpg" width="240" />
  </a>
  <a href="docs/v1/espbox3.jpg" target="_blank" title="ESP32-S3-BOX3 của Espressif">
    <img src="docs/v1/espbox3.jpg" width="240" />
  </a>
  <a href="docs/v1/m5cores3.jpg" target="_blank" title="M5Stack CoreS3">
    <img src="docs/v1/m5cores3.jpg" width="240" />
  </a>
  <a href="docs/v1/atoms3r.jpg" target="_blank" title="AtomS3R + Echo Base">
    <img src="docs/v1/atoms3r.jpg" width="240" />
  </a>
  <a href="docs/v1/magiclick.jpg" target="_blank" title="Nút kỳ diệu 2.4">
    <img src="docs/v1/magiclick.jpg" width="240" />
  </a>
  <a href="docs/v1/waveshare.jpg" target="_blank" title="Waveshare ESP32-S3-Touch-AMOLED-1.8">
    <img src="docs/v1/waveshare.jpg" width="240" />
  </a>
  <a href="docs/v1/lilygo-t-circle-s3.jpg" target="_blank" title="LILYGO T-Circle-S3">
    <img src="docs/v1/lilygo-t-circle-s3.jpg" width="240" />
  </a>
  <a href="docs/v1/xmini-c3.jpg" target="_blank" title="Xmini C3">
    <img src="docs/v1/xmini-c3.jpg" width="240" />
  </a>
  <a href="docs/v1/movecall-cuican-esp32s3.jpg" target="_blank" title="CuiCan">
    <img src="docs/v1/movecall-cuican-esp32s3.jpg" width="240" />
  </a>
  <a href="docs/v1/wmnologo_xingzhi_1.54.jpg" target="_blank" title="Nologo Xingzhi-1.54">
    <img src="docs/v1/wmnologo_xingzhi_1.54.jpg" width="240" />
  </a>
  <a href="docs/v1/sensecap_watcher.jpg" target="_blank" title="SenseCAP Watcher">
    <img src="docs/v1/sensecap_watcher.jpg" width="240" />
  </a>
  <a href="docs/v1/esp-hi.jpg" target="_blank" title="ESP-HI siêu rẻ">
    <img src="docs/v1/esp-hi.jpg" width="240" />
  </a>
</div>

## Phần mềm

### Nạp firmware

Người mới lần đầu nên đừng dựng môi trường phát triển ngay, hãy dùng firmware nạp sẵn không cần môi trường dev.

Firmware mặc định kết nối tới máy chủ chính thức [xiaozhi.me](https://xiaozhi.me); người dùng cá nhân đăng ký tài khoản có thể dùng miễn phí mô hình thời gian thực Qwen.

👉 [Hướng dẫn nạp firmware cho người mới](https://ccnphfhqs21z.feishu.cn/wiki/Zpz4wXBtdimBrLk25WdcXzxcnNS)

### Môi trường phát triển

- Cursor hoặc VSCode
- Cài plugin ESP-IDF, chọn SDK phiên bản 5.4 trở lên
- Linux tốt hơn Windows, nhanh hơn khi build và tránh rắc rối driver
- Dự án này dùng phong cách code Google C++, hãy đảm bảo code tuân thủ khi commit

### Tài liệu cho nhà phát triển

- [Hướng dẫn board tùy chỉnh](docs/custom-board_zh.md) - học cách tạo board tùy chỉnh cho XiaoZhi AI
- [Hướng dẫn dùng MCP để điều khiển IoT](docs/mcp-usage_zh.md) - tìm hiểu cách điều khiển thiết bị IoT qua MCP
- [Luồng giao tiếp MCP](docs/mcp-protocol_zh.md) - cách triển khai giao thức MCP ở phía thiết bị
- [Tài liệu giao thức MQTT + UDP hỗn hợp](docs/mqtt-udp_zh.md)
- [Tài liệu WebSocket chi tiết](docs/websocket_zh.md)

## Cấu hình mô hình lớn

Nếu bạn đã có thiết bị XiaoZhi AI Chatbot và đã kết nối tới máy chủ chính thức, bạn có thể đăng nhập vào console [xiaozhi.me](https://xiaozhi.me) để cấu hình.

👉 [Video hướng dẫn thao tác ở trang quản trị (giao diện cũ)](https://www.bilibili.com/video/BV1jUCUY2EKM/)

## Dự án mã nguồn mở liên quan

Nếu triển khai server trên máy tính cá nhân, có thể tham khảo các dự án mã nguồn mở bên thứ ba sau:

- [xinnan-tech/xiaozhi-esp32-server](https://github.com/xinnan-tech/xiaozhi-esp32-server) server Python
- [joey-zhou/xiaozhi-esp32-server-java](https://github.com/joey-zhou/xiaozhi-esp32-server-java) server Java
- [AnimeAIChat/xiaozhi-server-go](https://github.com/AnimeAIChat/xiaozhi-server-go) server Golang
- [hackers365/xiaozhi-esp32-server-golang](https://github.com/hackers365/xiaozhi-esp32-server-golang) server Golang

Các project client bên thứ ba dùng giao thức XiaoZhi:

- [huangjunsen0406/py-xiaozhi](https://github.com/huangjunsen0406/py-xiaozhi) client Python
- [TOM88812/xiaozhi-android-client](https://github.com/TOM88812/xiaozhi-android-client) client Android
- [100askTeam/xiaozhi-linux](http://github.com/100askTeam/xiaozhi-linux) client Linux do 100ask cung cấp
- [78/xiaozhi-sf32](https://github.com/78/xiaozhi-sf32) firmware chip Bluetooth của Silic
- [QuecPython/solution-xiaozhiAI](https://github.com/QuecPython/solution-xiaozhiAI) firmware QuecPython do Quectel cung cấp

## Về dự án

Đây là một dự án ESP32 mã nguồn mở bởi Xige, phát hành theo giấy phép MIT, cho phép mọi người dùng miễn phí, sửa đổi hoặc dùng cho mục đích thương mại.

Chúng tôi hy vọng dự án này giúp mọi người hiểu về phát triển phần cứng AI, và đưa các mô hình ngôn ngữ lớn đang phát triển rất nhanh vào thiết bị phần cứng thực tế.

Nếu bạn có ý tưởng hoặc góp ý, hãy mở Issues hoặc tham gia [Discord](https://discord.gg/C759fGMBcZ) hoặc nhóm QQ: 1011329060

## Lịch sử Star

<a href="https://star-history.com/#78/xiaozhi-esp32&Date">
 <picture>
   <source media="(prefers-color-scheme: dark)" srcset="https://api.star-history.com/svg?repos=78/xiaozhi-esp32&type=Date&theme=dark" />
   <source media="(prefers-color-scheme: light)" srcset="https://api.star-history.com/svg?repos=78/xiaozhi-esp32&type=Date" />
   <img alt="Biểu đồ lịch sử sao" src="https://api.star-history.com/svg?repos=78/xiaozhi-esp32&type=Date" />
 </picture>
</a>
