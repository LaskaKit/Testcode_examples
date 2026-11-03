#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <sys/unistd.h>
#include <sys/stat.h>
#include <assert.h>
#include "esp_partition.h"
#include "esp_err.h"
#include "esp_log.h"
#include "esp_spiffs.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_system.h"
#include "usbcdc.h"
#include "esp_vfs_fat.h"
#include "GDEP133C02.h"
#include "comm.h"
#include "pindefine.h"
#include "status.h"
#include "image.h"
void app_main(void)
{

//=============   EPD Initial ========================


   	//=============   Control Signal Setting ========================
	initialGpio();
	initialSpi();
	setGpioLevel(LOAD_SW,GPIO_HIGH);
    epdHardwareReset();
	setPinCsAll(GPIO_HIGH);
	//===============================================================

	//epdStatus = checkDriverICStatus();

    initEPD();
	epdDisplayColorBar();
	delayms(2000);

	initEPD();
	pic_display_test(gImage);
	delayms(2000);


	initEPD();
	draw_checkerboard();
	delayms(2000);

	initEPD();
	epdDisplayColor(WHITE);
	delayms(2000);
//====================================================

    while (1);
                          
} 