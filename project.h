/*===========================================================================
 * project.h  –  Data Acquisition System for LPC21xx
 *               RTX RTOS Version (Keil uVision)
 *
 * All peripheral driver prototypes remain identical to the bare-metal
 * version.  Only main.c changes; every driver .c file is reused as-is.
 *===========================================================================*/

#ifndef PROJECT_H
#define PROJECT_H

#include <lpc21xx.h>
#include <RTL.h>          /* Keil RTX kernel API                            */
#include <stdio.h>
#include <string.h>

/*---------------------------------------------------------------------------
 *  Primitive type aliases (same as original project)
 *---------------------------------------------------------------------------*/
typedef unsigned char       u8;
typedef unsigned short int  u16;
typedef unsigned int        u32;
typedef int                 s32;
typedef float               u32f;

/*---------------------------------------------------------------------------
 *  RTC data structures
 *---------------------------------------------------------------------------*/
typedef struct {
    u8 hours;
    u8 min;
    u8 sec;
    u8 pm;      /* 0 = AM, 1 = PM */
} Time;

typedef struct {
    u8 day;     /* 1=SUN … 7=SAT */
    u8 date;
    u8 month;
    u8 year;    /* 2-digit, e.g. 25 → 2025 */
} Date;

/*---------------------------------------------------------------------------
 *  Shared sensor data – written by sensor tasks, read by display tasks.
 *  Protected by g_data_mutex.
 *---------------------------------------------------------------------------*/
typedef struct {
    /* ADC / sensor readings */
    float   voltage;        /* voltage divider channel (V)   */
    float   temperature;    /* LM35 temperature (°C)         */
    u32     light;          /* LDR intensity (lux, approx.)  */
    /* RTC snapshot */
    Time    now;
    Date    today;
} SensorData;

/* Global shared data and RTX synchronisation objects                        */
extern SensorData  g_data;          /* shared sensor snapshot               */
extern OS_MUT      g_data_mutex;    /* guards g_data                        */
extern OS_MUT      g_lcd_mutex;     /* guards LCD (hardware resource)       */
extern OS_MUT      g_uart_mutex;    /* guards UART0 (hardware resource)     */

/*---------------------------------------------------------------------------
 *  Day-name lookup (defined in main.c)
 *---------------------------------------------------------------------------*/
extern const char *day_name[];

/*===========================================================================
 *  Driver prototypes  (unchanged from bare-metal version)
 *===========================================================================*/

/* ADC -------------------------------------------------------------------- */
void  adc_init(void);
u32   adc_read(u32 ch_num);          /* param is u32 – matches adc_driver.c  */

/* LCD (4-bit) ------------------------------------------------------------ */
void  lcd_init(void);
void  lcd_cmd(u8 cmd);
void  lcd_data(u8 dat);
void  lcd_string(u8 *str);
void  lcd_int(int val);
void  lcd_float(float voltage);      /* single-arg – matches lcd_4bit_driver.c */

/* UART0 ------------------------------------------------------------------ */
void  uart0_init(u32 baud);
void  uart0_tx(u8 ch);
void  uart0_string(u8 *str);
void  uart0_int(int val);

/* SPI0 ------------------------------------------------------------------- */
void  spi0_init(void);
u8    spi0(u8 data);                 /* named "spi0" – matches spi_driver.c   */

/* MCP3204 (12-bit SPI ADC) ----------------------------------------------- */
u32   mcp3204_adc_read(u8 channel);

/* I2C0 ------------------------------------------------------------------- */
void          i2c_init(void);
void          i2c_write(u8 sa, u8 mr, u8 data); /* matches i2c_driver.c     */
unsigned char i2c_read (u8 sa, u8 mr);           /* matches i2c_driver.c     */

/* DS1307 RTC ------------------------------------------------------------- */
void  set_time(Time *t);
void  set_date(Date *d);
void  get_time(Time *t);
void  get_date(Date *d);

/* Delay (Timer0-based, still used inside drivers) ------------------------ */
void  delay_ms(u32 ms);

#endif /* PROJECT_H */
