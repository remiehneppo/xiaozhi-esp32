# Lệnh biên dịch

## Biên dịch một lệnh

```bash
python scripts/release.py sensecap-watcher
```

## Cấu hình biên dịch thủ công

```bash
idf.py set-target esp32s3
```

**Cấu hình**

```bash
idf.py menuconfig
```

Chọn bo mạch

```
Xiaozhi Assistant -> Board Type -> SenseCAP Watcher
```

Một số tùy chọn bổ sung cho Watcher như sau, cần chọn trong `menuconfig`.

```
CONFIG_BOARD_TYPE_SEEED_STUDIO_SENSECAP_WATCHER=y
CONFIG_ESPTOOLPY_FLASHSIZE_32MB=y
CONFIG_PARTITION_TABLE_CUSTOM_FILENAME="partitions/v2/32m.csv"
CONFIG_BOOTLOADER_CACHE_32BIT_ADDR_QUAD_FLASH=y
CONFIG_ESPTOOLPY_FLASH_MODE_AUTO_DETECT=n
CONFIG_IDF_EXPERIMENTAL_FEATURES=y
```

## Biên dịch và nạp

```bash
idf.py -DBOARD_NAME=sensecap-watcher build flash
```

Lưu ý: nếu thiết bị hiện tại trước đây được xuất xưởng với firmware SenseCAP (không phải bản Xiaozhi), hãy đặc biệt cẩn thận khi xử lý địa chỉ phân vùng firmware trong flash để tránh xóa nhầm thông tin riêng của thiết bị SenseCAP Watcher (EUI, v.v.). Nếu không, dù khôi phục lại firmware SenseCAP thì thiết bị vẫn không thể kết nối đúng tới máy chủ SenseCraft! Vì vậy, trước khi nạp firmware, hãy ghi lại các thông tin cần thiết của thiết bị để đảm bảo có cách khôi phục.

Bạn có thể dùng lệnh sau để sao lưu thông tin sản xuất

```bash
# firstly backup the factory information partition which contains the credentials for connecting the SenseCraft server
esptool.py --chip esp32s3 --baud 2000000 --before default_reset --after hard_reset --no-stub read_flash 0x9000 204800 nvsfactory.bin

```
