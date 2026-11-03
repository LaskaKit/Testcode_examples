#include "GDEB0709E01.h"

#include <string.h>

static const uint8_t PSR_V[2] = {0xDF, 0x6B};
static const uint8_t PWR_V[6] = {0x0F, 0x00, 0x28, 0x2C, 0x28, 0x38};
static const uint8_t POF_V[1] = {0x01};
static const uint8_t POFS_MV[4] = {0x00, 0xC0, 0x03, 0xA8};
static const uint8_t POFS_SV[4] = {0x00, 0xC0, 0x03, 0x9A};
static const uint8_t DRF_V[1] = {0x00};
static const uint8_t PLL_V[1] = {0x08};
static const uint8_t CDI_V[1] = {0x37};
static const uint8_t TCON_V[2] = {0x03, 0x03};
static const uint8_t TRES_V[4] = {0x04, 0xB0, 0x03, 0x20};
static const uint8_t CMD66_V[6] = {0x49, 0x55, 0x13, 0x5D, 0x05, 0x10};
static const uint8_t EN_BUF_V[1] = {0x07};
static const uint8_t CCSET_V[1] = {0x01};
static const uint8_t PWS_V[1] = {0x22};
static const uint8_t AN_TM_V[9] = {0x00, 0x0C, 0x0C, 0xD9, 0xDD, 0xDD, 0x15, 0x15, 0x55};
static const uint8_t AGID_V[1] = {0x10};
static const uint8_t CMDA4_V[9] = {0x03, 0x00, 0x01, 0x03, 0x00, 0x03, 0x00, 0x00, 0x00};
static const uint8_t DCDC_V[3] = {0x44, 0x54, 0x00};
static const uint8_t BTST_P_V[2] = {0xE0, 0x20};
static const uint8_t BOOST_VDDP_EN_V[1] = {0x01};
static const uint8_t BTST_N_V[2] = {0xE0, 0x20};
static const uint8_t BUCK_BOOST_VDDN_V[1] = {0x01};
static const uint8_t TFT_VCOM_POWER_V[1] = {0x02};

enum {
    CMD_PSR = 0x00, CMD_PWR = 0x01, CMD_POF = 0x02, CMD_POFS = 0x03,
    CMD_PON = 0x04, CMD_BTST_N = 0x05, CMD_BTST_P = 0x06,
    CMD_DTM = 0x10, CMD_DRF = 0x12, CMD_PLL = 0x30,
    CMD_CDI = 0x50, CMD_TCON = 0x60, CMD_TRES = 0x61,
    CMD_AN_TM = 0x74, CMD_AGID = 0x86, CMD_CMDA4 = 0xA4,
    CMD_DCDC = 0xA5, CMD_BUCK_BOOST_VDDN = 0xB0,
    CMD_TFT_VCOM_POWER = 0xB1, CMD_EN_BUF = 0xB6,
    CMD_BOOST_VDDP_EN = 0xB7, CMD_CCSET = 0xE0, CMD_PWS = 0xE3,
    CMD_CMD66 = 0xF0
};

static inline void CSAll(uint8_t value)
{
    DEV_Digital_Write(EPD_CS_M_PIN, value);
    DEV_Digital_Write(EPD_CS_S_PIN, value);
}

static void SendCommand(uint8_t command)
{
    DEV_Digital_Write(EPD_DC_PIN, 0);
    DEV_SPI_WriteByte(command);
}

static void SendDataByte(uint8_t data)
{
    DEV_Digital_Write(EPD_DC_PIN, 1);
    DEV_SPI_WriteByte(data);
}

static void SendData(const uint8_t *data, uint32_t len)
{
    DEV_Digital_Write(EPD_DC_PIN, 1);
    DEV_SPI_Write_nByte(data, len);
}

static void SendRegister(uint8_t command, const uint8_t *data, uint32_t len)
{
    SendCommand(command);
    SendData(data, len);
}

