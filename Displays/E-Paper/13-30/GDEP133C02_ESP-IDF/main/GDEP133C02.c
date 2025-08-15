
#define __GDEP133C02_C__

#include <stdio.h>
#include <string.h>
#include "GDEP133C02.h"
#include "pindefine.h"
#include "comm.h"
#include "status.h"

const unsigned char spiCsPin[2] = {
		SPI_CS0, SPI_CS1
};
const unsigned char PSR_V[2] = {
	0xDF, 0x69
};
const unsigned char PWR_V[6] = {
	0x0F, 0x00, 0x28, 0x2C, 0x28, 0x38
};
const unsigned char POF_V[1] = {
	0x00
};
const unsigned char DRF_V[1] = {
	0x01
};
const unsigned char CDI_V[1] = {
	0xF7
};
const unsigned char TCON_V[2] = {
	0x03, 0x03
};
const unsigned char TRES_V[4] = {
	0x04, 0xB0, 0x03, 0x20
};
const unsigned char CMD66_V[6] = {
	0x49, 0x55, 0x13, 0x5D, 0x05, 0x10
};
const unsigned char EN_BUF_V[1] = {
	0x07
};
const unsigned char CCSET_V[1] = {
	0x01
};
const unsigned char PWS_V[1] = {
	0x22
};
const unsigned char AN_TM_V[9] = {
	0xC0, 0x1C, 0x1C, 0xCC, 0xCC, 0xCC, 0x15, 0x15, 0x55
};

const unsigned char AGID_V[1] = {
	0x10
};

const unsigned char BTST_P_V[2] = {
	0xE8, 0x28
};
const unsigned char BOOST_VDDP_EN_V[1] = {
	0x01
};
const unsigned char BTST_N_V[2] = {
	0xE8, 0x28
};
const unsigned char BUCK_BOOST_VDDN_V[1] = {
	0x01
};
const unsigned char TFT_VCOM_POWER_V[1] = {
	0x02
};

char partialWindowUpdateStatus = DONE;

//================== GPIO Setting ====================================
void resetPin(unsigned int pinStatus)
{
	setGpioLevel(EPD_RST, pinStatus);
}

void setPinCsAll(unsigned int setLevel){
	unsigned char i;
	for(i=0;i<2;i++)
	{
		setGpioLevel(spiCsPin[i], setLevel);
	}
}

void setPinCs(unsigned char csNumber, unsigned int setLevel){
	setGpioLevel(spiCsPin[csNumber], setLevel);
}
void checkBusyHigh(void)// If BUSYN=0 then waiting
{
	while(!(getGpioLevel(EPD_BUSY)));
}

void checkBusyLow(void)// If BUSYN=1 then waiting
{
	while(getGpioLevel(EPD_BUSY));
}
//====================================================================

void epdHardwareReset(void)
{
	resetPin(GPIO_LOW);
	delayms(20);
	resetPin(GPIO_HIGH);
	delayms(20);
}

void writeEpd(unsigned char epdCommand, unsigned char *epdData, unsigned int epdDataLength)
{
	spiTransmit(epdCommand, epdData, epdDataLength);
}

void readEpd(unsigned char epdCommand, unsigned char *epdData, unsigned int epdDataLength)
{
	spiReceive(epdCommand, epdData, epdDataLength);
}

void writeEpdCommand(unsigned char epdCommand)
{
	spiTransmitCommand(epdCommand);
}

void writeEpdData(unsigned char *epdData, unsigned int epdDataLength)
{
	spiTransmitData(epdData, epdDataLength);
}

