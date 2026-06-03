# ESP32 Nhan

Custom ESP32-S3 board with a GC9A01 240x240 SPI round display and discrete I2S speaker/microphone modules.

## Pinout

- Display GC9A01: RST GPIO8, CS GPIO10, DC GPIO9, SDA/MOSI GPIO11, SCL/SCLK GPIO12
- Speaker MAX98357A: DIN GPIO14, BCLK GPIO37, LRC GPIO36
- Microphone INMP441: SD GPIO39, SCK GPIO37, WS GPIO36
- Button: GPIO41

## Build note

This board revision routes I2S WS/BCLK through GPIO36/GPIO37. On ESP32-S3 modules with Octal PSRAM, these pins overlap the MSPI/PSRAM pin group. Leaving PSRAM enabled causes a watchdog reset while initializing the I2S RX channel.

Build this board with PSRAM disabled:

```bash
idf.py -DSDKCONFIG_DEFAULTS="sdkconfig.defaults;sdkconfig.defaults.esp32s3;main/boards/esp32-nhan/sdkconfig.defaults" set-target esp32s3 build
```

Wake word AFE/audio processor is also disabled in the board defaults because it depends on PSRAM-sized memory headroom.
