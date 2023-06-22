#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include <avr/io.h>
#include <avr/interrupt.h>
#include "font.h"
#include <Arduino.h>


#define SET      1
#define RESET    0

#ifndef SSD1306_H
#define SSD1306_H

#endif


#define SSD1306_MEMORYMODE          0x20 
#define SSD1306_COLUMNADDR          0x21 
#define SSD1306_PAGEADDR            0x22 
#define SSD1306_SETCONTRAST         0x81 
#define SSD1306_CHARGEPUMP          0x8D 
#define SSD1306_SEGREMAP            0xA0 
#define SSD1306_DISPLAYALLON_RESUME 0xA4 
#define SSD1306_NORMALDISPLAY       0xA6 
#define SSD1306_INVERTDISPLAY       0xA7 
#define SSD1306_SETMULTIPLEX        0xA8 
#define SSD1306_DISPLAYOFF          0xAE 
#define SSD1306_DISPLAYON           0xAF 
#define SSD1306_COMSCANDEC          0xC8 
#define SSD1306_SETDISPLAYOFFSET    0xD3 
#define SSD1306_SETDISPLAYCLOCKDIV  0xD5 
#define SSD1306_SETPRECHARGE        0xD9 
#define SSD1306_SETCOMPINS          0xDA 
#define SSD1306_SETVCOMDETECT       0xDB 
#define SSD1306_SETSTARTLINE        0x40 
#define SSD1306_DEACTIVATE_SCROLL   0x2E ///< Stop scroll
#define SSD1306_FORCEDISPLAYON      0xA5


void I2C_Init(void);
void I2C_Start(void);
void I2C_Repeted_Start(char read_address);
void I2C_Stop(void);
void I2C_WRITE(unsigned char data);
char I2C_Read_Ack(void);

void ssd1306_setup(void);
void ssd1306_command(uint8_t c);
void ssd1306_drawPixel(unsigned char x, unsigned char y, unsigned char color);
void ssd1306_update(void);
void ssd1306_clear(void);
void SSD1306_DrawBitmap(int16_t x, int16_t y, const unsigned char* bitmap, int16_t w, int16_t h, uint16_t color);
void ssd1306_Drawletter(int8_t x, int8_t y, uint8_t letter, uint8_t color);
void ssd1306_Strings(int8_t x, int8_t y, char text[], uint8_t color);
void ssd1306_drawrectagle(int8_t x, int8_t y, int8_t color, int8_t lenght , int8_t wide);
void ssd1306_drawlinev(int8_t x, int8_t y, int8_t color, int8_t lenght);
void ssd1306_drawlineh(int8_t x, int8_t y, int8_t color, int8_t lenght);
void Init_Userinterface(uint8_t);
void Init_Chargeinterface(void);
uint8_t Init_BatterySetup(void);
void Init_Test(void);













