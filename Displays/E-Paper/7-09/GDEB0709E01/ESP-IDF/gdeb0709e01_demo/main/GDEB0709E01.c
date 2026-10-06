#include "GDEB0709E01.h"

#include <string.h>
#include "driver/gpio.h"
#include "driver/spi_master.h"
#include "esp_log.h"
#include "esp_heap_caps.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define EPD_SCK_PIN     9
#define EPD_MOSI_PIN    41
#define EPD_MISO_PIN    40
#define EPD_CS_M_PIN    18
#define EPD_CS_S_PIN    17
#define EPD_RST_PIN     6
#define EPD_DC_PIN      2
#define EPD_BUSY_PIN    7
#define EPD_PWR_PIN     45

static const char *TAG = "GDEB0709E01";
static spi_device_handle_t s_spi = NULL;
static uint8_t *s_dma_buf = NULL;

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

static void cs_all(int level)
{
    gpio_set_level(EPD_CS_M_PIN, level);
    gpio_set_level(EPD_CS_S_PIN, level);
}

static esp_err_t spi_tx(const uint8_t *data, size_t len)
{
    if (len > GDEB0709E01_SIDE_BYTES) return ESP_ERR_INVALID_SIZE;
    memcpy(s_dma_buf, data, len);

    spi_transaction_t t = {0};
    t.length = len * 8;
    t.tx_buffer = s_dma_buf;
    return spi_device_transmit(s_spi, &t);
}

static esp_err_t send_command(uint8_t cmd)
{
    gpio_set_level(EPD_DC_PIN, 0);
    s_dma_buf[0] = cmd;
    spi_transaction_t t = {0};
    t.length = 8;
    t.tx_buffer = s_dma_buf;
    return spi_device_transmit(s_spi, &t);
}

static esp_err_t send_data(const uint8_t *data, size_t len)
{
    gpio_set_level(EPD_DC_PIN, 1);
    return spi_tx(data, len);
}

static esp_err_t send_register(uint8_t cmd, const uint8_t *data, size_t len)
{
    esp_err_t err = send_command(cmd);
    if (err != ESP_OK) return err;
    return send_data(data, len);
}

static esp_err_t write_master(uint8_t cmd, const uint8_t *data, size_t len)
{
    gpio_set_level(EPD_CS_M_PIN, 0);
    esp_err_t err = send_register(cmd, data, len);
    gpio_set_level(EPD_CS_M_PIN, 1);
    return err;
}

static esp_err_t write_slave(uint8_t cmd, const uint8_t *data, size_t len)
{
    gpio_set_level(EPD_CS_S_PIN, 0);
    esp_err_t err = send_register(cmd, data, len);
    gpio_set_level(EPD_CS_S_PIN, 1);
    return err;
}

static void reset_panel(void)
{
    gpio_set_level(EPD_RST_PIN, 1);
    vTaskDelay(pdMS_TO_TICKS(30));
    gpio_set_level(EPD_RST_PIN, 0);
    vTaskDelay(pdMS_TO_TICKS(30));
    gpio_set_level(EPD_RST_PIN, 1);
    vTaskDelay(pdMS_TO_TICKS(30));
    gpio_set_level(EPD_RST_PIN, 0);
    vTaskDelay(pdMS_TO_TICKS(30));
    gpio_set_level(EPD_RST_PIN, 1);
    vTaskDelay(pdMS_TO_TICKS(30));
}

static void wait_busy_release(void)
{
    ESP_LOGI(TAG, "EPD busy...");
    while (gpio_get_level(EPD_BUSY_PIN) == 0) {
        vTaskDelay(pdMS_TO_TICKS(10));
    }
    vTaskDelay(pdMS_TO_TICKS(20));
    ESP_LOGI(TAG, "EPD ready.");
}

