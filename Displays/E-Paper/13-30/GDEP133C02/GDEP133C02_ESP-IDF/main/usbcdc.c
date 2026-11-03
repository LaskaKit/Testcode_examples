
#include <stdio.h>
#include <stdint.h>
#include <stdlib.h>
#include <string.h>
#include "usbcdc.h"

extern uint8_t USB_Rx_flag;
extern uint16_t AP_data_Total_Packet;
extern uint16_t AP_data_Switch_Packet;
extern uint16_t AP_data_Switch_Remain;
extern uint16_t AP_data_Remain;
extern uint16_t AP_data_Packet_Cnt;

const char *TAG_USB_CDC = "USB_CDC";
extern uint32_t readCDCDataTotal;
extern uint32_t readCDCDataCnt;
extern uint32_t AP_DriverIC_data_cnt;

void tinyusb_cdc_rx_callback(int itf, cdcacm_event_t *event)
{
    if (USB_Rx_flag != 1)
    {

        esp_err_t ret1;
        
        // printf("Get USB_Rx_flag\r\n");
        if (readCDCDataCnt == 0)
        {
            // printf("readCDCDataCnt == 0\r\n");
            ret1 = tinyusb_cdcacm_read(itf, &databuf[readCDCDataCnt], 6, &rx_size);
            // printf("get first packet header %d bytes data\r\n", rx_size);
            readCDCDataCnt += rx_size;
            //  printf("CMD == %02X\r\n",databuf[0]);
           switch (databuf[0])
           {
           case 0xC0:
           ret1 = tinyusb_cdcacm_read(itf, &databuf[readCDCDataCnt], 58, &rx_size);
           readCDCDataCnt += rx_size;
           readCDCDataTotal = 7+(databuf[4]*256)+databuf[5];

            
            break;
            case 0xC1:
            readCDCDataTotal = 6;
            
            break;
            case 0xC2:
            ret1 = tinyusb_cdcacm_read(itf, &databuf[readCDCDataCnt], 58, &rx_size);
            readCDCDataCnt += rx_size;
            readCDCDataTotal = 7+(databuf[4]*256)+databuf[5];

            break;
            case 0xC3:
            readCDCDataTotal = 6;
            
            break;
            case 0xC4:
            readCDCDataTotal = 6;
            

            break;
            case 0xC5:
            readCDCDataTotal = 6;
            
            break;
            case 0xFF:
            readCDCDataTotal = 6;
            
            break;

           
           default:
               ret1 = tinyusb_cdcacm_read(itf, &databuf[readCDCDataCnt], 58, &rx_size);
               readCDCDataCnt += rx_size;
               readCDCDataTotal = 7+(databuf[4]*256)+databuf[5];

            break;
           }
        //    printf("(0x%02X first)rx_size=> %d, readCDCDataTotal=> %d, readCDCDataCnt=> %d \r\n", databuf[0], rx_size, readCDCDataTotal, readCDCDataCnt);
            
        }else if  ((readCDCDataTotal - readCDCDataCnt) > 64)
        {
            ret1 = tinyusb_cdcacm_read(itf, &databuf[readCDCDataCnt], 64, &rx_size);
            readCDCDataCnt += rx_size;
            // printf("rx_size => %d, readCDCDataTotal=> %d, readCDCDataCnt=> %d \r\n",rx_size , readCDCDataTotal, readCDCDataCnt);
            
        }
        else if ((readCDCDataTotal - readCDCDataCnt) <= 64)
        {
            ret1 = tinyusb_cdcacm_read(itf, &databuf[readCDCDataCnt], (readCDCDataTotal - readCDCDataCnt), &rx_size);
            readCDCDataCnt += rx_size;
            // printf("(0x%02X last)rx_size => %d, readCDCDataTotal=> %d, readCDCDataCnt=> %d \r\n", databuf[0], rx_size , readCDCDataTotal, readCDCDataCnt);
     
        }
        // printf("readCDCDataTotal = %d, readCDCDataCnt = %d\r\n",readCDCDataTotal, readCDCDataCnt);
        if (readCDCDataTotal == readCDCDataCnt)
        {
            /* code */
            USB_Rx_flag = 1;
            readCDCDataCnt = 0;
            // printf("Get 0x%02X Group Packet \r\n",databuf[0]);
        }
        

    } else {
        printf("Data Lost**********************************\r\n");
    }
    
     
    
}   

void tinyusb_cdc_line_state_changed_callback(int itf, cdcacm_event_t *event)
{
    int dtr = event->line_state_changed_data.dtr;
    int rst = event->line_state_changed_data.rts;
    ESP_LOGI(TAG_USB_CDC, "Line state changed! dtr:%d, rst:%d", dtr, rst);
}


void usbcdc_init(void)
{
	ESP_LOGI(TAG_USB_CDC, "USB initialization");
    tinyusb_config_t tusb_cfg = {}; // the configuration using default values
    ESP_ERROR_CHECK(tinyusb_driver_install(&tusb_cfg));

    tinyusb_config_cdcacm_t acm_cfg = {
        .usb_dev = TINYUSB_USBDEV_0,
        .cdc_port = TINYUSB_CDC_ACM_0,
        .rx_unread_buf_sz = 8192,//2560,//64,
        // .rx_unread_buf_sz = 64,

        .callback_rx = &tinyusb_cdc_rx_callback, // the first way to register a callback
        .callback_rx_wanted_char = NULL,
        .callback_line_state_changed = NULL,
        .callback_line_coding_changed = NULL
    };

    ESP_ERROR_CHECK(tusb_cdc_acm_init(&acm_cfg));
    /* the second way to register a callback */
    ESP_ERROR_CHECK(tinyusb_cdcacm_register_callback(
                        TINYUSB_CDC_ACM_0,
                        CDC_EVENT_LINE_STATE_CHANGED,
                        &tinyusb_cdc_line_state_changed_callback));
    ESP_LOGI(TAG_USB_CDC, "USB initialization DONE");
}

void HextoString(uint8_t* HexBuf, uint8_t* StringBuf, uint8_t number)
{
	unsigned char idx, Buf;

	for (idx = number; idx > 0; idx--)
	{
		Buf = (*HexBuf >> 4) & 0x0F;
		if (Buf < 10)  *StringBuf = Buf + 48;
		else  *StringBuf = Buf + 55;
		StringBuf++;
		Buf = *HexBuf & 0x0F;
		if (Buf < 10)  *StringBuf = Buf + 48;
		else  *StringBuf = Buf + 55;
		StringBuf++;
		HexBuf++;
	}
}

unsigned char string_to_number(unsigned char Buf)
{
	if ((Buf > 47) && (Buf < 58))// ASCII(十進制) 48 ~ 57 = 0 ~ 9
	{
		return Buf - 48;
	}
	else
	{
		return 0xFF;
	}
}

