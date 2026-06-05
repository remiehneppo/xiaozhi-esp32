# Hướng Dẫn Phong Cách Mã Nguồn

## Công Cụ Định Dạng

Dự án này sử dụng `clang-format` để giữ cho phong cách mã nguồn nhất quán. Tệp `.clang-format` trong thư mục gốc của dự án dựa trên hướng dẫn phong cách C++ của Google với một vài điều chỉnh dành riêng cho dự án.

### Cài Đặt clang-format

Hãy đảm bảo `clang-format` có sẵn trước khi bạn sử dụng nó:

- **Windows**:
  ```powershell
  winget install LLVM
  # hoặc với Chocolatey
  choco install llvm
  ```

- **Linux**:
  ```bash
  sudo apt install clang-format              # Ubuntu/Debian
  sudo dnf install clang-tools-extra         # Fedora
  ```

- **macOS**:
  ```bash
  brew install clang-format
  ```

### Cách Sử Dụng

1. **Định dạng một tệp duy nhất**:
   ```bash
   clang-format -i path/to/your/file.cpp
   ```

2. **Định dạng toàn bộ dự án**:
   ```bash
   # Chạy từ thư mục gốc của dự án
   find main -iname '*.h' -o -iname '*.cc' | xargs clang-format -i
   ```

3. **Kiểm tra định dạng mà không sửa đổi tệp (hữu ích trong CI / pre-commit)**:
   ```bash
   clang-format --dry-run -Werror path/to/your/file.cpp
   ```

### Tích Hợp IDE

- **Visual Studio Code**:
  1. Cài đặt tiện ích mở rộng C/C++.
  2. Đặt `C_Cpp.formatting` thành `clangFormat` trong cài đặt.
  3. Tùy chọn bật `editor.formatOnSave`.

- **CLion**:
  1. Mở `Editor > Code Style > C/C++` trong cài đặt.
  2. Đặt `Formatter` thành `clang-format`.
  3. Chọn "use the .clang-format file in the project".

### Các Quy Tắc Chính

- Thụt lề bằng 4 khoảng trắng.
- Chiều rộng dòng được giới hạn ở 100 ký tự.
- Dấu ngoặc kiểu attach-style (`{` trên cùng dòng với câu lệnh điều khiển).
- Con trỏ và tham chiếu liên kết với kiểu (căn trái).
- Các include được sắp xếp tự động.
- Các bộ chỉ định truy cập được thụt lề bằng -4 khoảng trắng.

### Lưu Ý

1. Hãy đảm bảo mã đã được định dạng trước khi commit.
2. Không sửa thủ công sự căn chỉnh sau khi chạy clang-format.
3. Để loại trừ một khối khỏi việc định dạng, hãy bao nó bằng:
   ```cpp
   // clang-format off
   your code
   // clang-format on
   ```

### FAQ

1. **Định dạng thất bại**:
   - Kiểm tra xem `clang-format` có quá cũ không.
   - Đảm bảo tệp được mã hóa bằng UTF-8.
   - Xác thực cú pháp của tệp `.clang-format` của bạn.

2. **Kết quả khác với mong đợi**:
   - Xác minh rằng `.clang-format` trong thư mục gốc của dự án thực sự được chọn.
   - Đảm bảo không có `.clang-format` nào khác ở cấp cao hơn trong cây đang thắng thế.

Chào mừng các câu hỏi và đề xuất - vui lòng mở một issue hoặc một pull request.
