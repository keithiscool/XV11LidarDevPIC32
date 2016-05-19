#include "stm32f4xx.h"
#include "Serial.h"
#include "math.h"
#include "string.h"

#include "stm32f429i_discovery_lcd.h"
#include "stm32f429i_discovery_ioe.h"

#define XV11_PACKAGE_LENGTH	22
#define XV11_START_BYTE	0xFA

uint8_t XV11_Package[22];

uint16_t Distance[360];
uint32_t PrintTimestamp;
uint32_t DisplayTimestamp;

uint16_t PackageChecksum(uint8_t * packagePointer);

static void LCD_Config(void);
static void LCD_AF_GPIOConfig(void);
static void delay(__IO uint32_t nCount);

const float RAD = (M_TWOPI / 360.0f);
uint16_t deg;
float radians, displayDistance;
uint16_t x[360], y[360];
uint16_t degSelected[2];
uint8_t scanEnabled = 1;
uint8_t pointSelect = 0;
float sqrtDist;
float tempCalc, tempCalc2, tempCalc3;

uint8_t textBuffer[30];

static TP_STATE* TP_State;
static void TP_Config(void);

int main(void)
{
	Serial_Init();
	SyncUp();

	for (deg = 0; deg < 360; deg++) { // reset pixels
		x[deg] = 120;
		y[deg] = 120;
	}

    while(1)
    {
    	if (scanEnabled == 1) {
    		while (Serial_Buffer_Count() < XV11_PACKAGE_LENGTH);
    		LoadPackage(XV11_Package);
    		if (XV11_Package[0] != XV11_START_BYTE) {
    			SyncUp();
    		} else {
    			ParsePackage(XV11_Package);
    		}
    	}
    }
}


void SyncUp(void)
{
	uint8_t i;
	int16_t ch = 0;
	int16_t ch2 = 0;

	Serial_Buffer_Clear();

	while (ch != XV11_START_BYTE) {
		while (Serial_Buffer_Get() != XV11_START_BYTE);

		i = XV11_PACKAGE_LENGTH;
		while (i > 0) {
			ch = Serial_Buffer_Get();
			if (ch >= 0) i--;
		}

		// read the rest
		i = XV11_PACKAGE_LENGTH - 1;
		while (i > 0) {
			ch2 = Serial_Buffer_Get();
			if (ch2 >= 0) i--;
		}
	}
}

void LoadPackage(uint8_t * packagePointer)
{
	uint8_t i = 0;
	uint8_t ch;
	int16_t temp;

	while (i < XV11_PACKAGE_LENGTH) {
		temp = Serial_Buffer_Get();
		if (temp >= 0) {
			packagePointer[i] = temp;
			i++;
		} else {
			temp = 0;
		}
	}
}

uint16_t GoodReadings = 0, BadReadings = 0;
uint16_t AnglesCovered = 0;
void ParsePackage(uint8_t * packagePointer)
{
	uint16_t i;
	uint16_t Index;
	uint8_t Speed;
	uint8_t InvalidFlag[4];
	uint8_t WarningFlag[4];
	uint16_t Checksum, ChecksumCalculated;

	Checksum = ((uint16_t)packagePointer[21] << 8) | packagePointer[20];
	ChecksumCalculated = PackageChecksum(packagePointer);
	if (Checksum != ChecksumCalculated) {
		BadReadings += 4;
	}

	Index = (packagePointer[1] - 0xA0) * 4;
	Speed = ((uint16_t)packagePointer[3] << 8) | packagePointer[2];
	InvalidFlag[0] = (packagePointer[5] & 0x80) >> 7;
	InvalidFlag[1] = (packagePointer[9] & 0x80) >> 7;
	InvalidFlag[2] = (packagePointer[13] & 0x80) >> 7;
	InvalidFlag[3] = (packagePointer[17] & 0x80) >> 7;
	WarningFlag[0] = (packagePointer[5] & 0x40) >> 6;
	WarningFlag[1] = (packagePointer[9] & 0x40) >> 6;
	WarningFlag[2] = (packagePointer[13] & 0x40) >> 6;
	WarningFlag[3] = (packagePointer[17] & 0x40) >> 6;

	if (Index == 0) {
		AnglesCovered = 0;
		for (i = 0; i < 360; i++) {
			if (Distance[i] > 0) AnglesCovered++;
		}

		GoodReadings = 0;
		BadReadings = 0;
	}

	for (i = 0; i < 4; i++) {
		if (!InvalidFlag[i])
		{
			Distance[Index+i] = packagePointer[4+(i*4)] | ((uint16_t)(packagePointer[5+(i*4)] & 0x3F) << 8);
			GoodReadings++;
		} else {
			Distance[Index+i] = 0;
			BadReadings++;
		}
	}

}

uint16_t PackageChecksum(uint8_t * packagePointer)
{
	uint8_t i;
	uint16_t data[10];
	uint16_t checksum;
	uint32_t chk32;

	// group the data by word, little-endian
	for (i = 0; i < 10; i++) {
		data[i] = packagePointer[2*i] | (((uint16_t)packagePointer[2*i+1]) << 8);
	}

	// compute the checksum on 32 bits
	chk32 = 0;
	for (i = 0; i < 10; i++) {
    	chk32 = (chk32 << 1) + data[i];
	}

   // return a value wrapped around on 15bits, and truncated to still fit into 15 bits
   checksum = (chk32 & 0x7FFF) + ( chk32 >> 15 ); // wrap around to fit into 15 bits
   checksum = checksum & 0x7FFF; // truncate to 15 bits
}


void DrawDistanceMap(void)
{
	// UP TO 3500 i distance = 3.5m
	radians = 0;
	for (deg = 0; deg < 360; deg++) {
		LCD_SetTextColor(LCD_COLOR_BLACK);
		LCD_DrawLine(x[deg], y[deg], 1, LCD_DIR_HORIZONTAL); // PutPixel(x, y);
		LCD_DrawLine(x[deg]+1, y[deg], 1, LCD_DIR_HORIZONTAL); // PutPixel(x, y);
		LCD_DrawLine(x[deg], y[deg]+1, 1, LCD_DIR_HORIZONTAL); // PutPixel(x, y);
		LCD_DrawLine(x[deg]+1, y[deg]+1, 1, LCD_DIR_HORIZONTAL); // PutPixel(x, y);

		if (Distance[deg] < 3000 && Distance[deg] > 0) {
			displayDistance = Distance[deg] / 25.0f;
			LCD_SetTextColor(LCD_COLOR_WHITE);
			x[deg] = (cos(M_PI_2-radians) * displayDistance) + 120;
			y[deg] = (sin(M_PI_2-radians) * displayDistance) + 120;
			LCD_DrawLine(x[deg], y[deg], 1, LCD_DIR_HORIZONTAL); // PutPixel(x, y);
			LCD_DrawLine(x[deg]+1, y[deg], 1, LCD_DIR_HORIZONTAL); // PutPixel(x, y);
			LCD_DrawLine(x[deg], y[deg]+1, 1, LCD_DIR_HORIZONTAL); // PutPixel(x, y);
			LCD_DrawLine(x[deg]+1, y[deg]+1, 1, LCD_DIR_HORIZONTAL); // PutPixel(x, y);
		}
		radians += RAD;
	}
	LCD_SetTextColor(LCD_COLOR_BLUE2);
	LCD_DrawFullCircle(120, 120, 6);
}
