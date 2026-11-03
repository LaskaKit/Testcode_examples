# GDEB0709E01 ESP32-S3 双环境驱动示例

本程序包专门针对 **GDEB0709E01 7.09 英寸 Spectra 6 六色电子纸**。

- 分辨率：1200 × 1600
- 接口：SPI
- 像素格式：4 bit/pixel，1 byte 打包 2 个像素
- 面板由两个控制器分别驱动左右 600 列
- 单幅图像数据：1200 × 1600 / 2 = 960000 bytes
- 已取消 Wi-Fi、Web Server、DNS、图片上传、轮播等功能。
- 示例直接使用 `image.h` 中的 `gImage[]`。
- 驱动代码中不再保留其他尺寸屏幕的旧注释、旧函数名或旧示例代码。

## GPIO

| 信号 | GPIO |
|---|---:|
| SCK | 9 |
| MOSI | 41 |
| MISO | 40 |
| CS Master | 18 |
| CS Slave | 17 |
| RST | 6 |
| DC | 2 |
| BUSY | 7 |
| PWR | 45 |

## Arduino IDE

打开：

`Arduino_IDE/GDEB0709E01_Demo/GDEB0709E01_Demo.ino`

选择 ESP32-S3 对应开发板后编译上传。

Arduino 版本使用 ESP32 Arduino SPI 驱动进行批量 SPI 传输；大块数据传输由底层 SPI 驱动使用 DMA。

## ESP-IDF

进入：

`ESP-IDF/gdeb0709e01_demo`

使用 ESP-IDF 5.x：

```text
idf.py set-target esp32s3
idf.py build
idf.py flash monitor
```

ESP-IDF 版本直接使用 `spi_master`，SPI 总线配置启用 DMA。

## Demo 行为

默认：

```text
上电
 -> 初始化
 -> 显示 image.h 中的 gImage
 -> 刷新完成
 -> 进入 Sleep
 -> 图片保持在电子纸上
```

如果需要测试“显示图片后再刷白”，把 Arduino 示例中的：

```cpp
#define CLEAR_TO_WHITE_AFTER_IMAGE 0
```

改成：

```cpp
#define CLEAR_TO_WHITE_AFTER_IMAGE 1
```

然后重新编译即可。

## 注意

`image.h` 必须保持当前 960000 字节的数据格式，不需要在 MCU 端重新抖动或转换。

本示例不包含任何网络功能，客户拿到程序后只需要关注：
- `GDEB0709E01.h`
- `GDEB0709E01.cpp`
- `DEV_Config.h/.cpp`
- `image.h`

以及 ESP-IDF 下对应的 `main/` 文件。
