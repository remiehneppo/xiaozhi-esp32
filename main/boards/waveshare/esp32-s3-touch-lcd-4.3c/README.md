# Hướng dẫn sử dụng 

* [ESP32-S3-Touch-LCD-4.3C docs](https://www.waveshare.com/esp32-s3-touch-lcd-4.3c.htm)

## Trải nghiệm nhanh

Tải xuống [firmware] đã biên dịch: [https://files.waveshare.com/wiki/ESP32-S3-Touch-LCD-4.3C/ESP32-S3-Touch-LCD-4.3C-Xiaozhi.bin](https://files.waveshare.com/wiki/ESP32-S3-Touch-LCD-4.3C/ESP32-S3-Touch-LCD-4.3C-Xiaozhi.bin) 

```shell
esptool.py --chip esp32s3 -p /dev/ttyACM0 -b 460800 --before=default_reset --after=hard_reset write_flash --flash_mode dio --flash_freq 80m --flash_size 16MB 0x00 ESP32-S3-Touch-LCD-4.3C-Xiaozhi.bin 
