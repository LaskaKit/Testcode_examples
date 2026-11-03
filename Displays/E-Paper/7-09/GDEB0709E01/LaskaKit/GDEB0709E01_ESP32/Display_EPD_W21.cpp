#include "Display_EPD_W21_spi.h"
#include "Display_EPD_W21.h"
#include <SPI.h>
#include <string.h>

// ---------------------------------------------------------------
// GDEB0709E01 (7.09")
// Register values from Good Display GDEP133C02 ESP32 example (2026-04-20),
// except AN_TM (0x74), which comes from the original GDEB0709E01 code.
// With the GD 13.3" AN_TM value (C0 1E 1E CE CE CE ...) green on the 7.09"
// panel turns olive in rows that contain several colours.
// ---------------------------------------------------------------
#define PSR             0x00
#define PWR             0x01
#define POF             0x02
#define PON             0x04
#define BTST_N          0x05
#define BTST_P          0x06
#define DTM             0x10
#define DRF             0x12
#define CDI             0x50
#define TCON            0x60
#define TRES            0x61
#define AN_TM           0x74
#define AGID            0x86
#define BUCK_BOOST_VDDN 0xB0
#define TFT_VCOM_POWER  0xB1
#define EN_BUF          0xB6
#define BOOST_VDDP_EN   0xB7
#define CCSET           0xE0
#define PWS             0xE3
#define CMD66           0xF0



static const unsigned char PSR_V[2]             = {0xDF, 0x69};
static const unsigned char PWR_V[6]             = {0x0F, 0x00, 0x28, 0x2C, 0x28, 0x38};
static const unsigned char POF_V[1]             = {0x00};
static const unsigned char DRF_V[1]             = {0x01};
static const unsigned char CDI_V[1]             = {0xF7};
static const unsigned char TCON_V[2]            = {0x03, 0x03};
static const unsigned char TRES_V[4]            = {0x04, 0xB0, 0x03, 0x20};
static const unsigned char CMD66_V[6]           = {0x49, 0x55, 0x13, 0x5D, 0x05, 0x10};
static const unsigned char EN_BUF_V[1]          = {0x07};
static const unsigned char CCSET_V[1]           = {0x01};
static const unsigned char PWS_V[1]             = {0x22};
static const unsigned char AN_TM_V[9]           = {0x00, 0x0C, 0x0C, 0xD9, 0xDD, 0xDD, 0x15, 0x15, 0x55};
static const unsigned char AGID_V[1]            = {0x10};
static const unsigned char BTST_P_V[2]          = {0xE8, 0x28};
static const unsigned char BTST_N_V[2]          = {0xE8, 0x28};
static const unsigned char BOOST_VDDP_EN_V[1]   = {0x01};
static const unsigned char BUCK_BOOST_VDDN_V[1] = {0x01};
static const unsigned char TFT_VCOM_POWER_V[1]  = {0x02};

// register to master only (csx = 0)
static void writeMaster(unsigned char cmd, const unsigned char *data, unsigned int len)
{
  EPD_CS(0, LOW);
  EPD_W21_WriteREG(cmd, data, len);
  EPD_CS_All(HIGH);
}

// register to both driver ICs at once
static void writeBoth(unsigned char cmd, const unsigned char *data, unsigned int len)
{
  EPD_CS_All(LOW);
  EPD_W21_WriteREG(cmd, data, len);
  EPD_CS_All(HIGH);
}

void lcd_chkstatus(void)
{
  // BUSY = LOW -> driver IC is busy
  unsigned long t0 = millis();
  while (!isEPD_W21_BUSY) {
    delay(1);
    if (millis() - t0 > 120000) {          // 2 min safety timeout
      Serial.println("BUSY timeout!");
      break;
    }
  }
}

static void EPD_hardware_reset(void)
{
  EPD_W21_RST_0;
  delay(20);
  EPD_W21_RST_1;
  delay(20);
}

