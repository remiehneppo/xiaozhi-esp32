# Thông tin sản phẩm
# Zhengchen Technology AI camera + 4G

## Giới thiệu
AI camera của Zhengchen Technology là một dự án chỉnh sửa sâu từ Xiaozhi AI, với nhiều cải tiến và tối ưu hóa.

## Bản hợp nhất
Mã nguồn của bản hợp nhất được duy trì trong dự án Xiaozhi AI chính và cập nhật cùng nhịp với dự án gốc, giúp người dùng dễ mở rộng cũng như dễ tích hợp firmware của bên thứ ba. Hỗ trợ đánh thức bằng giọng nói, ngắt bằng giọng nói, OTA và các tính năng khác.

## Bản tùy biến
Do thay đổi ở tầng nền tảng quá lớn, mã nguồn của bản tùy biến được duy trì riêng và định kỳ hợp nhất từ mã nguồn dự án chính.

https://e.tb.cn/h.6Gl2LC7rsrswQZp?tk=qFuaV9hzh0k CZ356
```
【Taobao】 "Xiaozhi AI có camera, hỗ trợ nhận dạng vật thể, ngắt bằng hai micro, bo phát triển ESP32S3N16R8"
https://e.tb.cn/h.hBc8Gcx9cUluJJO?tk=YW5C4LPixKg



## Lệnh cấu hình và biên dịch

Vì dự án này cần cấu hình khá nhiều tùy chọn `sdkconfig`, nên khuyến nghị dùng script biên dịch.

**Biên dịch**

```bash
python ./scripts/release.py zhengchen-cam-ml307
```

Nếu cần biên dịch thủ công, hãy tham khảo `zhengchen-cam-ml307/config.json` để chỉnh các tùy chọn tương ứng trong `menuconfig`.

**Nạp firmware**

```bash
idf.py flash


```

MCP Tool：
self.get_device_status
self.audio_speaker.set_volume
self.screen.set_brightness
self.screen.set_theme
self.gif.set_gif_mode
self.display.set_mode
self.camera.take_photo       
self.AEC.set_mode
self.AEC.get_mode
self.res.esp_restart
