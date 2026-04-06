#include <lpc21xx.h>
#include "project.h"

//data types
typedef  unsigned char u8;
typedef unsigned int u32;
typedef float u32f;
typedef int s32;
typedef unsigned short int u16;
/*
  MCP3204 External ADC Driver
  adc_driver.c

        connections:
        P0.4-> SCK 0
        P0.5-> MISO 0
        P0.6-> MOSI 0
        P0.7-> CS 0
*/

u32 mcp3204_adc_read(u8 ch_num){
        u8 byteH, byteL;
        u32 adcval;
        byteH=byteL=0;
        adcval=0;
        ch_num<<=6;//set ch num

        IOCLR0=(1<<7);//CS=0    P0.7=0  select slave for commu

        spi0(0x06);//Start Bit & Single Ended Mode Selected
        byteH=spi0(ch_num);//get higher byte (B8-B11) from ADC
        byteL=spi0(0);//get lower byte (B0-B7) from ADC

        IOSET0=(1<<7);//CS=1 deselect slave

        byteH&=0x0F;//Mask higher 4bits
        adcval=(byteH<<8)|byteL;//merge result

        return adcval;
}

