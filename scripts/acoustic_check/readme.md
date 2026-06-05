# Kiểm tra sóng âm
GUI này được sử dụng để kiểm tra việc chuyển đổi PCM được truyền ngược lại qua `udp` từ thiết bị Xiaozhi sang miền thời gian/miền tần số, có thể lưu trữ âm thanh theo độ dài cửa sổ, dùng để xác định phân bố tần số nhiễu và kiểm tra độ chính xác của truyền tải ASCII sóng âm,

Để kiểm tra firmware, cần bật `USE_AUDIO_DEBUGGER` và thiết lập `AUDIO_DEBUG_UDP_SERVER` là địa chỉ máy cục bộ.
Kiểm tra sóng âm `demod` có thể được thực hiện bằng cách sử dụng `sonic_wifi_config.html` hoặc tải lên [Cấu hình mạng sóng âm Xiaozhi](https://iqf7jnhi.pinit.eth.limo) trên `PinMe`

# Ghi chép kiểm tra giải mã sóng âm

> `✓` đại diện cho việc giải mã thành công khi nhận tín hiệu PCM thô qua I2S DIN, `△` đại diện cho việc cần khử nhiễu hoặc thao tác bổ sung để giải mã ổn định, `X` đại diện cho hiệu quả không tốt ngay cả sau khi khử nhiễu (có thể giải mã một phần nhưng rất không ổn định).
> Một số ADC cần điều chỉnh khử nhiễu tinh vi hơn trong giai đoạn cấu hình I2C, do thiết bị không phổ thông nên tạm thời chỉ kiểm tra theo config được cung cấp trong boards

| Thiết bị | ADC | MIC | Hiệu quả | Ghi chú |
| ---- | ---- | --- | --- | ---- |
| bread-compact | INMP441 | MEMEMIC tích hợp | ✓ |
| atk-dnesp32s3-box | ES8311 | | ✓ |
| magiclick-2p5 | ES8311 | | ✓ |
| lichuang-dev | ES7210 | | △ | Khi kiểm tra cần tắt INPUT_REFERENCE
| kevin-box-2 | ES7210 | | △ | Khi kiểm tra cần tắt INPUT_REFERENCE
| m5stack-core-s3 | ES7210 | | △ | Khi kiểm tra cần tắt INPUT_REFERENCE
| xmini-c3 | ES8311 | | △ | Cần khử nhiễu
| atoms3r-echo-base | ES8311 | | △ | Cần khử nhiễu
| atk-dnesp32s3-box0 | ES8311 | | X | Có thể nhận và giải mã, nhưng tỉ lệ mất gói rất cao
| movecall-moji-esp32s3 | ES8311 | | X | Có thể nhận và giải mã, nhưng tỉ lệ mất gói rất cao
