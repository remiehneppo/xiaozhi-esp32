# Quy trình tương tác MCP (Model Context Protocol)

LƯU Ý: Được AI hỗ trợ tạo, khi triển khai dịch vụ backend, vui lòng tham khảo mã để xác nhận chi tiết!!

Giao thức MCP trong dự án này được sử dụng để giao tiếp giữa API backend (Client MCP) và thiết bị ESP32 (Server MCP), cho phép backend phát hiện và gọi các chức năng (công cụ) do thiết bị cung cấp.

## Định dạng giao thức

Dựa trên mã (`main/protocols/protocol.cc`, `main/mcp_server.cc`), tin nhắn MCP được đóng gói trong phần thân tin nhắn của giao thức truyền thông cơ bản (như WebSocket hoặc MQTT). Cấu trúc bên trong của nó tuân theo quy cách [JSON-RPC 2.0](https://www.jsonrpc.org/specification).

Ví dụ về cấu trúc tin nhắn tổng thể:

```json
{
  "session_id": "...", // ID phiên
  "type": "mcp",       // Loại tin nhắn, cố định là "mcp"
  "payload": {         // Tải trọng JSON-RPC 2.0
    "jsonrpc": "2.0",
    "method": "...",   // Tên phương thức (ví dụ: "initialize", "tools/list", "tools/call")
    "params": { ... }, // Tham số phương thức (đối với request)
    "id": ...,         // ID yêu cầu (đối với request và response)
    "result": { ... }, // Kết quả thực thi phương thức (đối với success response)
    "error": { ... }   // Thông tin lỗi (đối với error response)
  }
}
```

Trong đó, phần `payload` là tin nhắn JSON-RPC 2.0 tiêu chuẩn:

- `jsonrpc`: Chuỗi cố định "2.0".
- `method`: Tên phương thức cần gọi (đối với Request).
- `params`: Tham số của phương thức, một giá trị có cấu trúc, thường là đối tượng (đối với Request).
- `id`: Định danh của yêu cầu, được client cung cấp khi gửi yêu cầu và được trả về nguyên trạng khi server phản hồi. Dùng để khớp yêu cầu và phản hồi.
- `result`: Kết quả khi phương thức thực thi thành công (đối với Success Response).
- `error`: Thông tin lỗi khi phương thức thực thi thất bại (đối với Error Response).

## Quy trình tương tác và thời điểm gửi

Tương tác MCP chủ yếu xoay quanh việc client (API backend) phát hiện và gọi các "công cụ" (Tool) trên thiết bị.

1.  **Thiết lập kết nối và thông báo khả năng**

    - **Thời điểm:** Sau khi thiết bị khởi động và kết nối thành công với API backend.
    - **Bên gửi:** Thiết bị.
    - **Tin nhắn:** Thiết bị gửi tin nhắn "hello" của giao thức cơ bản đến API backend, trong đó chứa danh sách các khả năng mà thiết bị hỗ trợ, ví dụ như hỗ trợ giao thức MCP (`"mcp": true`).
    - **Ví dụ (Không phải tải trọng MCP, mà là tin nhắn giao thức cơ bản):**
      ```json
      {
        "type": "hello",
        "version": ...,
        "features": {
          "mcp": true,
          ...
        },
        "transport": "websocket", // hoặc "mqtt"
        "audio_params": { ... },
        "session_id": "..." // Thiết bị có thể thiết lập sau khi nhận hello từ server
      }
      ```

2.  **Khởi tạo phiên MCP**

    - **Thời điểm:** Sau khi API backend nhận được tin nhắn "hello" từ thiết bị và xác nhận thiết bị hỗ trợ MCP, nó thường được gửi đi như yêu cầu đầu tiên của phiên MCP.
    - **Bên gửi:** API backend (Client).
    - **Phương thức:** `initialize`
    - **Tin nhắn (MCP payload):**

      ```json
      {
        "jsonrpc": "2.0",
        "method": "initialize",
        "params": {
          "capabilities": {
            // Khả năng của client, tùy chọn

            // Liên quan đến thị giác camera
            "vision": {
              "url": "...", // Camera: Địa chỉ xử lý hình ảnh (phải là địa chỉ http, không phải địa chỉ websocket)
              "token": "..." // token url
            }

            // ... các khả năng client khác
          }
        },
        "id": 1 // ID yêu cầu
      }
      ```

    - **Thời điểm thiết bị phản hồi:** Sau khi thiết bị nhận và xử lý yêu cầu `initialize`.
    - **Tin nhắn phản hồi của thiết bị (MCP payload):**
      ```json
      {
        "jsonrpc": "2.0",
        "id": 1, // Khớp với ID yêu cầu
        "result": {
          "protocolVersion": "2024-11-05",
          "capabilities": {
            "tools": {} // Các tools ở đây dường như không liệt kê chi tiết, cần tools/list
          },
          "serverInfo": {
            "name": "...", // Tên thiết bị (BOARD_NAME)
            "version": "..." // Phiên bản firmware thiết bị
          }
        }
      }
      ```

3.  **Phát hiện danh sách công cụ của thiết bị**

    - **Thời điểm:** Khi API backend cần lấy danh sách các chức năng cụ thể (công cụ) mà thiết bị hiện hỗ trợ và cách gọi chúng.
    - **Bên gửi:** API backend (Client).
    - **Phương thức:** `tools/list`
    - **Tin nhắn (MCP payload):**
      ```json
      {
        "jsonrpc": "2.0",
        "method": "tools/list",
        "params": {
          "cursor": "" // Dùng để phân trang, lần yêu cầu đầu tiên là chuỗi rỗng
        },
        "id": 2 // ID yêu cầu
      }
      ```
    - **Thời điểm thiết bị phản hồi:** Sau khi thiết bị nhận yêu cầu `tools/list` và tạo danh sách công cụ.
    - **Tin nhắn phản hồi của thiết bị (MCP payload):**
      ```json
      {
        "jsonrpc": "2.0",
        "id": 2, // Khớp với ID yêu cầu
        "result": {
          "tools": [ // Danh sách đối tượng công cụ
            {
              "name": "self.get_device_status",
              "description": "...",
              "inputSchema": { ... } // Schema tham số
            },
            {
              "name": "self.audio_speaker.set_volume",
              "description": "...",
              "inputSchema": { ... } // Schema tham số
            }
            // ... nhiều công cụ hơn
          ],
          "nextCursor": "..." // Nếu danh sách lớn cần phân trang, ở đây sẽ chứa giá trị cursor cho yêu cầu tiếp theo
        }
      }
      ```
    - **Xử lý phân trang:** Nếu trường `nextCursor` không rỗng, client cần gửi lại yêu cầu `tools/list`, và đưa giá trị `cursor` này vào `params` để lấy trang công cụ tiếp theo.

4.  **Gọi công cụ của thiết bị**

    - **Thời điểm:** Khi API backend cần thực thi một chức năng cụ thể trên thiết bị.
    - **Bên gửi:** API backend (Client).
    - **Phương thức:** `tools/call`
    - **Tin nhắn (MCP payload):**
      ```json
      {
        "jsonrpc": "2.0",
        "method": "tools/call",
        "params": {
          "name": "self.audio_speaker.set_volume", // Tên công cụ cần gọi
          "arguments": {
            // Tham số công cụ, định dạng đối tượng
            "volume": 50 // Tên tham số và giá trị của nó
          }
        },
        "id": 3 // ID yêu cầu
      }
      ```
    - **Thời điểm thiết bị phản hồi thành công:** Sau khi thiết bị nhận yêu cầu `tools/call` và thực
