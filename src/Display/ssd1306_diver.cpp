#include "ssd1306.h"
#include <stdio.h>


unsigned char ssd1306_buffer[1136];

unsigned char ssd1306_write = 0b01111000; // i2c address

//init

void I2C_Init(void)
{
	TWSR = 0x00;
	TWBR  = ((F_CPU/ 100000) - 16) / (2*pow(4,(TWSR&((1<<TWPS0)|(1<<TWPS1)))));
}

//start
void I2C_Start(void)
{
 TWCR = (1 << TWSTA) | (1 << TWEN) | (1 << TWINT) ;
 while(!(TWCR & (1<<TWINT)));

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


void ssd1306_Drawletter(int8_t x, int8_t y, uint8_t letter, uint8_t color)
{
    uint8_t call = 0 ;
    uint8_t diff = letter - 0x20;
       for(int j = 0; j<= 4; j++)
    {
             for(int k = 0; k<=7;k++)
        {
     call = (ASCI[diff][j]>>k)&0x01;
     if(call != 0)
     ssd1306_drawPixel(j+x,k+y,color);
     else
     ssd1306_drawPixel(j+x,k+y,!color);
    
    }
}
}

void ssd1306_Strings(int8_t x, int8_t y, char text[], uint8_t color)
{
    uint8_t i = 0;
while(text[i] != 0)
{   
    x=x+5;
    ssd1306_Drawletter(x+i,y,text[i],color);
    i++;
}
i=0;
}

void ssd1306_drawrectagle(int8_t x, int8_t y, int8_t color, int8_t lenght , int8_t wide)
{
    int i=0;
    int j=0;

for(j = y; j <= wide + y;j++)
{
for(i = x;i <= lenght + x;i++)
{
    ssd1306_drawPixel(i,j,1);
}
   ssd1306_drawPixel(i,j,1);
}
}


void ssd1306_drawlinev(int8_t x, int8_t y, int8_t color, int8_t lenght)
{
int i=0;
for(i = y;i <= lenght + y;i++)
{
    ssd1306_drawPixel(x,i+y,1);
}
}

void ssd1306_drawlineh(int8_t x, int8_t y, int8_t color, int8_t lenght)
{
int i=0;
for(i = x;i <= lenght + x;i++)
{
    ssd1306_drawPixel(x+i,y,1);
}
}

void Init_Userinterface(uint8_t selection)
{
switch(selection){
    case 0:
ssd1306_clear();
ssd1306_drawrectagle(5,18,1,120,10);
ssd1306_Strings(45,1,"Menu",1);
ssd1306_Strings(45,20,"Start",0);
ssd1306_Strings(38,35,"Battery ",1);
ssd1306_Strings(45,50,"Test",1);
ssd1306_update();
   break;
    case 1:
ssd1306_clear();    
ssd1306_drawrectagle(5,33,1,120,10);
ssd1306_Strings(45,1,"Menu",1);
ssd1306_Strings(45,20,"Start",1);
ssd1306_Strings(38,35,"Battery ",0);
ssd1306_Strings(45,50,"Test",1);
ssd1306_update();
   break;
       case 2:
ssd1306_clear();    
ssd1306_drawrectagle(5,48,1,120,10);
ssd1306_Strings(45,1,"Menu",1);
ssd1306_Strings(45,20,"Start",1);
ssd1306_Strings(38,35,"Battery ",1);
ssd1306_Strings(45,50,"Test",0);
ssd1306_update();
   break;
}
ssd1306_update();

}

void Init_Chargeinterface()
{
ssd1306_clear();
ssd1306_update();
ssd1306_drawrectagle(4,20,1,17,30);
ssd1306_drawrectagle(32,20,1,17,30);
ssd1306_drawrectagle(60,20,1,17,30);
ssd1306_Strings(110,1,"+",1);
ssd1306_Strings(110,60,"-",1);
ssd1306_Strings(59,1,"A",1);
ssd1306_drawlinev(13,2, 1, 20);
ssd1306_drawlineh(7,3,1,20);
ssd1306_drawlineh(35,3,1,37);
ssd1306_drawlineh(39,50,1,11);
ssd1306_drawlineh(45,63,1,20);
ssd1306_drawlinev(90,25,1,12);
ssd1306_drawlineh(22,15,1,20);
ssd1306_drawlinev(43,8,1,10);
ssd1306_drawlinev(65,8,1,10);
ssd1306_drawlineh(11,48,1,14);
}

void Init_Test()
{
    ssd1306_clear();
    ssd1306_drawrectagle(4, 18, 1, 120, 7);
    ssd1306_Strings(1, 18, "Cell1", 0);
    ssd1306_Strings(1, 31, "D:", 1);
    ssd1306_Strings(1, 52, "V: ", 1);
    ssd1306_Strings(43, 18, "Cell2", 0);
     ssd1306_Strings(43, 31, "D:", 1);
    ssd1306_Strings(43, 52, "V: ", 1);
    ssd1306_Strings(83, 18, "Cell3", 0);
    ssd1306_Strings(83, 31, "D:", 1);
    ssd1306_Strings(83, 52, "V: ", 1);
    ssd1306_drawlinev(45, 10, 1, 40);
    ssd1306_drawlinev(85, 10, 1, 40);
    ssd1306_update();
}

uint8_t Init_BatterySetup()
{
    static uint8_t counter;
    uint8_t selectButton = 0;
    static uint8_t configFlag;
    uint8_t readdownButton = 0;
    uint8_t readupButton = 0;


ssd1306_clear();
readdownButton=digitalRead(12);
delay(50);
readupButton=digitalRead(11);
delay(50);
selectButton=digitalRead(7);
delay(50);


if(readdownButton == 0)
{
    counter++;
}

if(readupButton == 0)
{
    counter--;
}

switch(counter){
    case 0:
ssd1306_clear();
ssd1306_drawrectagle(5,8,1,120,10);
ssd1306_Strings(4,10,"Configuration: 3S1P",0);
ssd1306_Strings(4,20,"Configuration: 3S2P",1);
ssd1306_Strings(4,30,"Configuration: 4S1P",1);
ssd1306_Strings(4,40,"Configuration: 4S2P",1);
ssd1306_update();
   break;
    case 1:
ssd1306_clear();    
ssd1306_Strings(4,10,"Configuration: 3S1P",1);
ssd1306_drawrectagle(5,18,1,120,10);
ssd1306_Strings(4,20,"Configuration: 3S2P",0);
ssd1306_Strings(4,30,"Configuration: 4S1P",1);
ssd1306_Strings(4,40,"Configuration: 4S2P",1);
ssd1306_update();
    break;
    case 2:
ssd1306_clear();    
ssd1306_Strings(4,10,"Configuration: 3S1P",1);
ssd1306_Strings(4,20,"Configuration: 3S2P",1);
ssd1306_drawrectagle(5,28,1,120,10);
ssd1306_Strings(4,30,"Configuration: 4S1P",0);
ssd1306_Strings(4,40,"Configuration: 4S2P",1);
ssd1306_update();
   break;
       case 3:
ssd1306_clear();    
ssd1306_Strings(4,10,"Configuration: 3S1P",1);
ssd1306_Strings(4,20,"Configuration: 3S2P",1);
ssd1306_Strings(4,30,"Configuration: 4S1P",1);
ssd1306_drawrectagle(5,38,1,120,10);
ssd1306_Strings(4,40,"Configuration: 4S2P",0);
ssd1306_update();
   break;

}
   if(counter > 3){
    counter = 0;
   }
if((counter == 0) && (selectButton == 0))
{
    configFlag = 1;
    return(configFlag);
} else{
    if((counter == 1) && (selectButton == 0))
     {
    configFlag = 2;
    return(configFlag);
     }else{
     if((counter == 2) && (selectButton == 0))
     {
     configFlag = 3;
     return(configFlag);
     }else{
        if((counter == 3) && (selectButton == 0))
        {
     configFlag = 4;
     return(configFlag);
        }
            configFlag = 0;
            return(configFlag);
        }
     }
}
}


