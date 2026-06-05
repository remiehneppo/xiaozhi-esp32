# Công cụ chuyển đổi ảnh LVGL

Thư mục này chứa hai script Python dùng để xử lý và chuyển đổi ảnh sang định dạng LVGL:

## 1. LVGLImage (LVGLImage.py)

Script chuyển đổi được tham chiếu từ LVGL[repo chính thức](https://github.com/lvgl/lvgl) [LVGLImage.py](https://github.com/lvgl/lvgl/blob/master/scripts/LVGLImage.py)

## 2. Công cụ chuyển đổi ảnh LVGL (lvgl_tools_gui.py)

Gọi `LVGLImage.py` để chuyển đổi hàng loạt ảnh sang định dạng ảnh LVGL
Có thể được sử dụng để sửa đổi biểu cảm mặc định của Xiao Zhi, hướng dẫn sửa đổi cụ thể [tại đây](https://www.bilibili.com/video/BV12FQkYeEJ3/)

### Tính năng

- Thao tác đồ họa, giao diện thân thiện hơn
- Hỗ trợ chuyển đổi ảnh hàng loạt
- Tự động nhận dạng định dạng ảnh và chọn chuyển đổi định dạng màu tối ưu
- Hỗ trợ đa độ phân giải

### Cách sử dụng

Tạo môi trường ảo
```bash
# Tạo venv
python -m venv venv
# Kích hoạt môi trường
source venv/bin/activate  # Linux/Mac
venv\Scripts\activate      # Windows
