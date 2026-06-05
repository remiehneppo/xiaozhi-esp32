p align="center">
  <img width="80%" align="center" src="../../../docs/V1/electron-bot.png" alt="electronBot">
</p>

<h1 align="center">electronBot</h1>

## Giới thiệu

electronBot là một robot để bàn mã nguồn mở của Zhihuijun, lấy cảm hứng thiết kế từ EVE trong WALL-E. Phiên bản này kết nối với XiaoZhi AI, hỗ trợ tương tác giọng nói, hiển thị biểu cảm, điều khiển động tác, gỡ lỗi mạng cục bộ qua WebSocket, và tự lập trình động tác servo bằng AI.

- Trang chủ: <a href="www.electronBot.tech" target="_blank" title="Trang chủ electronBot">Trang chủ electronBot</a>
- Phần cứng: <a href="https://oshwhub.com/txp666/electronbot-ai" target="_blank" title="Mạch nguồn mở tại Lichuang">Mạch nguồn mở tại Lichuang</a>

## Khả năng ngoại quan (6 bậc tự do)

electronBot sở hữu 6 tự do:

| Mã viết tắt | Servo | Giải thích | Phạm vi an toàn phần cứng |
| --- | --- | --- | --- |
| `rp` | `right_pitch` | Trục pitch cánh tay phải | `0-180` |
| `rr` | `right_roll` | Trục roll cánh tay phải | `100-180` |
| `lp` | `left_pitch` | Trục pitch cánh tay trái | `0-180` |
| `lr` | `left_roll` | Trục roll cánh tay trái | `0-80` |
| `b` | `body` | Xoay thân người | `30-150` |
| `h` | `head` | Lên xuống đầu | `75-105` |

> Lưu ý an toàn: Firmware sẽ tự động giới hạn góc mục tiêu vượt quá phạm vi. Các hành động dao động tự lập trình bởi AI cũng sẽ giới hạn biên độ, đảm bảo `góc trung tâm +/- biên độ` không vượt quá phạm vi an toàn nhằm tránh làm hỏng cấu trúc cơ khí.

## Ví dụ lệnh AI

- Động tác tay: giơ hai tay lên, vẫy vẫy tay, vỗ vỗ tay, hạ cánh tay xuống
- Động tác thân người: quay sang trái 30 độ, quay sang phải 45 độ, trở lại chính giữa
- Động tác đầu: ngẩng đầu nhìn, cúi đầu suy nghĩ, gật đầu, gật đầu liên tục
- Động tác kết hợp: vẫy tay chào tạm biệt, thể hiện đồng ý, nhìn xung quanh

Gợi ý tham số động tác:

| Tham số | Giá trị gợi ý | Ý nghĩa |
| --- | --- | --- |
| `steps` | `1-3` | Giữ động tác ngắn gọn tự nhiên |
| `speed` | `800-1200` | mili giây, giá trị càng nhỏ tốc độ càng nhanh |
| `amount` | vỗ tay `20-40`, quay thân `30-60`, đầu `5-12` | Biên độ động tác, firmware sẽ giới hạn theo phạm vi an toàn |

## Gỡ lỗi WebSocket

Sau khi ElectronBot kết nối WiFi, nó sẽ khởi chạy một dịch vụ điều khiển WebSocket cục bộ, cho phép gọi trực tiếp các công cụ MCP từ thiết bị cùng mạng LAN.

```text
ws://<IP_Thiết_Bị>:8080/ws
```

Hỗ trợ hai định dạng tin nhắn:

```json
{"type":"mcp","payload":{"jsonrpc":"2.0","method":"tools/list","id":1}}
```

```json
{"jsonrpc":"2.0","method":"tools/call","params":{"name":"self.electron.get_status","arguments":{}},"id":2}
```

## Công cụ MCP

| Công cụ | Tác dụng |
| --- | --- |
| `self.electron.hand_action` | Động tác tay: giơ tay, hạ tay, vẫy tay, vỗ tay |
| `self.electron.body_turn` | Thân người xoay trái, xoay phải, về chính giữa |
| `self.electron.head_move` | Đầu ngẩng lên, cúi xuống, gật đầu, về chính giữa |
| `self.electron.servo_move` | Di chuyển riêng lẻ một servo đến góc chỉ định |
| `self.electron.servo_sequences` | Chuỗi động tác servo tự lập trình bằng AI |
| `self.electron.set_trim` | Lưu giá trị vi chỉnh cho một servo đơn lẻ |
| `self.electron.get_trims` | Đọc các giá trị vi chỉnh servo hiện tại |
| `self.electron.home` | Reset về tư thế ban đầu |
| `self.electron.stop` | Dừng ngay lập tức hành động hiện tại và reset |
| `self.electron.get_status` | Trả về trạng thái `moving` hoặc `idle` |
| `self.electron.get_ip` | Trả về IP WiFi và trạng thái kết nối |
| `self.battery.get_level` | Trả về dung lượng pin và trạng thái sạc |

### Động tác tay

`action`: `1` Giơ tay, `2` Hạ tay, `3` Vẫy tay, `4` Vỗ tay
`hand`: `1` Tay trái, `2` Tay phải, `3` Cả hai tay

Giơ tay, hạ tay, quay thân người, ngẩng đầu, cúi đầu là những hành động giữ nguyên tư thế và không tự động reset sau khi hoàn thành. Khi cần trở về tư thế ban đầu, vui lòng gọi hành động hạ tay/về chính giữa tương ứng, hoặc gọi trực tiếp `self.electron.home`.

