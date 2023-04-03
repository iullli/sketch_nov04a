#include "ssd1306.h"
#include <stdio.h>

unsigned char ssd1306_buffer[1136];

unsigned char ssd1306_write = 0b01111000; // i2c address
unsigned char ssd1306_read = 0b01111001; // i2c addres

//init

void I2C_Init(void)
{
        // Activate the pull-up resistors for SDA/SCL(TWI).
                                       // Otherwise, you need to attach external pull-ups

	TWSR = 0x00;
	TWBR  = ((F_CPU/ 100000) - 16) / (2*pow(4,(TWSR&((1<<TWPS0)|(1<<TWPS1)))));
	

    
}

//start
void I2C_Start(void)
{
 TWCR = (1 << TWSTA) | (1 << TWEN) | (1 << TWINT) ;
 while(!(TWCR & (1<<TWINT)));

//  TWDR = 0x3C;

//  TWCR =(1 << TWEN) | (1 << TWINT) ;
// while(!(TWCR & (1<<TWINT)));
}

void I2C_Repeted_Start(char read_address)
{
 TWCR = (1 << TWSTA) | (1 << TWEN) | (1 << TWINT) ;
 while(!(TWCR & (1<<TWINT)));

 TWDR = read_address;
 TWCR = (1 << TWEN) | (1 << TWINT) ;
while(!(TWCR & (1<<TWINT)));
}




//stop
void I2C_Stop(void)
{
    TWCR = (1 << TWINT) | (1 << TWSTO) | (1 << TWEN);
    while(!(TWCR & (1<<TWSTO)));
}





/// write
void I2C_WRITE(unsigned char data)
{

// TWDR = 0x3C;
// TWCR = (1<<TWINT)|(1<<TWEN);
// while(!(TWCR&(1<<TWINT)));

TWDR = data;
TWCR = (1<<TWINT)|(1<<TWEN);
while(!(TWCR & (1<<TWINT)));

}


char I2C_Read_Ack(void)
{
TWCR = (1 << TWINT) |  (1 << TWEN);
while(!(TWCR & (1<<TWINT)));
return TWDR;
}


void ssd1306_setup()
{
    // give a little delay for the ssd1306 to power up
    delay(500);
     ssd1306_command(SSD1306_DISPLAYOFF);
    ssd1306_command(SSD1306_SETDISPLAYCLOCKDIV);
    ssd1306_command(0x80);
    ssd1306_command(SSD1306_SETMULTIPLEX);
    ssd1306_command(0x3F); // height-1 = 31
    ssd1306_command(SSD1306_SETDISPLAYOFFSET);
    ssd1306_command(0x00);
    ssd1306_command(SSD1306_SETSTARTLINE);
    ssd1306_command(SSD1306_CHARGEPUMP);
    ssd1306_command(0x14);
    ssd1306_command(SSD1306_MEMORYMODE);
    ssd1306_command(0x00);
    ssd1306_command(SSD1306_SEGREMAP | 0x1);
    ssd1306_command(SSD1306_COMSCANDEC);
    ssd1306_command(SSD1306_SETCOMPINS);
    ssd1306_command(0x12);
    ssd1306_command(SSD1306_SETCONTRAST);
    ssd1306_command(0xCF);
    ssd1306_command(SSD1306_SETPRECHARGE);
    ssd1306_command(0xF1);
    ssd1306_command(SSD1306_SETVCOMDETECT);
    ssd1306_command(0x40);
    ssd1306_command(SSD1306_DISPLAYON);
    ssd1306_clear();
    ssd1306_update();
}

void ssd1306_command(uint8_t c)
{
  I2C_Start();
  I2C_WRITE(ssd1306_write);
  I2C_WRITE(0x00);
  I2C_WRITE(c);
  I2C_Stop();

}

void ssd1306_drawPixel(unsigned char x, unsigned char y, unsigned char color)
{
    if ((x < 0) || (x >= 128) || (y < 0) || (y >= 64))
    {
        return;
    }

    if (color == 1)
    {
        ssd1306_buffer[x + (y / 8) * 128] |= (1 << (y & 7));
    }
    else
    {
        ssd1306_buffer[x + (y / 8) * 128] &= ~(1 << (y & 7));
    }
}

void ssd1306_update()
{
   
    ssd1306_command(SSD1306_PAGEADDR);
    ssd1306_command(0);
    ssd1306_command(0x07);
    ssd1306_command(SSD1306_COLUMNADDR);
    ssd1306_command(0);
    ssd1306_command(128-1); // Width

    unsigned short count = 1024;          // WIDTH * ((HEIGHT + 7) / 8)
    unsigned char *ptr = ssd1306_buffer; // first address of the pixel buffer

    I2C_Start();
    I2C_WRITE(ssd1306_write);
    I2C_WRITE(0x40);
    // send every pixel
    while (count--)
    {
       I2C_WRITE(*ptr++);
    }
  I2C_Stop();
}



void ssd1306_clear()
{
    memset(ssd1306_buffer, 0, 1136); // make every bit a 0, memset in string.h
}

void SSD1306_DrawBitmap(int16_t x, int16_t y, const unsigned char* bitmap, int16_t w, int16_t h, uint16_t color)
{

    int16_t byteWidth = (w + 7) / 8; // Bitmap scanline pad = whole byte
    uint8_t byte = 0;

    for(int16_t j=0; j<h; j++, y++)
    {
        for(int16_t i=0; i<w; i++)
        {
            if(i & 7)
            {
               byte <<= 1;
            }
            else
            {
               byte = (*(const unsigned char *)(&bitmap[j * byteWidth + i / 8]));
            }
            if(byte & 0x80) ssd1306_drawPixel(x+i, y, color);
        }
    }
}

void ssd1306_DrawText(int16_t x, int16_t y, char i)
{
char str[50]= {0};
int s=0;
sprintf(str,"%d",i);
while (str[s]!=0){
    ssd1306_Drawletter(x,y,str[s]);
    s++;
}
s=0;

}

void ssd1306_Drawletter(int8_t x, int8_t y, uint8_t letter)
{
    uint8_t call = 0 ;
    uint8_t diff = letter - 0x20;
       for(int j = 0; j<= 4; j++)
    {
             for(int k = 0; k<=7;k++)
        {
     call = (ASCI[diff][j]>>k)&0x01;
     if(call != 0)
     ssd1306_drawPixel(j+x,k+y,1);
     else
     ssd1306_drawPixel(j+x,k+y,0);
    
    }
}
}

void ssd1306_Strings(int8_t x, int8_t y, char text[])
{
    uint8_t i = 0;
while(text[i] != 0)
{   
    x=x+5;
    ssd1306_Drawletter(x+i,y,text[i]);
    i++;
}
i=0;
}


