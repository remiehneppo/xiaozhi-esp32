# ESP-S3-BAO

ESP32-S3 开发板，基于全动 ESP32-S3 开发板（quandong-s3-dev）移植，支持 LVGL 自定义背景图片。

## 硬件规格

| 项目 | 参数 |
|---|---|
| SoC | ESP32-S3 |
| 屏幕 | ILI9341 240×320 SPI |
| 音频 | ES8311 + PCA9557 |
| 按键 | BOOT (GPIO0) |
| 背光 | GPIO45 |
| I2C SDA | GPIO16 |
| I2C SCL | GPIO15 |

## 构建

```bash
idf.py set-target esp32s3
idf.py menuconfig → Component config → XiaoZhi → BOARD_TYPE → ESP-S3-BAO
idf.py build
```

## 自定义背景图片

本板支持通过 LVGL 背景图片自定义界面外观（同时适用于亮色/暗色主题）。

### 1. 准备图片

- 尺寸：**240×320**（竖屏）
- 格式：**PNG**（推荐）

### 2. 安装转换工具依赖

```bash
pip3 install pypng lz4
```

### 3. 转换图片

```bash
python3 scripts/Image_Converter/LVGLImage.py \
    -i your_background.png \
    -o esps3_bao_dev_bg.bin \
    -f RGB565
```

### 4. 放置文件

将生成的 `esps3_bao_dev_bg.bin` 复制到：

```
main/assets/common/esps3_bao_dev_bg.bin
```

### 5. 重新构建

```bash
idf.py build && idf.py flash
```

### 回退到纯色背景

删除 `main/assets/common/esps3_bao_dev_bg.bin` 即可自动回退到纯色背景（亮色 `#FFFFFF` / 暗色 `#000000`）。

### 修改纯色背景颜色

编辑 `main/display/lcd_display.cc` 中 `InitializeLcdThemes()` 函数：

```cpp
// 亮色主题
light_theme->set_background_color(lv_color_hex(0xFFFFFF));

// 暗色主题
dark_theme->set_background_color(lv_color_hex(0x000000));
```
