# Trang giới thiệu sản phẩm

## Giới thiệu
Camera AI của Zhengchen Technology là một dự án tùy chỉnh (mod) của XiaoZhi AI, đã thực hiện nhiều đổi mới và tối ưu hóa.

## Phiên bản hợp nhất
Mã nguồn phiên bản hợp nhất được duy trì trong dự án chính của XiaoZhi AI, cập nhật phiên bản cùng với dự án chính, giúp người dùng tự mở rộng và mở rộng firmware của bên thứ ba. Hỗ trợ các chức năng như đánh thức bằng giọng nói, ngắt bằng giọng nói, OTA, v.v.

## Phiên bản tùy chỉnh (mod)
Phiên bản tùy chỉnh (mod) do thay đổi nền tảng quá lớn nên mã nguồn được duy trì riêng, và được hợp nhất với mã nguồn dự án chính định kỳ.

https://e.tb.cn/h.6Gl2LC7rsrswQZp?tk=qFuaV9hzh0k CZ356
【Taobao】 "XiaoZhi AI có camera, hỗ trợ nhận diện vật thể, bo mạch ESP32S3N16R8 với hai micro và khả năng ngắt lời bằng giọng nói"
https://e.tb.cn/h.hBc8Gcx9cUluJJO?tk=YW5C4LPixKg



## Lệnh cấu hình và biên dịch

Vì dự án này cần cấu hình nhiều tùy chọn sdkconfig, nên khuyến nghị sử dụng script biên dịch.

**Biên dịch**

```bash
python ./scripts/release.py zhengchen-cam