void initEPD(void)
{
	epdHardwareReset();
	checkBusyHigh();
	//checkBusyLow();

	setPinCs(0,GPIO_LOW);
	writeEpd(AN_TM, AN_TM_V, sizeof(AN_TM_V));
	setPinCsAll(GPIO_HIGH);

	setPinCsAll(GPIO_LOW);
	writeEpd(CMD66, CMD66_V, sizeof(CMD66_V));
	setPinCsAll(GPIO_HIGH);

	setPinCsAll(GPIO_LOW);
	writeEpd(PSR, PSR_V, sizeof(PSR_V));
	setPinCsAll(GPIO_HIGH);

	setPinCsAll(GPIO_LOW);
	writeEpd(CDI, CDI_V, sizeof(CDI_V));
	setPinCsAll(GPIO_HIGH);

	setPinCsAll(GPIO_LOW);
	writeEpd(TCON, TCON_V, sizeof(TCON_V));
	setPinCsAll(GPIO_HIGH);

	setPinCsAll(GPIO_LOW);
	writeEpd(AGID, AGID_V, sizeof(AGID_V));
	setPinCsAll(GPIO_HIGH);

	setPinCsAll(GPIO_LOW);
	writeEpd(PWS, PWS_V, sizeof(PWS_V));
	setPinCsAll(GPIO_HIGH);

	setPinCsAll(GPIO_LOW);
	writeEpd(CCSET, CCSET_V, sizeof(CCSET_V));
	setPinCsAll(GPIO_HIGH);

	setPinCsAll(GPIO_LOW);
	writeEpd(TRES, TRES_V, sizeof(TRES_V));
	setPinCsAll(GPIO_HIGH);

	setPinCs(0,GPIO_LOW);
	writeEpd(PWR, PWR_V, sizeof(PWR_V));
	setPinCsAll(GPIO_HIGH);

	setPinCs(0,GPIO_LOW);
	writeEpd(EN_BUF, EN_BUF_V, sizeof(EN_BUF_V));
	setPinCsAll(GPIO_HIGH);

	setPinCs(0,GPIO_LOW);
	writeEpd(BTST_P, BTST_P_V, sizeof(BTST_P_V));
	setPinCsAll(GPIO_HIGH);

	setPinCs(0,GPIO_LOW);
	writeEpd(BOOST_VDDP_EN, BOOST_VDDP_EN_V, sizeof(BOOST_VDDP_EN_V));
	setPinCsAll(GPIO_HIGH);

	setPinCs(0,GPIO_LOW);
	writeEpd(BTST_N, BTST_N_V, sizeof(BTST_N_V));
	setPinCsAll(GPIO_HIGH);

	setPinCs(0,GPIO_LOW);
	writeEpd(BUCK_BOOST_VDDN, BUCK_BOOST_VDDN_V, sizeof(BUCK_BOOST_VDDN_V));
	setPinCsAll(GPIO_HIGH);

	setPinCs(0,GPIO_LOW);
	writeEpd(TFT_VCOM_POWER, TFT_VCOM_POWER_V, sizeof(TFT_VCOM_POWER_V));
	setPinCsAll(GPIO_HIGH);

#if SHOW_LOG
    printf("initEPD() has been executed. \r\n");
#endif
}

unsigned char checkDriverICStatus(void)
{
	unsigned char csx, status = DONE;
	unsigned char dataBuf[3];

	for(csx=0 ; csx < 2 ; csx++)
	{
		memset(dataBuf, 0, sizeof(dataBuf));
		setPinCs(csx,GPIO_LOW);
		readEpd(0xF2, dataBuf, sizeof(dataBuf));
		setPinCs(csx,GPIO_HIGH);
#if SHOW_LOG
		printf("Driver IC [%d] = 0x%02X 0x%02X 0x%02X \r\n", csx, dataBuf[0], dataBuf[1], dataBuf[2]);
#endif
		if((dataBuf[0] & 0x01) == 0x01)
		{
#if SHOW_LOG
			printf("Driver IC [%d] is ready. \r\n",csx);
#endif
		}
		else
		{
#if SHOW_LOG
			printf("Driver IC [%d] did not reply. \r\n",csx);
#endif
			status = ERROR;
		}

	}

	return status;
}

