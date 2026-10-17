# ESP-IDF 工程快速开始指南

## 工程结构已为你创建完成

```
GDEH037E01/
├── CMakeLists.txt                 # 根项目配置
├── main/                          # 主程序目录
│   ├── CMakeLists.txt            # 组件配置
│   ├── GDEH037E01_idf.c          # 主程序代码
│   └── image.h                   # 图像数据头文件
├── build.bat                      # Windows 编译脚本
├── build.sh                       # Linux/Mac 编译脚本
├── README.md                      # 详细文档
├── QUICK_START.md                 # 本文件
└── .gitignore                     # Git 忽略配置
```

## 前置要求

1. **安装 ESP-IDF**
   ```bash
   # Windows 或 Linux/Mac 都可以从以下位置获取
   https://github.com/espressif/esp-idf
   ```

2. **设置环境变量**
   ```bash
   # Windows (PowerShell)
   $env:IDF_PATH = "C:\esp\esp-idf"
   
   # Linux/Mac
   export IDF_PATH=~/esp/esp-idf
   ```

3. **安装 Python 依赖**
   ```bash
   python -m pip install --upgrade pip
   pip install -r $IDF_PATH/requirements.txt
   ```

## 快速开始步骤

### 第 1 步：选择你的开发板

编辑 `main/GDEH037E01_idf.c` 第 13 行：

**ESP32-S3:**
```c
#define BOARD_ESP32S3 1
```

**ESP32:**
```c
#define BOARD_ESP32S3 0
```

### 第 2 步：准备镜像数据

你的 `image.h` 中需要有 `image_data_sixcolor` 数组。

如果已经有了 Arduino 版本的 `image.h`，直接复制到 `main/` 目录即可。

### 第 3 步：编译项目

**Windows:**
```bash
.\build.bat build
```

**Linux/Mac:**
```bash
chmod +x build.sh
./build.sh build
```

### 第 4 步：烧写到设备

先找到你的串口号：
- **Windows**: COM3, COM4 等
- **Linux**: /dev/ttyUSB0, /dev/ttyACM0 等
- **Mac**: /dev/tty.usbserial-* 等

**Windows:**
```bash
.\build.bat flash COM3
```

**Linux/Mac:**
```bash
./build.sh flash /dev/ttyUSB0
```

### 第 5 步：监视输出

**Windows:**
```bash
.\build.bat monitor COM3
```

**Linux/Mac:**
```bash
./build.sh monitor /dev/ttyUSB0
```

## 或者一条命令搞定

**Windows:**
```bash
.\build.bat buildflash COM3
```

**Linux/Mac:**
```bash
./build.sh buildflash /dev/ttyUSB0
```

## 常见问题

### Q: 编译时找不到 image.h
A: 确保你有 `main/image.h` 文件，其中包含 `image_data_sixcolor` 数组定义。

### Q: BUSY 引脚没反应
程序会自动检测 BUSY 极性。如果还是不行：
1. 检查硬件接线
2. 查看串口日志中的极性检测信息
3. 尝试手动改变 pin 定义

### Q: 找不到串口
```bash
# 列出可用的串口
python -m serial.tools.list_ports
```

### Q: IDF_PATH 未设置
```bash
# 确保已正确设置环境变量
echo $IDF_PATH  (Linux/Mac)
echo %IDF_PATH% (Windows)
```

## 文件说明

| 文件 | 说明 |
|------|------|
| `main/GDEH037E01_idf.c` | 主程序，包含所有 EPD 驱动代码 |
| `main/image.h` | 图像数据定义 |
| `main/CMakeLists.txt` | 组件构建配置 |
| `CMakeLists.txt` | 项目构建配置 |
| `build.bat` | Windows 一键编译脚本 |
| `build.sh` | Linux/Mac 一键编译脚本 |
| `README.md` | 详细文档 |

## 后续开发

### 修改 pin 脚
编辑 `main/GDEH037E01_idf.c` 中的 `init_pins()` 函数。

### 修改 SPI 速度
在 `epd_hardware_init()` 函数中修改：
```c
.clock_speed_hz = 2000000,  // 改为你想要的频率
```

### 修改显示内容
在 `epd_task()` 函数中修改：
```c
epd_display_image(image_data_sixcolor, sizeof(image_data_sixcolor));
```

## 获得帮助

1. 查看 `README.md` 获取详细说明
2. 查看 `main/GDEH037E01_idf.c` 中的注释
3. 参考 ESP-IDF 官方文档: https://docs.espressif.com/

---

祝你开发愉快！🎉
