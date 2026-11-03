

#define __COMM_C__

#include <string.h>
#include <stdio.h>
#include "comm.h"
#include "pindefine.h"
#include "status.h"

#define SPI	SPI3_HOST
#define SPI_MAX_BUFFER_SIZE	32768

spi_device_handle_t spi;

esp_err_t initialSpi(void)
{
	esp_err_t status;

    spi_bus_config_t spiBusConfig={
        .data0_io_num=SPI_Data0,
        .data1_io_num=SPI_Data1,
        .sclk_io_num=SPI_CLK,
        .quadwp_io_num=-1,
        .quadhd_io_num=-1,
        .flags=SPICOMMON_BUSFLAG_MASTER,
        .max_transfer_sz = SPI_MAX_BUFFER_SIZE
    };

    spi_device_interface_config_t spiDeviceConfig = {
        .command_bits   = 8,
        .clock_speed_hz = SPI_MASTER_FREQ_10M,   //Clock out at 10 MHz
        .duty_cycle_pos=128,                     //50% duty cycle
        .queue_size=7,                           //We want to be able to queue 7 transactions at a time
        .cs_ena_posttrans=3,                     //Keep the CS low 3 cycles after transaction, to stop slave from missing the last bit when CS has less propagation delay than CLK
    };

    status=spi_bus_initialize(SPI, &spiBusConfig, SPI_DMA_CH_AUTO);
	ESP_ERROR_CHECK(status);
    //Attach the LCD to the SPI bus
	status=spi_bus_add_device(SPI, &spiDeviceConfig, &spi);
    ESP_ERROR_CHECK(status);

#if SHOW_LOG
    if(status == ESP_OK)
    {
        printf("initialSpi() has been executed. \r\n");
    }
#endif

	return status;

}

void initialGpio(void)
{
	esp_err_t status;

    gpio_config_t ioConfig = {};

    ioConfig.pin_bit_mask = ((1ULL<<EPD_RST) | (1ULL<<SPI_CS0) | (1ULL<<SPI_CS1)
                            | (1ULL<<LOAD_SW)  );
    ioConfig.mode = GPIO_MODE_OUTPUT;
    ioConfig.pull_up_en = GPIO_PULLUP_ENABLE;
    status = gpio_config(&ioConfig);

#if SHOW_LOG
    if(status == ESP_OK)
    {
        printf("gpio_config(&ioConfig) has been executed. \r\n");
    }
#endif

    ioConfig.pin_bit_mask = (1ULL<<EPD_BUSY);
    ioConfig.intr_type = GPIO_INTR_NEGEDGE;
    ioConfig.mode = GPIO_MODE_INPUT;
    ioConfig.pull_down_en = GPIO_PULLDOWN_DISABLE;
    ioConfig.pull_up_en = GPIO_PULLUP_DISABLE;
    status = gpio_config(&ioConfig);

#if SHOW_LOG
    if(status == ESP_OK)
    {
    	printf("initialGpio() has been executed. \r\n");
    }
#endif

}

void delayms(unsigned int delayTime)
{
	vTaskDelay(delayTime/portTICK_PERIOD_MS);
}

esp_err_t spiTransmitCommand(unsigned char commandBuf)
{
	esp_err_t status;
	spi_transaction_t trans;

	memset(&trans, 0, sizeof(trans));
    trans.cmd = commandBuf;

    trans.length = 0;
    trans.tx_buffer = NULL;
    status = spi_device_transmit(spi, &trans);
    assert(status==ESP_OK);

	return status;

}

esp_err_t spiTransmitData(unsigned char *dataBuffer, unsigned long dataLength)
{
	esp_err_t status=0;

	spi_transaction_ext_t trans_ext;

	while(dataLength >= SPI_MAX_BUFFER_SIZE)
	{
		memset(&trans_ext, 0, sizeof(trans_ext));
		trans_ext.command_bits = 0;
		trans_ext.base.length = SPI_MAX_BUFFER_SIZE * 8;
		// The trans_ext.base.length unit is bit, so the SPI_MAX_BUFFER_SIZE must be multiplied by 8.
		trans_ext.base.tx_buffer =  dataBuffer;
		trans_ext.base.flags   = SPI_TRANS_VARIABLE_CMD;
		status = spi_device_transmit(spi, &trans_ext);
		dataLength -= SPI_MAX_BUFFER_SIZE;
		dataBuffer += SPI_MAX_BUFFER_SIZE;
	}

	if(dataLength > 0)
	{
		memset(&trans_ext, 0, sizeof(trans_ext));
		trans_ext.command_bits = 0;
		trans_ext.base.length = dataLength * 8;
		// The trans_ext.base.length unit is bit, so the dataLength must be multiplied by 8.
		trans_ext.base.tx_buffer =  dataBuffer;
		trans_ext.base.flags   = SPI_TRANS_VARIABLE_CMD;
		status = spi_device_transmit(spi, &trans_ext);
	}

	return status;

}