void epdDisplay(void)
{

#if SHOW_LOG
    printf("Write PON \r\n");
#endif
	setPinCsAll(GPIO_LOW);
	writeEpdCommand(PON);
	checkBusyHigh();
	setPinCsAll(GPIO_HIGH);

#if SHOW_LOG
	printf("Write DRF \r\n");
#endif
	setPinCsAll(GPIO_LOW);
	delayms(30);
	writeEpd(DRF, DRF_V, sizeof(DRF_V));
	checkBusyHigh();
	setPinCsAll(GPIO_HIGH);

#if SHOW_LOG
	printf("Write POF \r\n");
#endif
	setPinCsAll(GPIO_LOW);
	writeEpd(POF, POF_V, sizeof(POF_V));
	checkBusyHigh();
	setPinCsAll(GPIO_HIGH);
#if SHOW_LOG
	printf("Display Done!! \r\n");
#endif
}

void epdDisplayColor(unsigned char colorSelect){

    unsigned long i;

    memset(epdImageDataBuffer,colorSelect,EPD_IMAGE_DATA_BUFFER);

    setPinCsAll(GPIO_LOW);
    writeEpdCommand(DTM);
    for(i = 0; i < 480000/EPD_IMAGE_DATA_BUFFER; i++){
        writeEpdData(epdImageDataBuffer, EPD_IMAGE_DATA_BUFFER);
    }
    writeEpdData(epdImageDataBuffer, 480000%EPD_IMAGE_DATA_BUFFER);
    setPinCsAll(GPIO_HIGH);

    epdDisplay();

#if SHOW_LOG
    printf("Display color complete. \r\n");
#endif
}


// Display screen parameters (already provided)
#define EPD_WIDTH 1200         // Total display width (pixels)
#define EPD_HEIGHT 1600        // Display height (pixels)
#define FIRST_PACK_SIZE 480000 // First data packet size (bytes)
#define TOTAL_IMAGE_SIZE 960000 // Total image data size (bytes)

void pic_display_test(const unsigned char *num)
{
    unsigned int Width, Width1, Height;
    // Calculate width and height using the same method as the second code
    Width = (EPD_WIDTH % 2 == 0) ? (EPD_WIDTH / 2) : (EPD_WIDTH / 2 + 1); // Width per section (pixels)
    Width1 = (Width % 2 == 0) ? (Width / 2) : (Width / 2 + 1);            // Width per section (bytes, assuming 8 bits per pixel)
    Height = EPD_HEIGHT;                                                   // Height (pixels)

    // Transfer data to the first section (main display)
    setPinCsAll(GPIO_HIGH);        // Deselect all
    setPinCs(0, 0);                // Select the first section (main display)
    writeEpdCommand(DTM);          // Send data transfer mode command
    for (unsigned int i = 0; i < Height; i++)
    {
        writeEpdData(num + i * Width, Width1); // Send the first half of each row's data
        vTaskDelay(pdMS_TO_TICKS(1));          // Delay 1ms to avoid hardware overload
    }
    setPinCsAll(GPIO_HIGH);        // Deselect

    // Transfer data to the second section (secondary display)
    setPinCs(1, 0);                // Select the second section (secondary display)
    writeEpdCommand(DTM);          // Send data transfer mode command
    for (unsigned int i = 0; i < Height; i++)
    {
        writeEpdData(num + i * Width + Width1, Width1); // Send the second half of each row's data
        vTaskDelay(pdMS_TO_TICKS(1));                   // Delay 1ms
    }
    setPinCsAll(GPIO_HIGH);        // Deselect

    // Refresh the display
    epdDisplay();                  // Trigger display
    vTaskDelay(pdMS_TO_TICKS(10)); // Delay 10ms to ensure refresh completion
    printf("Rendering completed\r\n"); // Print completion message
}