static esp_err_t turn_on_display(void)
{
    esp_err_t err;

    cs_all(0);
    err = send_command(CMD_PON);
    cs_all(1);
    if (err != ESP_OK) return err;
    wait_busy_release();

    vTaskDelay(pdMS_TO_TICKS(50));

    cs_all(0);
    err = send_register(CMD_DRF, DRF_V, sizeof(DRF_V));
    cs_all(1);
    if (err != ESP_OK) return err;
    wait_busy_release();

    cs_all(0);
    err = send_register(CMD_POF, POF_V, sizeof(POF_V));
    cs_all(1);
    if (err != ESP_OK) return err;
    wait_busy_release();

    ESP_LOGI(TAG, "EPD refresh complete.");
    return ESP_OK;
}

esp_err_t gdeb0709e01_init(void)
{
    gpio_config_t io = {
        .pin_bit_mask = (1ULL << EPD_RST_PIN) |
                        (1ULL << EPD_DC_PIN) |
                        (1ULL << EPD_CS_M_PIN) |
                        (1ULL << EPD_CS_S_PIN) |
                        (1ULL << EPD_PWR_PIN),
        .mode = GPIO_MODE_OUTPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    ESP_ERROR_CHECK(gpio_config(&io));

    gpio_config_t busy = {
        .pin_bit_mask = (1ULL << EPD_BUSY_PIN),
        .mode = GPIO_MODE_INPUT,
        .pull_up_en = GPIO_PULLUP_DISABLE,
        .pull_down_en = GPIO_PULLDOWN_DISABLE,
        .intr_type = GPIO_INTR_DISABLE
    };
    ESP_ERROR_CHECK(gpio_config(&busy));

    gpio_set_level(EPD_CS_M_PIN, 1);
    gpio_set_level(EPD_CS_S_PIN, 1);
    gpio_set_level(EPD_DC_PIN, 1);
    gpio_set_level(EPD_PWR_PIN, 1);
    gpio_set_level(EPD_RST_PIN, 1);

    spi_bus_config_t buscfg = {
        .mosi_io_num = EPD_MOSI_PIN,
        .miso_io_num = EPD_MISO_PIN,
        .sclk_io_num = EPD_SCK_PIN,
        .quadwp_io_num = -1,
        .quadhd_io_num = -1,
        .max_transfer_sz = GDEB0709E01_SIDE_BYTES
    };

    ESP_ERROR_CHECK(spi_bus_initialize(SPI2_HOST, &buscfg, SPI_DMA_CH_AUTO));

    spi_device_interface_config_t devcfg = {
        .clock_speed_hz = 20000000,
        .mode = 0,
        .spics_io_num = -1,
        .queue_size = 1,
        .flags = 0
    };

    ESP_ERROR_CHECK(spi_bus_add_device(SPI2_HOST, &devcfg, &s_spi));

    s_dma_buf = heap_caps_malloc(GDEB0709E01_SIDE_BYTES, MALLOC_CAP_DMA);
    if (!s_dma_buf) return ESP_ERR_NO_MEM;

    reset_panel();
    wait_busy_release();

    cs_all(1);

    ESP_ERROR_CHECK(write_master(CMD_AN_TM, AN_TM_V, sizeof(AN_TM_V)));

    cs_all(0);
    ESP_ERROR_CHECK(send_register(CMD_CMD66, CMD66_V, sizeof(CMD66_V)));
    cs_all(1);

    cs_all(0);
    ESP_ERROR_CHECK(send_register(CMD_PSR, PSR_V, sizeof(PSR_V)));
    cs_all(1);

    ESP_ERROR_CHECK(write_master(CMD_DCDC, DCDC_V, sizeof(DCDC_V)));

    cs_all(0);
    ESP_ERROR_CHECK(send_register(CMD_PLL, PLL_V, sizeof(PLL_V)));
    cs_all(1);

    cs_all(0);
    ESP_ERROR_CHECK(send_register(CMD_CDI, CDI_V, sizeof(CDI_V)));
    cs_all(1);

    cs_all(0);
    ESP_ERROR_CHECK(send_register(CMD_TCON, TCON_V, sizeof(TCON_V)));
    cs_all(1);

    ESP_ERROR_CHECK(write_master(CMD_POFS, POFS_MV, sizeof(POFS_MV)));
    ESP_ERROR_CHECK(write_slave(CMD_POFS, POFS_SV, sizeof(POFS_SV)));

    cs_all(0);
    ESP_ERROR_CHECK(send_register(CMD_AGID, AGID_V, sizeof(AGID_V)));
    cs_all(1);

    cs_all(0);
    ESP_ERROR_CHECK(send_register(CMD_PWS, PWS_V, sizeof(PWS_V)));
    cs_all(1);

    cs_all(0);
    ESP_ERROR_CHECK(send_register(CMD_CCSET, CCSET_V, sizeof(CCSET_V)));
    cs_all(1);

    cs_all(0);
    ESP_ERROR_CHECK(send_register(CMD_TRES, TRES_V, sizeof(TRES_V)));
    cs_all(1);

    ESP_ERROR_CHECK(write_master(CMD_CMDA4, CMDA4_V, sizeof(CMDA4_V)));
    ESP_ERROR_CHECK(write_master(CMD_PWR, PWR_V, sizeof(PWR_V)));
    ESP_ERROR_CHECK(write_master(CMD_EN_BUF, EN_BUF_V, sizeof(EN_BUF_V)));
    ESP_ERROR_CHECK(write_master(CMD_BTST_P, BTST_P_V, sizeof(BTST_P_V)));
    ESP_ERROR_CHECK(write_master(CMD_BOOST_VDDP_EN, BOOST_VDDP_EN_V, sizeof(BOOST_VDDP_EN_V)));
    ESP_ERROR_CHECK(write_master(CMD_BTST_N, BTST_N_V, sizeof(BTST_N_V)));
    ESP_ERROR_CHECK(write_master(CMD_BUCK_BOOST_VDDN, BUCK_BOOST_VDDN_V, sizeof(BUCK_BOOST_VDDN_V)));
    ESP_ERROR_CHECK(write_master(CMD_TFT_VCOM_POWER, TFT_VCOM_POWER_V, sizeof(TFT_VCOM_POWER_V)));

    ESP_LOGI(TAG, "EPD initialization complete.");
    return ESP_OK;
}

esp_err_t gdeb0709e01_display(const uint8_t *image)
{
    if (image == NULL) {
        return ESP_ERR_INVALID_ARG;
    }

    /*
     * GDEB0709E01
     *
     * Resolution:
     *     1200 x 1600
     *
     * 4 bits per pixel:
     *     1200 / 2 = 600 bytes per line
     *
     * Dual controller:
     *     Master: 600 pixels = 300 bytes per line
     *     Slave : 600 pixels = 300 bytes per line
     *
     * Image layout:
     *
     *     |<---------------- 1200 pixels ---------------->|
     *
     *     |<---- 600 ---->|<-------- 600 -------->|
     *     |   MASTER      |       SLAVE           |
     *
     *     Each line:
     *
     *         300 bytes    +    300 bytes
     *
     * IMPORTANT:
     *
     * CMD_DTM (0x10) must be sent only ONCE for each
     * controller. CS must remain LOW during the complete
     * image transfer.
     *
     * This follows the verified continuous-transfer
     * sequence used by the original dual-controller
     * display driver.
     */

    const uint32_t width_bytes = GDEB0709E01_BYTES_PER_ROW;
    const uint32_t side_bytes  = GDEB0709E01_SIDE_BYTES;


    /*
     * ========================================================
     * Master Controller
     * ========================================================
     *
     * CS_M LOW
     * CMD 0x10
     * 1600 lines x 300 bytes
     * CS_M HIGH
     */

    gpio_set_level(EPD_CS_M_PIN, 0);

    esp_err_t err = send_command(CMD_DTM);

    if (err == ESP_OK) {

        for (uint32_t row = 0;
             row < GDEB0709E01_HEIGHT;
             row++)
        {
            const uint8_t *line =
                image + row * width_bytes;

            err = send_data(line, side_bytes);

            if (err != ESP_OK) {
                break;
            }
        }
    }

    gpio_set_level(EPD_CS_M_PIN, 1);

    if (err != ESP_OK) {
        return err;
    }


    /*
     * ========================================================
     * Slave Controller
     * ========================================================
     *
     * CS_S LOW
     * CMD 0x10
     * 1600 lines x 300 bytes
     * CS_S HIGH
     */

    gpio_set_level(EPD_CS_S_PIN, 0);

    err = send_command(CMD_DTM);

    if (err == ESP_OK) {

        for (uint32_t row = 0;
             row < GDEB0709E01_HEIGHT;
             row++)
        {
            const uint8_t *line =
                image +
                row * width_bytes +
                side_bytes;

            err = send_data(line, side_bytes);

            if (err != ESP_OK) {
                break;
            }
        }
    }

    gpio_set_level(EPD_CS_S_PIN, 1);

    if (err != ESP_OK) {
        return err;
    }


    /*
     * ========================================================
     * Start display refresh
     * ========================================================
     */

    return turn_on_display();
}

esp_err_t gdeb0709e01_clear(uint8_t color)
{
    /*
     * GDEB0709E01:
     *
     * 4 bits per pixel
     *
     * One byte contains two pixels:
     *
     *     [pixel][pixel]
     *
     * Therefore the two 4-bit color values are packed as:
     *
     *     color << 4 | color
     *
     * Example:
     *
     *     WHITE = 0x1
     *
     *     packed = 0x11
     */

    uint8_t packed =
        (uint8_t)((color << 4) | color);


    /*
     * One controller line:
     *
     * 600 pixels / 2 = 300 bytes
     */

    memset(
        s_dma_buf,
        packed,
        GDEB0709E01_SIDE_BYTES
    );


    /*
     * ========================================================
     * Master Controller
     * ========================================================
     *
     * CS_M LOW
     * CMD 0x10
     * 1600 lines x 300 bytes
     * CS_M HIGH
     */

    gpio_set_level(EPD_CS_M_PIN, 0);

    esp_err_t err = send_command(CMD_DTM);

    if (err == ESP_OK) {

        for (uint32_t row = 0;
             row < GDEB0709E01_HEIGHT;
             row++)
        {
            err = send_data(
                s_dma_buf,
                GDEB0709E01_SIDE_BYTES
            );

            if (err != ESP_OK) {
                break;
            }
        }
    }

    gpio_set_level(EPD_CS_M_PIN, 1);

    if (err != ESP_OK) {
        return err;
    }


    /*
     * ========================================================
     * Slave Controller
     * ========================================================
     *
     * CS_S LOW
     * CMD 0x10
     * 1600 lines x 300 bytes
     * CS_S HIGH
     */

    gpio_set_level(EPD_CS_S_PIN, 0);

    err = send_command(CMD_DTM);

    if (err == ESP_OK) {

        for (uint32_t row = 0;
             row < GDEB0709E01_HEIGHT;
             row++)
        {
            err = send_data(
                s_dma_buf,
                GDEB0709E01_SIDE_BYTES
            );

            if (err != ESP_OK) {
                break;
            }
        }
    }

    gpio_set_level(EPD_CS_S_PIN, 1);

    if (err != ESP_OK) {
        return err;
    }


    /*
     * ========================================================
     * Start display refresh
     * ========================================================
     */

    return turn_on_display();
}

esp_err_t gdeb0709e01_sleep(void)
{
    cs_all(0);
    esp_err_t err = send_command(CMD_POF);
    if (err == ESP_OK) err = send_data((const uint8_t[]){0xA5}, 1);
    cs_all(1);
    vTaskDelay(pdMS_TO_TICKS(20));
    gpio_set_level(EPD_PWR_PIN, 0);
    return err;
}