void EPD_init(void)
{
  digitalWrite(POWER, HIGH);   // panel power on (may have been switched off by EPD_sleep)
  delay(50);

  EPD_hardware_reset();
  lcd_chkstatus();

  writeMaster(AN_TM, AN_TM_V, sizeof(AN_TM_V));
  writeBoth(CMD66, CMD66_V, sizeof(CMD66_V));
  writeBoth(PSR,   PSR_V,   sizeof(PSR_V));
  writeBoth(CDI,   CDI_V,   sizeof(CDI_V));
  writeBoth(TCON,  TCON_V,  sizeof(TCON_V));
  writeBoth(AGID,  AGID_V,  sizeof(AGID_V));
  writeBoth(PWS,   PWS_V,   sizeof(PWS_V));
  writeBoth(CCSET, CCSET_V, sizeof(CCSET_V));
  writeBoth(TRES,  TRES_V,  sizeof(TRES_V));

  writeMaster(PWR,            PWR_V,             sizeof(PWR_V));
  writeMaster(EN_BUF,          EN_BUF_V,          sizeof(EN_BUF_V));
  writeMaster(BTST_P,          BTST_P_V,          sizeof(BTST_P_V));
  writeMaster(BOOST_VDDP_EN,   BOOST_VDDP_EN_V,   sizeof(BOOST_VDDP_EN_V));
  writeMaster(BTST_N,          BTST_N_V,          sizeof(BTST_N_V));
  writeMaster(BUCK_BOOST_VDDN, BUCK_BOOST_VDDN_V, sizeof(BUCK_BOOST_VDDN_V));
  writeMaster(TFT_VCOM_POWER,  TFT_VCOM_POWER_V,  sizeof(TFT_VCOM_POWER_V));

  Serial.println("EPD_init done");
}

// read register 0xF2 from both driver ICs, bit0 = ready (needs EPD_MISO)
void EPD_check_driver_ICs(void)
{
#if EPD_MISO >= 0
  for (unsigned char csx = 0; csx < 2; csx++) {
    unsigned char buf[3];
    EPD_CS_All(HIGH);
    EPD_CS(csx, LOW);
    EPD_W21_WriteCMD(0xF2);
    for (int i = 0; i < 3; i++) buf[i] = SPI.transfer(0x00);
    EPD_CS_All(HIGH);
    Serial.printf("Driver IC [%d] = 0x%02X 0x%02X 0x%02X -> %s\n", csx, buf[0], buf[1], buf[2],
                  (buf[0] & 0x01) ? "ready" : "NO REPLY");
  }
#endif
}

// PON -> DRF -> POF, CS of both ICs held LOW while waiting (as in GD example)
void EPD_refresh(void)
{
  unsigned long t0 = millis();

  EPD_CS_All(LOW);
  EPD_W21_WriteCMD(PON);
  lcd_chkstatus();
  EPD_CS_All(HIGH);

  EPD_CS_All(LOW);
  delay(30);
  EPD_W21_WriteREG(DRF, DRF_V, sizeof(DRF_V));
  lcd_chkstatus();
  EPD_CS_All(HIGH);

  EPD_CS_All(LOW);
  EPD_W21_WriteREG(POF, POF_V, sizeof(POF_V));
  lcd_chkstatus();
  EPD_CS_All(HIGH);

  Serial.printf("Refresh done in %lu ms\n", millis() - t0);
}

// GD example has no deep-sleep command, panel power is switched off instead
void EPD_sleep(void)
{
  EPD_CS_All(HIGH);
  digitalWrite(POWER, LOW);
}

// Full image 1200x1600, 4 bit/px, row = 600 bytes:
// left 300 bytes of each row -> master, right 300 bytes -> slave
void PIC_display(const unsigned char *picData)
{
  EPD_CS_All(HIGH);
  EPD_CS(0, LOW);
  EPD_W21_WriteCMD(DTM);
  for (unsigned int i = 0; i < EPD_HEIGHT; i++)
    EPD_W21_WriteDATA_n(picData + i * EPD_ROW_BYTES, EPD_HALF_BYTES);
  EPD_CS_All(HIGH);

  EPD_CS(1, LOW);
  EPD_W21_WriteCMD(DTM);
  for (unsigned int i = 0; i < EPD_HEIGHT; i++)
    EPD_W21_WriteDATA_n(picData + i * EPD_ROW_BYTES + EPD_HALF_BYTES, EPD_HALF_BYTES);
  EPD_CS_All(HIGH);

  EPD_refresh();
}