static void ResetPanel(void)
{
    DEV_Digital_Write(EPD_RST_PIN, 1);
    DEV_Delay_ms(30);
    DEV_Digital_Write(EPD_RST_PIN, 0);
    DEV_Delay_ms(30);
    DEV_Digital_Write(EPD_RST_PIN, 1);
    DEV_Delay_ms(30);
    DEV_Digital_Write(EPD_RST_PIN, 0);
    DEV_Delay_ms(30);
    DEV_Digital_Write(EPD_RST_PIN, 1);
    DEV_Delay_ms(30);
}

static void WaitBusyRelease(void)
{
    Serial.println("EPD busy...");
    while (!DEV_Digital_Read(EPD_BUSY_PIN)) {
        DEV_Delay_ms(10);
    }
    DEV_Delay_ms(20);
    Serial.println("EPD ready.");
}

static void WriteMasterRegister(uint8_t command, const uint8_t *data, uint32_t len)
{
    DEV_Digital_Write(EPD_CS_M_PIN, 0);
    SendRegister(command, data, len);
    DEV_Digital_Write(EPD_CS_M_PIN, 1);
}

static void WriteSlaveRegister(uint8_t command, const uint8_t *data, uint32_t len)
{
    DEV_Digital_Write(EPD_CS_S_PIN, 0);
    SendRegister(command, data, len);
    DEV_Digital_Write(EPD_CS_S_PIN, 1);
}

void GDEB0709E01_Init(void)
{
    ResetPanel();
    WaitBusyRelease();

    CSAll(1);
    WriteMasterRegister(CMD_AN_TM, AN_TM_V, sizeof(AN_TM_V));
    CSAll(1);

    CSAll(0);
    SendRegister(CMD_CMD66, CMD66_V, sizeof(CMD66_V));
    CSAll(1);

    CSAll(0);
    SendRegister(CMD_PSR, PSR_V, sizeof(PSR_V));
    CSAll(1);

    WriteMasterRegister(CMD_DCDC, DCDC_V, sizeof(DCDC_V));

    CSAll(0);
    SendRegister(CMD_PLL, PLL_V, sizeof(PLL_V));
    CSAll(1);

    CSAll(0);
    SendRegister(CMD_CDI, CDI_V, sizeof(CDI_V));
    CSAll(1);

    CSAll(0);
    SendRegister(CMD_TCON, TCON_V, sizeof(TCON_V));
    CSAll(1);

    WriteMasterRegister(CMD_POFS, POFS_MV, sizeof(POFS_MV));
    WriteSlaveRegister(CMD_POFS, POFS_SV, sizeof(POFS_SV));

    CSAll(0);
    SendRegister(CMD_AGID, AGID_V, sizeof(AGID_V));
    CSAll(1);

    CSAll(0);
    SendRegister(CMD_PWS, PWS_V, sizeof(PWS_V));
    CSAll(1);

    CSAll(0);
    SendRegister(CMD_CCSET, CCSET_V, sizeof(CCSET_V));
    CSAll(1);

    CSAll(0);
    SendRegister(CMD_TRES, TRES_V, sizeof(TRES_V));
    CSAll(1);

    WriteMasterRegister(CMD_CMDA4, CMDA4_V, sizeof(CMDA4_V));
    WriteMasterRegister(CMD_PWR, PWR_V, sizeof(PWR_V));
    WriteMasterRegister(CMD_EN_BUF, EN_BUF_V, sizeof(EN_BUF_V));
    WriteMasterRegister(CMD_BTST_P, BTST_P_V, sizeof(BTST_P_V));
    WriteMasterRegister(CMD_BOOST_VDDP_EN, BOOST_VDDP_EN_V, sizeof(BOOST_VDDP_EN_V));
    WriteMasterRegister(CMD_BTST_N, BTST_N_V, sizeof(BTST_N_V));
    WriteMasterRegister(CMD_BUCK_BOOST_VDDN, BUCK_BOOST_VDDN_V, sizeof(BUCK_BOOST_VDDN_V));
    WriteMasterRegister(CMD_TFT_VCOM_POWER, TFT_VCOM_POWER_V, sizeof(TFT_VCOM_POWER_V));

    Serial.println("EPD initialization complete.");
}

