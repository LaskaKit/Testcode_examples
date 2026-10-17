#!/bin/bash

# ESP-IDF Build Script for Linux/Mac
# This script helps build and flash the GDEH037E01 project

echo "========================================="
echo "GDEH037E01 EPD Display IDF Build Script"
echo "========================================="
echo ""

# Check if IDF_PATH is set
if [ -z "$IDF_PATH" ]; then
    echo "[ERROR] IDF_PATH environment variable not set!"
    echo "Please set IDF_PATH to your ESP-IDF installation directory."
    echo "Example: export IDF_PATH=~/esp/esp-idf"
    exit 1
fi

echo "[INFO] IDF_PATH: $IDF_PATH"
echo ""

# Parse command line arguments
if [ $# -eq 0 ]; then
    echo "Usage: ./build.sh [command] [options]"
    echo ""
    echo "Commands:"
    echo "  build         - Build the project"
    echo "  flash PORT    - Flash to device (e.g., ./build.sh flash /dev/ttyUSB0)"
    echo "  monitor PORT  - Monitor serial output"
    echo "  clean         - Clean build files"
    echo "  menuconfig    - Open configuration menu"
    echo "  buildflash PORT - Build and flash in one command"
    echo ""
    exit 1
fi

CMD=$1
PORT=$2

# Initialize IDF environment
source "$IDF_PATH/export.sh" > /dev/null 2>&1
if [ $? -ne 0 ]; then
    echo "[ERROR] Failed to initialize ESP-IDF environment!"
    exit 1
fi

echo "[INFO] Building project..."
echo ""

case "$CMD" in
    build)
        idf.py build
        if [ $? -eq 0 ]; then
            echo "[SUCCESS] Build completed!"
            echo ""
        else
            echo "[ERROR] Build failed!"
            exit 1
        fi
        ;;
    
    flash)
        if [ -z "$PORT" ]; then
            echo "[ERROR] Port not specified!"
            echo "Usage: ./build.sh flash /dev/ttyUSB0"
            exit 1
        fi
        idf.py -p "$PORT" flash
        if [ $? -eq 0 ]; then
            echo "[SUCCESS] Flash completed!"
            echo ""
        else
            echo "[ERROR] Flash failed!"
            exit 1
        fi
        ;;
    
    monitor)
        if [ -z "$PORT" ]; then
            echo "[ERROR] Port not specified!"
            echo "Usage: ./build.sh monitor /dev/ttyUSB0"
            exit 1
        fi
        idf.py -p "$PORT" monitor
        ;;
    
    clean)
        idf.py fullclean
        echo "[SUCCESS] Clean completed!"
        echo ""
        ;;
    
    menuconfig)
        idf.py menuconfig
        ;;
    
    buildflash)
        if [ -z "$PORT" ]; then
            echo "[ERROR] Port not specified!"
            echo "Usage: ./build.sh buildflash /dev/ttyUSB0"
            exit 1
        fi
        echo "[INFO] Building..."
        idf.py build
        if [ $? -ne 0 ]; then
            echo "[ERROR] Build failed!"
            exit 1
        fi
        echo "[INFO] Flashing..."
        idf.py -p "$PORT" flash
        if [ $? -eq 0 ]; then
            echo "[SUCCESS] Build and flash completed!"
            echo ""
        else
            echo "[ERROR] Flash failed!"
            exit 1
        fi
        ;;
    
    *)
        echo "[ERROR] Unknown command: $CMD"
        echo "Use './build.sh' for help"
        exit 1
        ;;
esac