// whole screen in one colour, data sent to both ICs at once
void Display_All(unsigned char color)
{
  static unsigned char buf[EPD_HALF_BYTES];
  memset(buf, (color << 4) | color, sizeof(buf));

  EPD_CS_All(LOW);
  EPD_W21_WriteCMD(DTM);
  for (unsigned int i = 0; i < EPD_HEIGHT; i++)
    EPD_W21_WriteDATA_n(buf, EPD_HALF_BYTES);
  EPD_CS_All(HIGH);

  EPD_refresh();
}

void Display_All_White(void)
{
  Display_All(white);
}

// 6 horizontal bars (black, white, yellow, red, blue, green) as in GD epdDisplayColorBar()
void Display_Color_Bar(void)
{
  static const unsigned char colors[6] = {black, white, yellow, red, blue, green};
  static unsigned char buf[EPD_HALF_BYTES];
  const unsigned long barBytes = 80000;   // per IC, 6 x 80000 = 480000 = 1600 rows x 300 bytes

  EPD_CS_All(LOW);
  EPD_W21_WriteCMD(DTM);
  for (int c = 0; c < 6; c++) {
    memset(buf, (colors[c] << 4) | colors[c], sizeof(buf));
    unsigned long left = barBytes;
    while (left) {
      unsigned long n = (left > sizeof(buf)) ? sizeof(buf) : left;
      EPD_W21_WriteDATA_n(buf, n);
      left -= n;
    }
  }
  EPD_CS_All(HIGH);

  EPD_refresh();
}

// 6x8 checkerboard of all colours as in GD draw_checkerboard(),
// generated row by row (no 960 kB buffer / PSRAM needed)
static void checkerRow(unsigned int y, unsigned char *row)
{
  static const unsigned char colors[6] = {black, white, yellow, red, blue, green};
  const int cellW = EPD_WIDTH / 6;    // 200 px
  const int cellH = EPD_HEIGHT / 8;   // 200 px

  for (unsigned int x = 0; x < EPD_WIDTH; x += 2) {
    unsigned char c1 = colors[((x / cellW) + (y / cellH)) % 6];
    unsigned char c2 = colors[(((x + 1) / cellW) + (y / cellH)) % 6];
    int new_x = EPD_WIDTH - 2 - x;                 // right to left, same as GD
    row[new_x / 2] = (c1 << 4) | c2;
  }
}

void Display_Checkerboard(void)
{
  static unsigned char row[EPD_ROW_BYTES];

  EPD_CS_All(HIGH);
  EPD_CS(0, LOW);
  EPD_W21_WriteCMD(DTM);
  for (unsigned int y = 0; y < EPD_HEIGHT; y++) {
    checkerRow(y, row);
    EPD_W21_WriteDATA_n(row, EPD_HALF_BYTES);
  }
  EPD_CS_All(HIGH);

  EPD_CS(1, LOW);
  EPD_W21_WriteCMD(DTM);
  for (unsigned int y = 0; y < EPD_HEIGHT; y++) {
    checkerRow(y, row);
    EPD_W21_WriteDATA_n(row + EPD_HALF_BYTES, EPD_HALF_BYTES);
  }
  EPD_CS_All(HIGH);

  EPD_refresh();
}

// 6 vertical bars, 200 px each: every row contains all 6 colours
// (bars = 1 colour per row, checkerboard = 6 colours per row -> isolates row-content dependency)
void Display_Vertical_Bars(void)
{
  static const unsigned char colors[6] = {black, white, yellow, red, blue, green};
  static unsigned char row[EPD_ROW_BYTES];
  for (unsigned int x = 0; x < EPD_WIDTH; x += 2) {
    unsigned char c = colors[x / (EPD_WIDTH / 6)];
    row[x / 2] = (c << 4) | c;
  }

  EPD_CS_All(HIGH);
  EPD_CS(0, LOW);
  EPD_W21_WriteCMD(DTM);
  for (unsigned int y = 0; y < EPD_HEIGHT; y++) EPD_W21_WriteDATA_n(row, EPD_HALF_BYTES);
  EPD_CS_All(HIGH);

  EPD_CS(1, LOW);
  EPD_W21_WriteCMD(DTM);
  for (unsigned int y = 0; y < EPD_HEIGHT; y++) EPD_W21_WriteDATA_n(row + EPD_HALF_BYTES, EPD_HALF_BYTES);
  EPD_CS_All(HIGH);

  EPD_refresh();
}
