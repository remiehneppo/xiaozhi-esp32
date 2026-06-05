# Mixgo_Nova (Yunkong Youth)

<img src="https://mixly.cn/public/icon/2024/6/09705006c1c643beb96338791ee1dea0_m.png" alt="Mixgo_Nova" width="200"/>

&zwnj;**[Mixgo_Nova](https://mixly.cn/fredqian/mixgo_nova)**&zwnj; là một board đa năng được thiết kế cho IoT, giáo dục và các dự án maker, tích hợp nhiều cảm biến và module truyền thông không dây, hỗ trợ lập trình kéo-thả (Mixly) và tương tác giọng nói offline, phù hợp cho tạo mẫu nhanh và giảng dạy.

---

## 🛠️ Lệnh cấu hình build

**Vấn đề thu âm ES8374 CODE MIC:**

```
managed_components\espressif__esp_codec_dev\device\es8374

static int es8374_config_adc_input(audio_codec_es8374_t *codec, es_adc_input_t input)
{
    int ret = 0;
    int reg = 0;
    ret |= es8374_read_reg(codec, 0x21, &reg);
    if (ret == 0) {
        reg = (reg & 0xcf) | 0x24;
        ret |= es8374_write_reg(codec, 0x21, reg);
    }
    return ret;
}

PS: L386 `reg = (reg & 0xcf) | 0x14;` đổi thành `reg = (reg & 0xcf) | 0x24;`
```

**Đặt target build là ESP32S3:**

```bash
idf.py set-target esp32s3
```

**Mở menuconfig:**

```bash
idf.py menuconfig
```

**Chọn board:**

```
Xiaozhi Assistant -> Board Type -> Yunkong Youth
```

**Đổi cấu hình PSRAM:**

```
Component config -> ESP PSRAM -> SPI RAM config -> Mode (QUAD/OCT) -> QUAD Mode PSRAM
```

**Đổi cấu hình Flash:**

```
Serial flasher config -> Flash size -> 8 MB
Partition Table -> Custom partition CSV file -> partitions/v2/8m.csv
```

**Build:**

```bash
idf.py build
```

**Gộp BIN:**

```bash
idf.py merge-bin -o xiaozhi-nova.bin -f raw
```