void draw_checkerboard() {
    // Calculate the display's width and height
    unsigned int Width = (EPD_WIDTH % 2 == 0) ? (EPD_WIDTH / 2) : (EPD_WIDTH / 2 + 1); // Width per section (pixels)
    unsigned int Width1 = (Width % 2 == 0) ? (Width / 2) : (Width / 2 + 1);            // Width per section (bytes)
    unsigned int Height = EPD_HEIGHT;                                                  // Height (pixels)

    // Verify the calculation results
    if (Width != 600 || Width1 != 300 || Height != 1600) {
        printf("Calculation error: Width=%u, Width1=%u, Height=%u\r\n", Width, Width1, Height);
        return;
    }

    // Allocate buffer (each byte stores two pixels)
    unsigned int buffer_size = TOTAL_IMAGE_SIZE; // Total bytes
    unsigned char *num = (unsigned char *)malloc(buffer_size * sizeof(unsigned char));
    if (num == NULL) {
        printf("Memory allocation failed!\r\n");
        return;
    }

    // Initialize the buffer
    memset(num, 0, buffer_size);

    // Define checkerboard colors (6 colors)
    const unsigned char colors[6] = {
        BLACK,  // Black
        WHITE,  // White
        YELLOW, // Yellow
        RED,    // Red
        BLUE,   // Blue
        GREEN   // Green
    };

    // Calculate the size of each checkerboard cell
    const int grid_cols = 6; // 6 columns
    const int grid_rows = 8; // 8 rows
    const int cell_width = EPD_WIDTH / grid_cols;   // Width of each cell: 1200 / 6 = 200 pixels
    const int cell_height = EPD_HEIGHT / grid_rows; // Height of each cell: 1600 / 8 = 200 pixels

    // Fill pixel data (horizontal scan, from right to left, top to bottom)
    for (unsigned int y = 0; y < Height; y++) {
        for (unsigned int x = 0; x < EPD_WIDTH; x += 2) { // Process two pixels at a time
            // Calculate the cell position of the current pixel
            int grid_x = x / cell_width; // Column index
            int grid_y = y / cell_height; // Row index

            // Calculate color index (cycle through colors based on cell position for a staggered pattern)
            int color_index = (grid_x + grid_y) % 6; // Staggered distribution
            unsigned char color1 = colors[color_index]; // Color of the first pixel

            // Second pixel (if x+1 is in the same cell, the color is the same)
            int grid_x2 = (x + 1) / cell_width;
            int color_index2 = (grid_x2 + grid_y) % 6; // Staggered distribution
            unsigned char color2 = colors[color_index2]; // Color of the second pixel

            // Calculate buffer index (horizontal scan, from right to left)
            int new_x = (EPD_WIDTH - 2 - x); // From right to left
            int new_index = (y * Width) + (new_x / 2);

            // Combine two pixels into one byte
            num[new_index] = (color1 << 4) | color2;
        }
    }

    // Call pic_display_test to display
    pic_display_test(num);

    // Free the buffer
    free(num);
}



