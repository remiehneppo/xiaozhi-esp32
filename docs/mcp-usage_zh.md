# Hướng dẫn sử dụng điều khiển IoT bằng giao thức MCP

> Tài liệu này giới thiệu cách thực hiện điều khiển IoT cho thiết bị ESP32 dựa trên giao thức MCP. Vui lòng tham khảo [`mcp-protocol_zh.md`](./mcp-protocol_zh.md) để biết quy trình giao thức chi tiết.

## Giới thiệu

MCP (Model Context Protocol) là giao thức thế hệ mới được khuyến nghị sử dụng cho điều khiển IoT, cho phép phát hiện và gọi "Công cụ" (Tool) giữa backend và thiết bị thông qua định dạng JSON-RPC 2.0 tiêu chuẩn, giúp thực hiện điều khiển thiết bị linh hoạt.

## Quy trình sử dụng điển hình

1. Sau khi thiết bị khởi động, thiết lập kết nối với backend thông qua giao thức cơ bản (như WebSocket/MQTT).
2. Backend khởi tạo phiên bằng phương thức `initialize` của giao thức MCP.
3. Backend lấy tất cả các công cụ (chức năng) và mô tả tham số mà thiết bị hỗ trợ thông qua `tools/list`.
4. Backend gọi công cụ cụ thể thông qua `tools/call` để thực hiện điều khiển thiết bị.

Vui lòng xem [`mcp-protocol_zh.md`](./mcp-protocol_zh.md) để biết định dạng và tương tác giao thức chi tiết.

## Hướng dẫn phương thức đăng ký công cụ phía thiết bị

Thiết bị đăng ký "Công cụ" có thể được backend gọi thông qua phương thức `McpServer::AddTool`. Chữ ký hàm thường dùng của nó như sau:

```cpp
void AddTool(
    const std::string& name,           // Tên công cụ, khuyến nghị sử dụng phong cách "module.chức năng", duy nhất và có tính phân cấp
    const std::string& description,    // Mô tả công cụ, mô tả ngắn gọn chức năng, giúp mô hình lớn dễ hiểu
    const PropertyList& properties,    // Danh sách tham số đầu vào (có thể trống), hỗ trợ kiểu: Boolean, Integer, String
    std::function<ReturnValue(const PropertyList&)> callback // Triển khai callback khi công cụ được gọi
);
