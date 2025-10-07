// Konfigurace pro LaskaKit ESPD-3,5"
// Nakopirovat do slozky Arduino\libraries\TFT_eSPI\User_Setups
// Do souboru Arduino\libraries\TFT_eSPI\User_Setup_Select.h pridat radek: #include <User_Setups/Setup303_ILI9488_ESPD-3_5_v3.h> do sekce #ifndef USER_SETUP_LOADED

#define USER_SETUP_ID 304
#define USER_SETUP_INFO "LaskaKit ESP32-S3-DevKit with 2.25 76x284, ST7789"

// Display:
#define ST7789_DRIVER
#define TFT_WIDTH  76
#define TFT_HEIGHT 284
#define TFT_BACKLIGHT_ON LOW    // Level to turn ON back-light (HIGH or LOW)
#define PW_ON       47          // Turn µŠup ON
#define TFT_BL      42          // LED back-light control pin (used Blue wire, SDA pin on I2C µŠup)
#define TFT_MISO    -1          // MISO pin (not used)
#define TFT_MOSI    11          // MOSI pin (used White wire on SPI µŠup)
#define TFT_SCLK    12          // Clock pin (used Yellow wire on SPI µŠup)
#define TFT_CS      2           // Chip select control pin (used Yellow wire, SCL pin on I2C µŠup)
#define TFT_DC      10          // Data Command control pin (used Green wire on SPI µŠup)
#define TFT_RST     13          // RST is connectred to EN pin ESP32 (used Blue wire on SPI µŠup)

// I2C (µŠup and devices): used for display
//#define I2C_SDA         42      // Data pin 
//#define I2C_SCL         2       // Clock pin
// Battery mesurement:
#define BAT_PIN         9       // Battery voltage mesurement
#define deviderRatio 1.7693877551   // Voltage devider ratio on ADC pin 1MOhm + 1.3MOhm


#define LOAD_GLCD   // Font 1. Original Adafruit 8 pixel font needs ~1820 bytes in FLASH
#define LOAD_FONT2  // Font 2. Small 16 pixel high font, needs ~3534 bytes in FLASH, 96 characters
#define LOAD_FONT4  // Font 4. Medium 26 pixel high font, needs ~5848 bytes in FLASH, 96 characters
#define LOAD_FONT6  // Font 6. Large 48 pixel font, needs ~2666 bytes in FLASH, only characters 1234567890:-.apm
#define LOAD_FONT7  // Font 7. 7 segment 48 pixel font, needs ~2438 bytes in FLASH, only characters 1234567890:.
#define LOAD_FONT8  // Font 8. Large 75 pixel font needs ~3256 bytes in FLASH, only characters 1234567890:-.
#define LOAD_GFXFF  // FreeFonts. Include access to the 48 Adafruit_GFX free fonts FF1 to FF48 and custom fonts
#define SMOOTH_FONT

#define SPI_FREQUENCY  20000000 //
//#define SPI_FREQUENCY  79000000 // 79MHz is max. 80MHz has artefacts
#define SPI_READ_FREQUENCY  20000000
#define SPI_TOUCH_FREQUENCY  2500000
#define USE_HSPI_PORT
// Povolit CGRAM offset (jinak bílý pruh!)
#define CGRAM_OFFSET