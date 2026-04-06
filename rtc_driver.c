#include<lpc21xx.h>
#include "project.h"


//macros
#define RTC_SEC 0x00
#define RTC_MIN 0x01
#define RTC_HR 0x02
#define RTC_DAY 0x03
#define RTC_DATE 0x04
#define RTC_MON 0x05
#define RTC_YEAR 0x06

#define RTC_ADD 0xD0

//data types
typedef  unsigned char u8;
typedef unsigned int u32;
typedef float u32f;
typedef int s32;



u8 BCD2DEC(u8 bcd)
{
        return (((bcd>>4)*10)+(bcd&0xF));
}
u8 DEC2BCD(u8 dec)
{
        return (((dec/10)<<4)|(dec%10));
}

void set_time(Time *t)
{
        u8 hour;
        i2c_write(RTC_ADD,RTC_SEC,DEC2BCD(t->sec));
        i2c_write(RTC_ADD,RTC_MIN,DEC2BCD(t->min));
        hour=DEC2BCD(t->hours)|0x40;//6th bit is 1 for 12 hours
        if(t->pm)
        hour |=0x20;//set pm bit

        i2c_write(RTC_ADD,RTC_HR,hour);
}
void set_date(Date *d)
{
        i2c_write(RTC_ADD,RTC_DAY,DEC2BCD(d->day));
        i2c_write(RTC_ADD,RTC_DATE,DEC2BCD(d->date));
        i2c_write(RTC_ADD,RTC_MON,DEC2BCD(d->month));
        i2c_write(RTC_ADD,RTC_YEAR,DEC2BCD(d->year));

}

void get_time(Time *t)
{
        u8 hr;
        t->sec=BCD2DEC(i2c_read(RTC_ADD,RTC_SEC));
        t->min=BCD2DEC(i2c_read(RTC_ADD,RTC_MIN));

        hr=i2c_read(RTC_ADD,RTC_HR);
        if(hr&0x40)//12 hour mode
        {
                t->pm=(hr&0x20)? 1 : 0;
                t->hours=BCD2DEC(hr&0x1F);//extracting lower 4 bits
        }
        else
        {
        u8 hour24=BCD2DEC(hr&0x3F);
                if(hour24==0)
                {
                        t->hours=12;
                        t->pm=0;
                }
                else if(hour24<12)
                {
                t->hours=hour24;
                        t->pm=0;
                }
                else if(hour24==12)
                {
                t->hours=12;
                        t->pm=1;
                }
                else
                        {
                        t->hours=hour24-12;
                                t->pm=1;
                        }
        }
}
void get_date(Date *d)
{
        d->day=BCD2DEC(i2c_read(RTC_ADD,RTC_DAY));
        d->date=BCD2DEC(i2c_read(RTC_ADD,RTC_DATE));
        d->month=BCD2DEC(i2c_read(RTC_ADD,RTC_MON));
d->year=BCD2DEC(i2c_read(RTC_ADD,RTC_YEAR));
}



