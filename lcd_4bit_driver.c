#include<lpc21xx.h>
#include "project.h"

//data types
typedef  unsigned char u8;
typedef unsigned int u32;
typedef float u32f;
typedef int s32;
static void lcd_enable(void);

//control pins of lcd
#define rs (1<<17)
#define rw (1<<18)
#define en (1<<19)
#define data_pins (0xf<<20)

//lcd init
void lcd_init(void)
{
        IODIR1|=rs|en|data_pins|rw;
        IOCLR1|=rs|en|data_pins|rw;
        PINSEL2=0X0;
        lcd_cmd(0x02);//To select 4-Bit mode
        lcd_cmd(0x28);//Selecting 1st and 2nd row for the LCD
        lcd_cmd(0x0E);//Enabling the cursor
        lcd_cmd(0x01);//clear the screen
        lcd_cmd(0x0C);//Desabling the cursor
}
void lcd_cmd(u8 cmd)
{
        u32 temp;
        IOCLR1=data_pins;
        temp=(cmd & 0xF0)<<16;
        IOSET1=temp;

        IOCLR1=rs;
        IOCLR1=rw;
        lcd_enable();

        IOCLR1=data_pins;
        temp=(cmd & 0x0F)<<20;
        IOSET1=temp;
        IOCLR1=rs;
        IOCLR1=rw;
        lcd_enable();
}
void lcd_data(u8 data)
{
 u32 temp;
        IOCLR1=data_pins;
        temp=(data & 0xF0)<<16;
        IOSET1=temp;
        IOSET1=rs;
        IOCLR1=rw;
        lcd_enable();

        IOCLR1=data_pins;
        temp=(data & 0x0F)<<20;
        IOSET1=temp;
        IOSET1=rs;
        IOCLR1=rw;
        lcd_enable();
}
void lcd_enable()
{
        IOSET1=en;
        delay_ms(2);
        IOCLR1=en;
}
void lcd_string(u8 *ptr)
{
        while(*ptr!=0)
        {
                lcd_data(*ptr++);
        }
}

void lcd_integer(s32 num)
{
        int a[10],i=0;
        if(num < 0)
        {
                lcd_data('-');
                num=-num;
        }
        if(num==0)
        {
        lcd_data('0');
                return;
        }
        while(num>0)
        {
                a[i]=(num%10+48);
                num/=10;
                i++;
        }
        for(i=i-1;i>=0;i--)
        lcd_data(a[i]);
}
void lcd_float(float voltage) {
    int int_part = (int)voltage;
    int dec_part = (int)((voltage - int_part) * 10);

    lcd_data(0x30 + int_part / 10);      // Tens digit
    lcd_data(0x30 + int_part % 10);      // Units digit
    lcd_data('.');
    lcd_data(0x30 + dec_part);           // Decimal digit
    lcd_data('V');
}
unsigned long int lcd_strlen(char *p)
{
        int i=0;
        while(*p!=0)
        {
        i++;
        p++;
        }
        return i;
}
int lcd_strcmp(const char *s1,const char *s2)
{
        while(*s1 && *s2)
        {
        if(*s1!=*s2)
                return *s1-*s2;
        s1++;
        s2++;
        }
        return *s1-*s2;
}

void lcd_cir_left(char *p,u8 row)
{
        int i,j;
        int len=lcd_strlen(p);
   for(i=0;i<len;i++)
        {
                lcd_cmd((row==0?0x80:0xc0));
                for(j=0;j<16;j++)
                {
                        lcd_data(p[(i+j)%len]);
                }
                delay_ms(200);
        }

}
void lcd_cir_right(char *p,u8 row)
{
        int i,j;
        int len=lcd_strlen(p);
   for(i=0;i<len;i++)
        {
                lcd_cmd((row==0?0x80:0xc0));
                for(j=0;j<16;j++)
                {
                        lcd_data(p[(len-i+j)%len]);
                }
                delay_ms(200);
        }
}
void lcd_cgram(void)
{
        int i;
        u8 a[]={0x10,0xA,0X0,0X0,0X11,0XE,0X0};
        lcd_cmd(0x40);//slot 0
        for(i=0;i<8;i++)
        lcd_data(a[i]);
}

