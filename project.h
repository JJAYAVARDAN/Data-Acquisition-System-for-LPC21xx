#ifndef PROJECT_H
#define PROJECT_H

//data types
typedef  unsigned char u8;
typedef unsigned int u32;
typedef float u32f;
typedef int s32;

//delay
void delay_ms(unsigned int ms);

//lcd
void lcd_init(void);
void lcd_cmd(u8 cmd);
void lcd_data(u8 data);
void lcd_enable(void);
void lcd_string(u8 *ptr);
void lcd_integer(s32 num);
void lcd_float(float f);
unsigned long int lcd_strlen(char *p);
int lcd_strcmp(const char *s1,const char *s2);
void lcd_cir_left(char *p,u8 row);
void lcd_cir_right(char *p,u8 row);
void lcd_cgram(void);

//uart0
void uart0_init(u32 baud);
u8 uart0_rx(void);
void uart0_bin(int num);
void uart0_string(u8 *ptr);
void uart0_integer(int num);
void uart0_rx_string(char *ptr,int len);

////spi
void spi0_init(void);
u8 spi0(u8 data);

//external adc mcp3204 spi
u32 mcp3204_adc_read(u8 ch_num);

//adc
float adc_cal_vol(u32 val);
void adc_init(void);
u32 adc_read(u32 ch_num);


//i2c
void i2c_init(void);
void i2c_write(u8 sa,u8 mr,u8 data);
u8 i2c_read(u8 sa,u8 mr);

//rtc delarations
typedef struct
{
        u8 hours,min,sec,pm;
}Time;

typedef struct
{
        u8 day,date,month,year;
}Date;


//RTC functions
u8 BCD2DEC(u8 bcd);
u8 DEC2BCD(u8 dec);
void set_time(Time *t);
void set_date(Date *d);
void get_time(Time *t);
void get_date(Date *d);

#endif