static void TurnOnDisplay(void)
{
    Serial.println("EPD power on");
    CSAll(0);
    SendCommand(CMD_PON);
    CSAll(1);
    WaitBusyRelease();

    DEV_Delay_ms(50);

    CSAll(0);
    SendRegister(CMD_DRF, DRF_V, sizeof(DRF_V));
    CSAll(1);
    WaitBusyRelease();

    CSAll(0);
    SendRegister(CMD_POF, POF_V, sizeof(POF_V));
    CSAll(1);
    WaitBusyRelease();

    Serial.println("EPD refresh complete.");
}

void GDEB0709E01_Display(const uint8_t *image)
{
    if (!image) {
        return;
    }

    /*
     * GDEB0709E01
     * Resolution: 1200 x 1600
     *
     * 4-bit/pixel
     * Full image:
     *     1200 / 2 = 600 bytes per row
     *
     * Dual controller:
     *     Master = left/right 600 pixels = 300 bytes
     *     Slave  = remaining 600 pixels = 300 bytes
     *
     * IMPORTANT:
     * DTM command 0x10 is sent ONCE for each controller.
     * CS must remain LOW while the complete 1600-row image
     * data is transferred.
     */

    const uint32_t widthBytes =
        GDEB0709E01_WIDTH / 2;       // 600 bytes

    const uint32_t sideBytes =
        widthBytes / 2;              // 300 bytes

    /*
     * =========================
     * Master controller
     * =========================
     */

    DEV_Digital_Write(EPD_CS_M_PIN, 0);

    SendCommand(CMD_DTM);

    for (uint32_t row = 0;
         row < GDEB0709E01_HEIGHT;
         row++)
    {
        const uint8_t *line =
            image + row * widthBytes;

        SendData(line, sideBytes);
    }

    DEV_Digital_Write(EPD_CS_M_PIN, 1);


    /*
     * =========================
     * Slave controller
     * =========================
     */

    DEV_Digital_Write(EPD_CS_S_PIN, 0);

    SendCommand(CMD_DTM);

    for (uint32_t row = 0;
         row < GDEB0709E01_HEIGHT;
         row++)
    {
        const uint8_t *line =
            image + row * widthBytes;

        SendData(line + sideBytes, sideBytes);
    }

    DEV_Digital_Write(EPD_CS_S_PIN, 1);


    /*
     * Start display refresh
     */
    TurnOnDisplay();
}

void GDEB0709E01_Clear(uint8_t color)
{
    const uint8_t packed =
        (uint8_t)((color << 4) | color);

    uint8_t line[GDEB0709E01_SIDE_BYTES];

    memset(line, packed, sizeof(line));


    /*
     * =========================
     * Master
     * =========================
     */

    DEV_Digital_Write(EPD_CS_M_PIN, 0);

    SendCommand(CMD_DTM);

    for (uint32_t row = 0;
         row < GDEB0709E01_HEIGHT;
         row++)
    {
        SendData(line, sizeof(line));
    }

    DEV_Digital_Write(EPD_CS_M_PIN, 1);


    /*
     * =========================
     * Slave
     * =========================
     */

    DEV_Digital_Write(EPD_CS_S_PIN, 0);

    SendCommand(CMD_DTM);

    for (uint32_t row = 0;
         row < GDEB0709E01_HEIGHT;
         row++)
    {
        SendData(line, sizeof(line));
    }

    DEV_Digital_Write(EPD_CS_S_PIN, 1);


    /*
     * Refresh
     */
    TurnOnDisplay();
}
void GDEB0709E01_Sleep(void)
{
    CSAll(0);
    SendCommand(CMD_POF);
    SendDataByte(0xA5);
    CSAll(1);
    DEV_Delay_ms(20);
    DEV_Digital_Write(EPD_PWR_PIN, 0);
}