void epdDisplayColorBar(void)
{
	unsigned long i;

	setPinCsAll(GPIO_LOW);
	writeEpdCommand(DTM);

// BLACK
	memset(epdImageDataBuffer,BLACK,EPD_IMAGE_DATA_BUFFER);
	for(i = 0; i < 80000/EPD_IMAGE_DATA_BUFFER; i++){
		writeEpdData(epdImageDataBuffer, EPD_IMAGE_DATA_BUFFER);
	}
	writeEpdData(epdImageDataBuffer, 80000%EPD_IMAGE_DATA_BUFFER);
// WHITE
	memset(epdImageDataBuffer,WHITE,EPD_IMAGE_DATA_BUFFER);
	for(i = 0; i < 80000/EPD_IMAGE_DATA_BUFFER; i++){
		writeEpdData(epdImageDataBuffer, EPD_IMAGE_DATA_BUFFER);
	}
	writeEpdData(epdImageDataBuffer, 80000%EPD_IMAGE_DATA_BUFFER);
// YELLOW
	memset(epdImageDataBuffer,YELLOW,EPD_IMAGE_DATA_BUFFER);
	for(i = 0; i < 80000/EPD_IMAGE_DATA_BUFFER; i++){
		writeEpdData(epdImageDataBuffer, EPD_IMAGE_DATA_BUFFER);
	}
	writeEpdData(epdImageDataBuffer, 80000%EPD_IMAGE_DATA_BUFFER);
// RED
	memset(epdImageDataBuffer,RED,EPD_IMAGE_DATA_BUFFER);
	for(i = 0; i < 80000/EPD_IMAGE_DATA_BUFFER; i++){
		writeEpdData(epdImageDataBuffer, EPD_IMAGE_DATA_BUFFER);
	}
	writeEpdData(epdImageDataBuffer, 80000%EPD_IMAGE_DATA_BUFFER);
// BLUE
	memset(epdImageDataBuffer,BLUE,EPD_IMAGE_DATA_BUFFER);
	for(i = 0; i < 80000/EPD_IMAGE_DATA_BUFFER; i++){
		writeEpdData(epdImageDataBuffer, EPD_IMAGE_DATA_BUFFER);
	}
	writeEpdData(epdImageDataBuffer, 80000%EPD_IMAGE_DATA_BUFFER);
// GREEN
	memset(epdImageDataBuffer,GREEN,EPD_IMAGE_DATA_BUFFER);
	for(i = 0; i < 80000/EPD_IMAGE_DATA_BUFFER; i++){
		writeEpdData(epdImageDataBuffer, EPD_IMAGE_DATA_BUFFER);
	}
	writeEpdData(epdImageDataBuffer, 80000%EPD_IMAGE_DATA_BUFFER);
	setPinCsAll(GPIO_HIGH);

	epdDisplay();

#if SHOW_LOG
	printf("Display color bar complete. \r\n");
#endif
}





void writeEpdImage(unsigned char csx, unsigned char const *imageData, unsigned long imageDataLength)
{

	setPinCs(csx,GPIO_LOW);
	spiTransmitLargeData(DTM, imageData, imageDataLength);
	setPinCs(csx,GPIO_HIGH);

#if SHOW_LOG
	printf("Writing data is completed. \r\n");
#endif

}

