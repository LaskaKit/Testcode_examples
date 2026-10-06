# GDEB0709E01 ESP32-S3 Dual Environment Demo

本程序包从原始工程中整理出纯显示版本，目标屏为 GDEB0709E01。

## 目标

- Arduino IDE
- ESP-IDF
- ESP32-S3
- 直接显示 `image.h` 中的 `gImage[]`
- 无 Wi-Fi
- 无 Web Server
- 无 DNS
- 无图片上传
- 无轮播
- 无网络配置
- 保留原有面板初始化参数和双控制器 SPI 刷图结构
- 提供白屏清屏接口

## 屏幕数据

GDEB0709E01：
- 1200 × 1600
- 6 色
- 4 bit/pixel
- 每行 600 bytes
- Master 左侧 600 列：300 bytes
- Slave 右侧 600 列：300 bytes
- 总图像：960000 bytes

## 目录

```text
Arduino_IDE/
  GDEB0709E01_Demo/
    GDEB0709E01_Demo.ino
    GDEB0709E01.cpp
    GDEB0709E01.h
    DEV_Config.cpp
    DEV_Config.h
    image.h

ESP-IDF/
  gdeb0709e01_demo/
    CMakeLists.txt
    sdkconfig.defaults
    main/
      CMakeLists.txt
      main.c
      GDEB0709E01.c
      GDEB0709E01.h
      image.h
```

默认程序行为：初始化 → 显示 `image.h` → 刷新完成 → Sleep，图片保持显示。

如果需要测试白屏，把示例中的 `CLEAR_TO_WHITE_AFTER_IMAGE` 改为 `1`。
