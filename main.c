#include <lpc21xx.h>
#include "project.h"
#include<stdio.h>


const char *day[] = {
    "", "SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"
};
//data types
typedef  unsigned char u8;
typedef unsigned int u32;
typedef float u32f;
typedef int s32;
typedef unsigned short int u16;

char buf[50];

int main(void)
{
        Time now;
        Date today;

        u32 adc_val1,adc_val2,adc_val3,ldr_val;
        float vout1,vout2,temp;
        Date d={5,21,7,25}; //day date month year
        Time t={9,00,00,1};//hr , min, sec , am=0 pm=1
        i2c_init();
        lcd_init();
        adc_init();
        spi0_init();
        uart0_init(9600);
        set_time(&t);
        set_date(&d);


    while (1)
    {
         get_time(&now);
                get_date(&today);

                adc_val1=adc_read(2);
                vout1=((adc_val1*5)/1023);

                adc_val2=adc_read(1);
                vout2=((adc_val2*3.3)/1023);
                temp=(vout2-0.5)/0.01;

                adc_val3=mcp3204_adc_read(2);
                ldr_val=((4095-adc_val3)/40.95);

                sprintf(buf,"TIME:%02d:%02d:%02d %s TEMPERATURE=%.2fC voltage=%.2fv Light intensity=%d",now.hours,now.min,now.sec,now.pm ? "PM" : "AM",temp,vout1,ldr_val);


                uart0_string((u8 *)buf);
                uart0_string("\r\n");
                sprintf(buf,"DATE:%02d-%02d-20%02d %s",today.date,today.month,today.year,day[today.day]);

                uart0_string((u8 *)buf);
                uart0_string("\r\n");


            lcd_cmd(0x80);
            sprintf(buf,"%02d:%02d:%02d%s",now.hours,now.min,now.sec,now.pm ? "PM" : "AM");
                        lcd_string((u8 *)buf);
            lcd_cmd(0xC0);
            sprintf(buf,"V=%.2fvL=%dT=%.2fC",vout1,ldr_val,temp);
                        lcd_string((u8 *)buf);
                        delay_ms(1000);


    }
}