char partialWindowUpdateWithImageData(unsigned char csx, unsigned char const *imageData, unsigned long imageDataLength,
     unsigned int xStart, unsigned int yStart, unsigned int xPixel, unsigned int yLine, unsigned char epdDisplayEnable)
{
	unsigned char status = DONE;
	unsigned int HRST, HRED, VRST, VRED;
	unsigned char partialWindowData[9];

	HRST = xStart * 2;
	HRED = (xStart + xPixel) * 2 - 1; // The range is 0 ~ 1199
	VRST = yStart / 2;
	VRED = (yStart + yLine) / 2 - 1; // The range is 0 ~ 799

#if SHOW_LOG
	printf("csx = %d ; HRST = %d ; HRED = %d ; VRST = %d ; VRED = %d \r\n",csx, HRST, HRED, VRST, VRED);
#endif

	// HRST[10:0] = 8n (n = 0,1,2…)
	if (HRST % 8 != 0){
		status = -1;
#if SHOW_LOG
		printf("status = -1 ; There is a problem with xStart. \r\n");
#endif
	}
	// HRED[10:0] = 8m+3 (m = 4,5,6…)
	else if ((HRED - 7) % 8 != 0) {
		status = -2;
#if SHOW_LOG
		printf("status = -2 ; There is a problem with xPixel. \r\n");
#endif
	}
	//  xStart <= 584 ; xPixel <= 600
	else if ((xStart > 584) | (xPixel > 600)) {
		status = -3;
#if SHOW_LOG
		printf("status = -3 ; xStart or xPixel is over range. \r\n");
#endif
	}
	// HRED - HRST + 1 >= 32 & HRED + 1 <= 1200
	else if ((HRED - HRST + 1 < 32) | (HRED + 1 > 1200)){
		status = -4;
#if SHOW_LOG
		printf("status = -4 ; There is a problem with xStart & xPixel. \r\n");
#endif
	}
	else if ((yStart + yLine) % 2 != 0){
		status = -5;
#if SHOW_LOG
		printf("status = -5 ; yStart + yLine must be an even number. \r\n");
#endif
	}
	// yStart <= 1596 ; yLine <= 1600
	else if ((yStart > 1596) | (yLine > 1600)) {
		status = -6;
#if SHOW_LOG
		printf("status = -6 ; yStart or yLine is over range. \r\n");
#endif
	}
	//VRST - VRED + 1 > 0 & VRED + 1 <= 800
	else if (((int)(VRED - VRST) + 1 <= 0) | (VRED + 1 > 800)){
		status = -7;
#if SHOW_LOG
		printf("status = -7 ; There is a problem with yStart & yLine. \r\n");
#endif
	}
	else if(csx > 1){
		status = -8;
#if SHOW_LOG
		printf("status = -8 ; There is a problem with cxs. \r\n");
#endif
	}
	else
	{
		memset(partialWindowData,0,sizeof(partialWindowData));
		partialWindowData[0] = (unsigned char)(HRST >> 8);
		partialWindowData[1] = (unsigned char)(HRST);
		partialWindowData[2] = (unsigned char)(HRED >> 8);
		partialWindowData[3] = (unsigned char)(HRED);
		partialWindowData[4] = (unsigned char)(VRST >> 8);
		partialWindowData[5] = (unsigned char)(VRST);
		partialWindowData[6] = (unsigned char)(VRED >> 8);
		partialWindowData[7] = (unsigned char)(VRED);
		partialWindowData[8] = PTLW_ENABLE;

		setPinCs(csx,GPIO_LOW);
		writeEpd(CMD66, CMD66_V, sizeof(CMD66_V));
		setPinCs(csx,GPIO_HIGH);

		setPinCs(csx,GPIO_LOW);
		writeEpd(PTLW, partialWindowData, sizeof(partialWindowData));
		setPinCs(csx,GPIO_HIGH);

		setPinCs(csx,GPIO_LOW);
		spiTransmitLargeData(DTM, imageData, imageDataLength);
		setPinCs(csx,GPIO_HIGH);

	}

	if(status != DONE)
	{
		partialWindowUpdateStatus = ERROR;
#if SHOW_LOG
		printf("partialWindowUpdateStatus = ERROR \r\n");
#endif
	}

	if(epdDisplayEnable)
	{
		if(partialWindowUpdateStatus == DONE) epdDisplay();

		delayms(300);

		//========================= Turn off PTLW =========================
		memset(partialWindowData,0,sizeof(partialWindowData));
		partialWindowData[8] = PTLW_DISABLE;
		partialWindowUpdateStatus = DONE;

		setPinCsAll(GPIO_LOW);
		writeEpd(PTLW, partialWindowData, sizeof(partialWindowData));
		setPinCsAll(GPIO_HIGH);
		//=================================================================
	}

	return status;
}

