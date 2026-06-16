# Custom Emoji Assets

This project can load a custom LVGL emoji collection from the `assets` partition.
For LCD boards such as `esps3-bao-dev`, emoji files are looked up by emotion name.

## Required File Names

Prepare a directory containing `.gif` or `.png` files named with these emotion keys:

```text
neutral.gif
happy.gif
laughing.gif
funny.gif
sad.gif
angry.gif
crying.gif
loving.gif
embarrassed.gif
surprised.gif
shocked.gif
thinking.gif
winking.gif
cool.gif
relaxed.gif
delicious.gif
kissy.gif
confident.gif
sleepy.gif
silly.gif
confused.gif
```

The file extension may be `.gif` or `.png`; keep the basename unchanged.
The asset builder does not resize images, so prepare the desired dimensions before packaging.
For `esps3-bao-dev`, `128x128` GIFs are a good default.

## Build A New Collection

From the repository root, stage the new files in the layout expected by `build_default_assets.py`:

```bash
rm -rf /tmp/xiaozhi-custom-fonts
mkdir -p /tmp/xiaozhi-custom-fonts/gif/my_emoji
cp /path/to/new_emoji/*.gif /tmp/xiaozhi-custom-fonts/gif/my_emoji/
```

If the collection uses PNG files:

```bash
rm -rf /tmp/xiaozhi-custom-fonts
mkdir -p /tmp/xiaozhi-custom-fonts/png/my_emoji
cp /path/to/new_emoji/*.png /tmp/xiaozhi-custom-fonts/png/my_emoji/
```

Package the custom asset image:

```bash
python scripts/build_default_assets.py \
  --sdkconfig sdkconfig \
  --emoji_collection my_emoji \
  --xiaozhi_fonts_path /tmp/xiaozhi-custom-fonts \
  --extra_files main/assets/common \
  --output main/assets/custom/my_emoji_assets.bin
```

`--extra_files main/assets/common` is required for `esps3-bao-dev` because the board loads its background binaries from the assets partition.

## Firmware Configuration

Ensure `sdkconfig` uses the tracked custom asset image:

```text
# CONFIG_FLASH_DEFAULT_ASSETS is not set
CONFIG_FLASH_CUSTOM_ASSETS=y
CONFIG_CUSTOM_ASSETS_FILE="assets/custom/my_emoji_assets.bin"
```

## Verify

Check that the asset image contains the expected index entries:

```bash
strings main/assets/custom/my_emoji_assets.bin | rg "emoji_collection|happy.gif|neutral.gif|thinking.gif|esps3_bao_dev_bg_light.bin|srmodels.bin"
```

Build the firmware:

```bash
idf.py build
```

The CMake output should include:

```text
Custom assets flash configured: .../main/assets/custom/my_emoji_assets.bin -> assets partition
```

The final flash command should include:

```text
0x800000 main/assets/custom/my_emoji_assets.bin
```

## Commit And Push

Both `sdkconfig` and generated build artifacts are ignored by default, so force-add only the intended files:

```bash
git add -f sdkconfig main/assets/custom/my_emoji_assets.bin
git add docs/custom-emoji-assets.md
git commit -m "Update custom emoji assets"
git push fork main:main
```