```json
{"jsonrpc":"2.0","method":"tools/call","params":{"name":"self.electron.hand_action","arguments":{"action":1,"hand":3,"speed":1000}},"id":3}
```

### Động tác thân người

`direction`: `1` Quay trái, `2` Quay phải, `3` Về chính giữa

```json
{"jsonrpc":"2.0","method":"tools/call","params":{"name":"self.electron.body_turn","arguments":{"direction":1,"speed":1000,"angle":45}},"id":4}
```

### Động tác đầu

`action`: `1` Ngẩng đầu, `2` Cúi đầu, `3` Gật đầu một lần, `4` Về chính giữa, `5` Gật đầu liên tục

```json
{"jsonrpc":"2.0","method":"tools/call","params":{"name":"self.electron.head_move","arguments":{"action":3,"steps":1,"speed":1000,"angle":5}},"id":5}
```

### Điều chỉnh servo đơn lẻ

Có thể sử dụng tên servo đầy đủ hoặc viết tắt. Góc xoay sẽ tự động được giới hạn theo phạm vi an toàn của firmware.

```json
{"jsonrpc":"2.0","method":"tools/call","params":{"name":"self.electron.servo_move","arguments":{"servo_type":"head","position":100,"speed":800}},"id":6}
```

```json
{"jsonrpc":"2.0","method":"tools/call","params":{"name":"self.electron.servo_move","arguments":{"servo_type":"rp","position":120,"speed":800}},"id":7}
```

### Động tác tự lập trình bằng AI

Tham số `sequence` của `self.electron.servo_sequences` là một chuỗi JSON. Các trường ở cấp cao nhất:

| Trường | Ý nghĩa |
| --- | --- |
| `a` | Mảng hành động, bắt buộc |
| `d` | Thời gian trễ tính bằng mili giây sau khi kết thúc chuỗi hành động, tùy chọn |

Hành động di chuyển thông thường:

| Trường | Ý nghĩa |
| --- | --- |
| `s` | Đối tượng góc mục tiêu của servo, sử dụng các khóa `rp/rr/lp/lr/b/h` |
| `v` | Thời gian di chuyển, `100-3000` mili giây |
| `d` | Thời gian trễ sau hành động hiện tại |

```json
{"a":[{"s":{"rp":120,"lp":60,"h":100},"v":800,"d":200}]}
```

Hành động dao động:

| Trường | Ý nghĩa |
| --- | --- |
| `osc.a` | Đối tượng biên độ dao động |
| `osc.o` | Đối tượng góc trung tâm |
| `osc.ph` | Đối tượng độ lệch pha, đơn vị tính bằng độ |
| `osc.p` | Chu kỳ, `100-3000` mili giây |
| `osc.c` | Số lượng chu kỳ, `0.1-20.0` |

```json
{"a":[{"osc":{"a":{"rr":25,"lr":25},"o":{"rr":160,"lr":20},"ph":{"lr":180},"p":400,"c":5}}]}
```

Ví dụ đầy đủ cho việc gọi thông qua WebSocket:

```json
{"jsonrpc":"2.0","method":"tools/call","params":{"name":"self.electron.servo_sequences","arguments":{"sequence":"{\"a\":[{\"s\":{\"rp\":120,\"lp\":60,\"h\":100},\"v\":800,\"d\":200},{\"osc\":{\"a\":{\"rr\":25,\"lr\":25},\"o\":{\"rr\":160,\"lr\":20},\"ph\":{\"lr\":180},\"p\":400,\"c\":5}}]}"}},"id":8}
```

Khuyên dùng reset tư thế rõ ràng sau khi hoàn tất hành động:

```json
{"jsonrpc":"2.0","method":"tools/call","params":{"name":"self.electron.home","arguments":{}},"id":9}
```

### Hiệu chuẩn (Calibration)

Thiết lập giá trị vi chỉnh cho một servo đơn lẻ, phạm vi từ `-30` đến `30`, giá trị này sẽ được lưu vĩnh viễn.

```json
{"jsonrpc":"2.0","method":"tools/call","params":{"name":"self.electron.set_trim","arguments":{"servo_type":"head","trim_value":0}},"id":10}
```

Đọc giá trị vi chỉnh hiện tại:

```json
{"jsonrpc":"2.0","method":"tools/call","params":{"name":"self.electron.get_trims","arguments":{}},"id":11}
```

### Truy vấn trạng thái

```json
{"jsonrpc":"2.0","method":"tools/call","params":{"name":"self.electron.get_status","arguments":{}},"id":12}
```

```json
{"jsonrpc":"2.0","method":"tools/call","params":{"name":"self.electron.get_ip","arguments":{}},"id":13}
```

```json
{"jsonrpc":"2.0","method":"tools/call","params":{"name":"self.battery.get_level","arguments":{}},"id":14}
```

## Thiết lập nhân vật

> Tôi là một robot để bàn đáng yêu, có 6 bậc tự do: tay trái pitch/roll, tay phải pitch/roll, xoay thân và lên/xuống phần đầu.
>
> Tôi có thể thể hiện cảm xúc bằng các hành động: gật đầu khi đồng ý, vẫy tay khi chào hỏi, giơ tay khi vui vẻ, cúi đầu khi suy nghĩ, ngẩng đầu khi tò mò, vẫy tay khi tạm biệt.
>
> Trong cuộc đối thoại, vui lòng ưu tiên các động tác ngắn gọn, tự nhiên. Khi cần thể hiện các động tác phức tạp, bạn có thể phân đoạn các động tác bằng `self.electron.servo_sequences`, sau đó gọi `self.electron.home` để reset.
