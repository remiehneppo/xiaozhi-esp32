# Kiến trúc Dịch vụ Âm thanh

Dịch vụ âm thanh là một thành phần cốt lõi chịu trách nhiệm quản lý tất cả các chức năng liên quan đến âm thanh, bao gồm thu âm từ micro, xử lý, mã hóa/giải mã và phát lại âm thanh qua loa. Nó được thiết kế để mô-đun và hiệu quả, chạy các hoạt động chính của nó trong các tác vụ FreeRTOS chuyên dụng để đảm bảo hiệu suất thời gian thực.

## Các Thành phần Chính

-   **`AudioService`**: Bộ điều phối trung tâm. Nó khởi tạo và quản lý tất cả các thành phần âm thanh, tác vụ và hàng đợi dữ liệu khác.
-   **`AudioCodec`**: Một lớp trừu tượng hóa phần cứng (HAL) cho chip codec âm thanh vật lý. Nó xử lý giao tiếp I2S thô cho đầu vào và đầu ra âm thanh.
-   **`AudioProcessor`**: Thực hiện xử lý âm thanh thời gian thực trên luồng đầu vào micro. Điều này thường bao gồm Khử tiếng vọng âm học (AEC), triệt tiêu nhiễu và Phát hiện Hoạt động Giọng nói (VAD). `AfeAudioProcessor` là triển khai mặc định, sử dụng ESP-ADF Audio Front-End.
-   **`WakeWord`**: Phát hiện các từ khóa (ví dụ: "Xin chào, Xiaozhi", "Hi, ESP") từ luồng âm thanh. Nó chạy độc lập với bộ xử lý âm thanh chính cho đến khi phát hiện từ khóa đánh thức.
-   **`OpusEncoderWrapper` / `OpusDecoderWrapper`**: Quản lý việc mã hóa âm thanh PCM sang định dạng Opus và giải mã các gói Opus trở lại PCM. Opus được sử dụng vì khả năng nén cao và độ trễ thấp, làm cho nó lý tưởng cho truyền phát giọng nói.
-   **`OpusResampler`**: Một tiện ích để chuyển đổi các luồng âm thanh giữa các tốc độ mẫu khác nhau (ví dụ: lấy mẫu lại từ tốc độ mẫu gốc của codec sang 16kHz cần thiết cho xử lý).

## Mô hình Luồng (Threading Model)

Dịch vụ hoạt động trên ba tác vụ chính để xử lý các giai đoạn khác nhau của đường ống âm thanh đồng thời:

1.  **`AudioInputTask`**: Chỉ chịu trách nhiệm đọc dữ liệu PCM thô từ `AudioCodec`. Sau đó, nó đưa dữ liệu này đến `WakeWord` engine hoặc `AudioProcessor` dựa trên trạng thái hiện tại.
2.  **`AudioOutputTask`**: Chịu trách nhiệm phát âm thanh. Nó truy xuất dữ liệu PCM đã giải mã từ `audio_playback_queue_` và gửi nó đến `AudioCodec` để phát trên loa.
3.  **`OpusCodecTask`**: Một tác vụ làm việc xử lý cả mã hóa và giải mã. Nó lấy âm thanh thô từ `audio_encode_queue_`, mã hóa nó thành các gói Opus, và đặt chúng vào `audio_send_queue_`. Đồng thời, nó lấy các gói Opus từ `audio_decode_queue_`, giải mã chúng thành PCM, và đặt kết quả vào `audio_playback_queue_`.

## Luồng Dữ liệu (Data Flow)

Có hai luồng dữ liệu chính: đầu vào âm thanh (uplink) và đầu ra âm thanh (downlink).

### 1. Luồng Đầu vào Âm thanh (Uplink)

Luồng này thu âm từ micro, xử lý nó, mã hóa nó và chuẩn bị nó để gửi đến máy chủ.

```mermaid
graph TD
    subgraph Device
        Mic[("Microphone")] -->|I2S| Codec(AudioCodec)
        
        subgraph AudioInputTask
            Codec -->|Raw PCM| Read(ReadAudioData)
            Read -->|16kHz PCM| Processor(AudioProcessor)
        end

        subgraph OpusCodecTask
            Processor -->|Clean PCM| EncodeQueue(audio_encode_queue_)
            EncodeQueue --> Encoder(OpusEncoder)
            Encoder -->|Opus Packet| SendQueue(audio_send_queue_)
        end

        SendQueue --> |"PopPacketFromSendQueue()"| App(Application Layer)
    end
    
    App -->|Network| Server((Cloud Server))
