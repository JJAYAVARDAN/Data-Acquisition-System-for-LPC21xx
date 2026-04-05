#include<lpc21xx.h>
#include "project.h"

typedef float u32f;
typedef  unsigned char u8;
typedef unsigned int u32;
typedef int s32;


#define SPIF (S0SPSR&0x80)
void spi0_init(void){
        PINSEL0|=0x01500;
        IODIR0|=(1<<7);
        IOSET0=(1<<7);
        S0SPCCR=150;
        S0SPCR=0x0020;

}

u8 spi0(u8 data){
        S0SPDR=data;
        while(SPIF==0);
        return S0SPDR;
}