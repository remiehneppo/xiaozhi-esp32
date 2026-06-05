# 小智云聊 S3

## Giới thiệu

XiaoZhi Yunliao S3 là một biến thể tùy biến của XiaoZhi AI, là sản phẩm thương mại đầu tiên có màn hình lớn 2.8 inch bảo vệ mắt, chữ lớn và pin 2000mAh, với nhiều cải tiến và tối ưu.

## Bản chính thức

Mã nguồn bản chính thức được duy trì trong dự án XiaoZhi AI chính, cập nhật theo phiên bản của dự án gốc, thuận tiện cho việc mở rộng và dùng firmware bên thứ ba. Hỗ trợ đánh thức bằng giọng nói, ngắt lời bằng giọng nói, OTA và chuyển đổi linh hoạt 4G.

> ### Thao tác nút
>
> - **Bật máy**: ở trạng thái tắt, giữ nút 1 giây rồi thả ra, máy sẽ tự bật.
> - **Tắt máy**: ở trạng thái bật, giữ nút 1 giây rồi thả ra, thanh tiêu đề sẽ hiện "Vui lòng chờ", đợi thêm 2 giây máy sẽ tự tắt.
> - **Đánh thức/ngắt lời**: khi đang trong cuộc trò chuyện bình thường, nhấn một lần.
> - **Chuyển 4G/Wi-Fi**: trong lúc khởi động hoặc ở màn hình cấu hình mạng, nhấn đúp trong vòng 1 giây (cần có module 4G).
> - **Chuyển chế độ ngắt lời giọng nói (AEC)**: sau khi khởi động bình thường, khi đang ở trạng thái rảnh không trò chuyện, nhấn đúp trong vòng 1 giây để luân phiên đổi chế độ ngắt lời.
> - **Cấu hình mạng lại**: khi máy đang bật, nhấn ba lần trong vòng 1 giây, máy sẽ tự khởi động lại và vào màn hình cấu hình mạng.
>
> ### Lệnh giọng nói
>
> - **Bật/tắt chế độ ngắt lời giọng nói (AEC)**: khi đang phát nhạc, cần tắt chế độ này để tránh làm gián đoạn nhạc.
> - **Chuyển chế độ hiển thị màn IPS**: bản XiaoZhi Yunliao S3 mới đã nâng cấp màn IPS, cần đổi chế độ hiển thị thì mới hiển thị đúng; có thể chuyển qua lại.

## Bản tùy biến

Bản tùy biến có thay đổi nền tảng khá lớn nên được duy trì code riêng và định kỳ merge từ dự án chính.

> ### Vì sao gọi là tùy biến
>
> - Là bản đầu tiên hỗ trợ cấu hình Wi-Fi bằng mã QR WeChat.
> - Là bản đầu tiên hỗ trợ cấu hình bằng một điện thoại đơn.
> - Là bản đầu tiên hỗ trợ quét QR để vào console.
> - Là bản đầu tiên phát hành giao diện tiếng Phồn thể, Nhật và Anh.
> - Là bản đầu tiên hỗ trợ chế độ điều khiển hoàn toàn bằng giọng nói.
> - Cung cấp độc quyền nhiều cách nạp firmware, gồm script nhấp một lần.

## Khác biệt phiên bản

> | Tính năng       | Bản chính thức | Bản tùy biến |
> | -------------- | -------------- | ------------ |
> | Ngắt lời bằng giọng nói | ✓ | ✓ |
> | 4G | ✓ | ✓ |
> | Tự động cập nhật firmware | ✓ | X |
> | Hỗ trợ firmware bên thứ ba | ✓ | X |
> | Màn hình chờ thời tiết | X | ✓ |
> | Nhắc báo thức | X | ✓ |
> | Phát nhạc mạng | X | ✓ |
> | Cấu hình Wi-Fi bằng QR WeChat | X | ✓ |
> | Cấu hình bằng một điện thoại | X | ✓ |
> | Quét mã để vào console | X | ✓ |
> | Giao diện Phồn/ Nhật/ Anh | X | ✓ |
> | Hỗ trợ đa ngôn ngữ | Cần tự biên dịch | ✓ |
> | Loa/tai nghe Bluetooth ngoài | ✓ | ✓ |

# Lệnh build

**Clone dự án**

```bash
git clone https://github.com/78/xiaozhi-esp32.git
```

**Vào thư mục dự án**

```bash
cd xiaozhi-esp32
```

**Chọn target build ESP32S3**

```bash
idf.py set-target esp32s3
```

**Mở menuconfig**

```bash
idf.py menuconfig
```

**Chọn board**

```bash
- `Xiaozhi Assistant` → `Board Type` → chọn `XiaoZhi Yunliao S3` → chọn `Enable Device-Side AEC`
```

**Build**

```bash
idf.py build
```

**Nạp và mở terminal serial**

```bash
idf.py build flash monitor
```