char  partialWindowUpdateWithoutImageData(unsigned char csx, unsigned int xStart, unsigned int yStart,
	 unsigned int xPixel, unsigned int yLine, unsigned char epdDisplayEnable)
{
	unsigned char status = DONE;
	unsigned int HRST, HRED, VRST, VRED;
	unsigned char partialWindowData[9];

	HRST = xStart * 2;
	HRED = (xStart + xPixel) * 2 - 1; // The range is 0 ~ 1199
	VRST = yStart / 2;
	VRED = (yStart + yLine) / 2 - 1; // The range is 0 ~ 799

#if SHOW_LOG
	printf("csx = %d ; HRST = %d ; HRED = %d ; VRST = %d ; VRED = %d \r\n",csx, HRST, HRED, VRST, VRED);
#endif

	// HRST[10:0] = 8n (n = 0,1,2…)
	if (HRST % 8 != 0){
		status = -1;
#if SHOW_LOG
		printf("status = -1 ; There is a problem with xStart. \r\n");
#endif
	}
	// HRED[10:0] = 8m+3 (m = 4,5,6…)
	else if ((HRED - 7) % 8 != 0) {
		status = -2;
#if SHOW_LOG
		printf("status = -2 ; There is a problem with xPixel. \r\n");
#endif
	}
	//  xStart <= 584 ; xPixel <= 600
	else if ((xStart > 584) | (xPixel > 600)) {
		status = -3;
#if SHOW_LOG
		printf("status = -3 ; xStart or xPixel is over range. \r\n");
#endif
	}
	// HRED - HRST + 1 >= 32 & HRED + 1 <= 1200
	else if ((HRED - HRST + 1 < 32) | (HRED + 1 > 1200)){
		status = -4;
#if SHOW_LOG
		printf("status = -4 ; There is a problem with xStart & xPixel. \r\n");
#endif
	}
	else if ((yStart + yLine) % 2 != 0){
		status = -5;
#if SHOW_LOG
		printf("status = -5 ; yStart + yLine must be an even number. \r\n");
#endif
	}
	// yStart <= 1596 ; yLine <= 1600
	else if ((yStart > 1596) | (yLine > 1600)) {
		status = -6;
#if SHOW_LOG
		printf("status = -6 ; yStart or yLine is over range. \r\n");
#endif
	}
	//VRST - VRED + 1 > 0 & VRED + 1 <= 800
	else if (((int)(VRED - VRST) + 1 <= 0) | (VRED + 1 > 800)){
		status = -7;
#if SHOW_LOG
		printf("status = -7 ; There is a problem with yStart & yLine. \r\n");
#endif
	}
	else if(csx > 1){
		status = -8;
#if SHOW_LOG
		printf("status = -8 ; There is a problem with cxs. \r\n");
#endif
	}
	else
	{
		memset(partialWindowData,0,sizeof(partialWindowData));
		partialWindowData[0] = (unsigned char)(HRST >> 8);
		partialWindowData[1] = (unsigned char)(HRST);
		partialWindowData[2] = (unsigned char)(HRED >> 8);
		partialWindowData[3] = (unsigned char)(HRED);
		partialWindowData[4] = (unsigned char)(VRST >> 8);
		partialWindowData[5] = (unsigned char)(VRST);
		partialWindowData[6] = (unsigned char)(VRED >> 8);
		partialWindowData[7] = (unsigned char)(VRED);
		partialWindowData[8] = PTLW_ENABLE;

		setPinCs(csx,GPIO_LOW);
		writeEpd(CMD66, CMD66_V, sizeof(CMD66_V));
		setPinCs(csx,GPIO_HIGH);

		setPinCs(csx,GPIO_LOW);
		writeEpd(PTLW, partialWindowData, sizeof(partialWindowData));
		setPinCs(csx,GPIO_HIGH);
	}

	if(status != DONE)
	{
		partialWindowUpdateStatus = ERROR;
#if SHOW_LOG
		printf("partialWindowUpdateStatus = ERROR \r\n");
#endif
	}

	if(epdDisplayEnable)
	{
		if(partialWindowUpdateStatus == DONE) epdDisplay();

		delayms(300);

		//========================= Turn off PTLW =========================
		memset(partialWindowData,0,sizeof(partialWindowData));
		partialWindowData[8] = PTLW_DISABLE;
		partialWindowUpdateStatus = DONE;

		setPinCsAll(GPIO_LOW);
		writeEpd(PTLW, partialWindowData, sizeof(partialWindowData));
		setPinCsAll(GPIO_HIGH);
		//=================================================================
	}

	return status;
}


