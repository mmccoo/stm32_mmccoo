
#ifndef OLED_HH
#define OLED_HH

#include "stm32f1xx_hal.h"

// note that these are hard coded to use I2C1
void WriteOledCmd(unsigned char I2C_Command);
void WriteOledData(unsigned char I2C_Data);
void OLED_Fill(unsigned char fill_Data);
void InitOled(void);
void OLED_ShowStr(unsigned char x, unsigned char y, unsigned char ch[], unsigned char TextSize);

#endif
