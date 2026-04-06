#include<lpc21xx.h>
#include "project.h"

typedef  unsigned char u8;
typedef unsigned int u32;
typedef float u32f;
typedef int s32;

void adc_init(void)
{
PINSEL1|=0X15400000; //Select pin select Register
 //(P0.27,P0.28,P0.29,P0.30)
ADCR=0x00200400; // channel not selected
 // ADC Freq=3Mhz
 // Burst Mode is off
 // 10bit ADC Resolution
 // PDN=1.
//      PINSEL1|=0X7800;
        
} 
//adc_read() function()
#define DONE ((ADDR>>31)&1)
u32 adc_read(u32 ch_num)
{

unsigned int result=0;
ADCR|=(1<<ch_num); // select channel number
ADCR|=(1<<24); // Start ADC
while(DONE==0);//loop runs until DONE flag becomes 1
ADCR^=(1<<24); // Stop ADC
ADCR^=(1<<ch_num); // Dis-select ADC
result=(ADDR>>6)&0x3ff; // Extract 10bit result
return result; // return result.
}
float adc_cal_vol(u32 val)
{
return ((((float)val*3.3))/1023);

}



