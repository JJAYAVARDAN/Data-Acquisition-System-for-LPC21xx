#include<lpc21xx.h>
#include "project.h"

typedef float u32f;
typedef  unsigned char u8;
typedef unsigned int u32;
typedef int s32;
#define I2EN 1<<6

void i2c_init(void)
{


        PINSEL0|=0x00000050;
        I2CONSET=I2EN;
        I2SCLH=I2SCLL=75;
}
#define SI (I2CONSET>>3&1)
#define STA 1<<5
#define STO 1<<4
void i2c_write(u8 sa,u8 mr,u8 data)
{

        I2CONSET=STA;
        I2CONCLR=1<<3;
        while(SI==0);
        I2CONCLR=STA;


        I2DAT=sa;
        I2CONCLR=1<<3;
        while(SI==0);
        if(I2STAT == 0X20)
        {
                uart0_string("ERR:SA+W\r\n");
                goto exit;
        }

        I2DAT=mr;
        I2CONCLR=1<<3;
        while(SI==0);
        if(I2STAT==0x30)
        {
        uart0_string("ERR0R: IN MEMORY ADDRESS\r\n");
        goto exit;
        }

        I2DAT=data;
        I2CONCLR=1<<3;
        while(SI==0);
        if(I2STAT==0X30)
        {
        uart0_string("ERR : IN THE DATA\r\n");
                goto exit;
        }

        exit:
        I2CONSET|=1<<4;
        I2CONCLR=1<<3;
        delay_ms(1);
}
unsigned char i2c_read(u8 sa,u8 mr)
{
        u8 temp=0;

                I2CONSET=STA;
        I2CONCLR=1<<3;
        while(SI==0);
        I2CONCLR=STA;


        I2DAT=sa;
        I2CONCLR=1<<3;
        while(SI==0);
        if(I2STAT==0x20)
        {
        uart0_string("ERR:SA+W\r\n");
                goto exit;
        }

        I2DAT=mr;
        I2CONCLR=1<<3;
        while(SI==0);
        if(I2STAT==0x30)
        {
                uart0_string("ERR:in mr \r\n");
                goto exit;
        }

        I2CONSET=STA;
        I2CONCLR=1<<3;
        while(SI==0);
        I2CONCLR=STA;

        I2DAT=sa|1;
        I2CONCLR=1<<3;
        while(SI==0);
        if(I2STAT==0x48)
        {
                uart0_string("ERR: SA+R\r\n");
                goto exit;
        }

        I2CONCLR=1<<3;
        while(SI==0);
        temp=I2DAT;

        exit:
        I2CONSET|=1<<4;
        I2CONCLR=1<<3;
        delay_ms(1);
        return temp;
}