esp_err_t spiReceiveData(unsigned char *dataBuffer, unsigned long dataLength)
{
	esp_err_t status=0;

	spi_transaction_ext_t trans_ext;

	while(dataLength > SPI_MAX_BUFFER_SIZE)
	{
		memset(&trans_ext, 0, sizeof(trans_ext));

		trans_ext.command_bits = 0;
		trans_ext.base.length = SPI_MAX_BUFFER_SIZE * 8;
		// The trans_ext.base.length unit is bit, so the SPI_MAX_BUFFER_SIZE must be multiplied by 8.
		trans_ext.base.rx_buffer =  dataBuffer;
		trans_ext.base.rxlength = dataLength * 8;
		trans_ext.base.flags   = SPI_TRANS_VARIABLE_CMD;
		status = spi_device_transmit(spi, &trans_ext);
		dataLength -= SPI_MAX_BUFFER_SIZE;
		dataBuffer += SPI_MAX_BUFFER_SIZE;
	}

	if(dataLength > 0)
	{
		memset(&trans_ext, 0, sizeof(trans_ext));

		trans_ext.command_bits = 0;
		trans_ext.base.length = dataLength * 8;
		// The trans_ext.base.length unit is bit, so the dataLength must be multiplied by 8.
		trans_ext.base.rx_buffer =  dataBuffer;
		trans_ext.base.rxlength = dataLength * 8;
		trans_ext.base.flags   = SPI_TRANS_VARIABLE_CMD;
		status = spi_device_transmit(spi, &trans_ext);
	}

	return status;

}

esp_err_t spiTransmitLargeData(unsigned char commandBuf, unsigned char *dataBuffer, unsigned long dataLength)
{
	esp_err_t status=0;
	spi_transaction_t trans;
	spi_transaction_ext_t trans_ext;

	unsigned char firstPacket = 1;

	while(dataLength > SPI_MAX_BUFFER_SIZE)
	{
		if(firstPacket)
		{
			memset(&trans, 0, sizeof(trans));
			trans.cmd = commandBuf;
			trans.length = SPI_MAX_BUFFER_SIZE * 8;
			// The trans.length unit is bit, so the SPI_MAX_BUFFER_SIZE must be multiplied by 8.
			trans.tx_buffer = dataBuffer;
			trans.rx_buffer = NULL;
			status = spi_device_transmit(spi, &trans);
			firstPacket = 0;
		}
		else
		{
			memset(&trans_ext, 0, sizeof(trans_ext));
			trans_ext.command_bits = 0;
			trans_ext.base.length = SPI_MAX_BUFFER_SIZE * 8;
			// The trans_ext.base.length unit is bit, so the dataLength must be multiplied by 8.
			trans_ext.base.tx_buffer =  dataBuffer;
			trans_ext.base.flags   = SPI_TRANS_VARIABLE_CMD;
			status = spi_device_transmit(spi, &trans_ext);
		}

		dataLength -= SPI_MAX_BUFFER_SIZE;
		dataBuffer += SPI_MAX_BUFFER_SIZE;

	}

	if(dataLength > 0)
	{
		if(firstPacket)
		{
			memset(&trans, 0, sizeof(trans));
			trans.cmd = commandBuf;
			trans.length = dataLength * 8;
			// The trans.length unit is bit, so the dataLength must be multiplied by 8.
			trans.tx_buffer = dataBuffer;
			trans.rx_buffer = NULL;
			status = spi_device_transmit(spi, &trans);
			firstPacket = 0;
		}
		else
		{
			memset(&trans_ext, 0, sizeof(trans_ext));
			trans_ext.command_bits = 0;
			trans_ext.base.length = dataLength * 8;
			// The trans_ext.base.length unit is bit, so the dataLength must be multiplied by 8.
			trans_ext.base.tx_buffer =  dataBuffer;
			trans_ext.base.flags   = SPI_TRANS_VARIABLE_CMD;
			status = spi_device_transmit(spi, &trans_ext);

		}
	}

	return status;

}

esp_err_t spiTransmit(unsigned char commandBuf, unsigned char *dataBuffer, unsigned int dataLength)
{
	esp_err_t status=0;
	spi_transaction_t trans;

	memset(&trans, 0, sizeof(trans));

	if(dataLength < SPI_MAX_BUFFER_SIZE)
	{
		trans.cmd = commandBuf;
		trans.length = dataLength * 8;
		// The trans.length unit is bit, so the dataLength must be multiplied by 8.
		trans.tx_buffer = dataBuffer;
		trans.rx_buffer = NULL;
		status = spi_device_transmit(spi, &trans);

	}
	else status = -1; // The dataLength is over the SPI_MAX_BUFFER_SIZE

	return status;
}

esp_err_t spiReceive(unsigned char commandBuf, unsigned char *dataBuffer, unsigned int dataLength)
{
	esp_err_t status=0;

	spi_transaction_t trans;

	memset(&trans, 0, sizeof(trans));

	if(dataLength < SPI_MAX_BUFFER_SIZE)
	{
	    trans.cmd = commandBuf;
		trans.length= dataLength * 8;
		// The trans.length unit is bit, so the dataLength must be multiplied by 8.
		trans.rxlength = dataLength * 8;
		// The trans.rxlength unit is bit, so the dataLength must be multiplied by 8.
		trans.rx_buffer = dataBuffer;

		//==== SPI Transmit Command & Receive Data ====
		status = spi_device_transmit(spi, &trans);
		assert( status == ESP_OK );
	}
	else status = -1; // The dataLength is over the SPI_MAX_BUFFER_SIZE

	return status;

}

void setGpioLevel(unsigned char pinNumber, unsigned char voltageLevel)
{
	//==== Set GPIO voltage level ====
	gpio_set_level(pinNumber, voltageLevel);
}

unsigned char getGpioLevel(unsigned char pinNumber)
{
	unsigned char voltageLevel;
	//==== Get GPIO voltage level ====
	voltageLevel = gpio_get_level(pinNumber);

	return voltageLevel;
}

