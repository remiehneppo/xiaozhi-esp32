# ogg_covertor Bộ chuyển đổi âm thanh OGG hàng loạt XiaoZhiAI

Script này là công cụ chuyển đổi OGG hàng loạt, hỗ trợ chuyển đổi các tệp âm thanh đầu vào sang định dạng OGG mà XiaoZhi có thể sử dụng.

Được triển khai dựa trên thư viện bên thứ ba Python `ffmpeg-python`, **yêu cầu** môi trường `ffmpeg`.

Bạn có thể truy cập [tại đây](https://ffmpeg.org/download.html) để tải phiên bản ffmpeg tương ứng với hệ thống của mình, và thêm nó vào biến môi trường hoặc đặt nó trong thư mục chứa script.

Hỗ trợ chuyển đổi qua lại giữa OGG và âm thanh, điều chỉnh âm lượng và các chức năng khác.

# Tạo và kích hoạt môi trường ảo

```bash
# Tạo môi trường ảo
python -m venv venv
# Kích hoạt môi trường ảo
source venv/bin/activate # Mac/Linux
venv\Scripts\activate # Windows
