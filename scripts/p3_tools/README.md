# Công cụ chuyển đổi và phát định dạng âm thanh P3

Thư mục này chứa hai script Python để xử lý các tệp âm thanh định dạng P3:

## 1. Công cụ chuyển đổi âm thanh (convert_audio_to_p3.py)

Chuyển đổi tệp âm thanh thông thường sang định dạng P3 (cấu trúc luồng của header 4 byte + gói dữ liệu Opus) và chuẩn hóa âm lượng.

### Cách sử dụng

```bash
python convert_audio_to_p3.py <Tệp âm thanh đầu vào> <Tệp P3 đầu ra> [-l LUFS] [-d]
