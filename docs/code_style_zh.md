# Hướng dẫn phong cách mã

## Công cụ định dạng mã

Dự án này dùng `clang-format` để thống nhất phong cách code. Ở thư mục gốc đã có file cấu hình `.clang-format`, dựa trên Google C++ Style Guide và có một số tinh chỉnh riêng.

### Cài đặt clang-format

Trước khi dùng, hãy নিশ্চিত bảo bạn đã cài `clang-format`:

- **Windows**:
  ```powershell
  winget install LLVM
  # hoặc dùng Chocolatey
  choco install llvm
  ```

- **Linux**:
  ```bash
  sudo apt install clang-format  # Ubuntu/Debian
  sudo dnf install clang-tools-extra  # Fedora
  ```

- **macOS**:
  ```bash
  brew install clang-format
  ```

### Cách dùng

1. **Định dạng một file**:
   ```bash
   clang-format -i path/to/your/file.cpp
   ```

2. **Định dạng toàn bộ dự án**:
   ```bash
   # Chạy ở thư mục gốc dự án
   find main -iname *.h -o -iname *.cc | xargs clang-format -i
   ```

3. **Kiểm tra trước khi commit**:
   ```bash
   # Kiểm tra file có đúng chuẩn hay không, không sửa file
   clang-format --dry-run -Werror path/to/your/file.cpp
   ```

### Tích hợp IDE

- **Visual Studio Code**:
  1. Cài extension C/C++
  2. Trong phần Settings, đặt `C_Cpp.formatting` thành `clang-format`
  3. Có thể bật tự định dạng khi lưu: `editor.formatOnSave: true`

- **CLion**:
  1. Vào `Editor > Code Style > C/C++`
  2. Đặt `Formatter` thành `clang-format`
  3. Chọn dùng file cấu hình `.clang-format` trong dự án

### Quy tắc chính

- Thụt lề 4 dấu cách
- Giới hạn độ dài dòng ở 100 ký tự
- Dùng kiểu brace Attach
- Dấu `*` và `&` căn về bên trái
- Tự động sắp xếp `#include`
- Độ thụt của access specifier là -4 dấu cách

### Lưu ý

1. Trước khi nộp code, hãy chắc chắn đã format đầy đủ
2. Đừng chỉnh tay lại căn lề của code đã được format
3. Nếu muốn một đoạn code không bị format, hãy bọc bằng:
   ```cpp
   // clang-format off
   // code của bạn
   // clang-format on
   ```

### Câu hỏi thường gặp

1. **Format thất bại**:
   - Kiểm tra phiên bản `clang-format` có quá cũ không
   - Đảm bảo file mã hóa UTF-8
   - Xác minh cú pháp file `.clang-format`

2. **Kết quả khác mong đợi**:
   - Kiểm tra dự án có đang dùng file `.clang-format` ở thư mục gốc hay không
   - Đảm bảo không có file `.clang-format` khác được ưu tiên hơn

Nếu có câu hỏi hoặc góp ý, cứ mở issue hoặc pull request.
