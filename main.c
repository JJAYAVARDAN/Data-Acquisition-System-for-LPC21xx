/*===========================================================================
 * main.c  –  Data Acquisition System for LPC21xx
 *             RTX RTOS Version (Keil uVision, Keil RTX kernel)
 *
 * RTX TASK ARCHITECTURE
 * ─────────────────────
 *  task_init     (prio 1) : Hardware init, spawns all other tasks, deletes self
 *  task_adc      (prio 3) : Reads onboard ADC (voltage + temperature) every 500 ms
 *  task_ldr      (prio 3) : Reads MCP3204 SPI ADC (LDR light) every 500 ms
 *  task_rtc      (prio 4) : Reads DS1307 RTC (time + date) every 1 s
 *  task_lcd      (prio 2) : Refreshes 16×2 LCD with latest snapshot every 1 s
 *  task_uart     (prio 2) : Sends UART0 report every 1 s
 *
 * SYNCHRONISATION
 * ───────────────
 *  g_data_mutex  : protects the shared SensorData struct
 *  g_lcd_mutex   : serialises access to the LCD hardware
 *  g_uart_mutex  : serialises access to UART0 hardware
 *
 * RTX TICK
 * ────────
 *  Default Keil RTX tick = 10 ms  →  os_dly_wait(100) == 1 second
 *  Configure in RTX_Conf_CM.c / RTX_Config.c as needed.
 *
 * HOW TO ADD TO KEIL PROJECT
 * ──────────────────────────
 *  1. Open / create your .uvproj for LPC2148.
 *  2. In "Manage Run-Time Environment" add:
 *       CMSIS → RTOS (API)  [RTX]
 *     OR manually add RTX source files from
 *       \Keil\ARM\PACK\ARM\CMSIS\<ver>\RTOS\RTX\SRC\ARM\
 *  3. Add all .c driver files + this main.c to the project.
 *  4. Ensure RTX_Conf_CM.c (or RTX_Config.c for ARM7) is included.
 *  5. Set OS_TASKCNT >= 6 in the RTX config file.
 *  6. Build and flash.
 *===========================================================================*/

#include "project.h"

/*---------------------------------------------------------------------------
 *  Day-name table (index 0 unused; DS1307 day is 1-based)
 *---------------------------------------------------------------------------*/
const char *day_name[] = {
    "", "SUN", "MON", "TUE", "WED", "THU", "FRI", "SAT"
};

/*---------------------------------------------------------------------------
 *  Shared global data + RTX synchronisation objects
 *---------------------------------------------------------------------------*/
SensorData  g_data;          /* single producer / multi-consumer snapshot    */
OS_MUT      g_data_mutex;    /* guards g_data                                */
OS_MUT      g_lcd_mutex;     /* guards LCD hardware                          */
OS_MUT      g_uart_mutex;    /* guards UART0 hardware                        */

/* UART print buffer (each task uses its own stack-allocated buf, but we
   keep one small shared format buf protected by g_uart_mutex)               */
static char uart_buf[80];
static char lcd_buf[20];

/*===========================================================================
 *  TASK: task_adc
 *  Priority : 3 (medium)
 *  Period   : 500 ms
 *
 *  Reads onboard 10-bit ADC channels:
 *    CH2 → voltage divider  (Vref = 5 V)
 *    CH1 → LM35 temperature (Vref = 3.3 V)
 *  Stores results in g_data under mutex protection.
 *===========================================================================*/
__task void task_adc(void)
{
    u32   raw_v, raw_t;
    float vout_v, vout_t, temp;

    for (;;)
    {
        /* --- read hardware (no mutex needed; ADC hardware is owned here) -- */
        raw_v  = adc_read(2);                    /* voltage channel          */
        vout_v = ((float)raw_v  * 5.0f)  / 1023.0f;

        raw_t  = adc_read(1);                    /* temperature channel      */
        vout_t = ((float)raw_t  * 3.3f)  / 1023.0f;
        temp   = (vout_t - 0.5f) / 0.01f;       /* LM35: 10 mV/°C, 0.5 V offset */

        /* --- update shared data -----------------------------------------  */
        os_mut_wait(&g_data_mutex, 0xFFFF);
            g_data.voltage     = vout_v;
            g_data.temperature = temp;
        os_mut_release(&g_data_mutex);

        os_dly_wait(50);                         /* 50 ticks × 10 ms = 500 ms */
    }
}

/*===========================================================================
 *  TASK: task_ldr
 *  Priority : 3 (medium)
 *  Period   : 500 ms
 *
 *  Reads MCP3204 12-bit SPI ADC channel 2 (LDR).
 *  Converts raw value to approximate lux.
 *===========================================================================*/
__task void task_ldr(void)
{
    u32 raw_ldr, lux;

    for (;;)
    {
        raw_ldr = mcp3204_adc_read(2);
        lux     = (u32)((4095.0f - (float)raw_ldr) / 40.95f);

        os_mut_wait(&g_data_mutex, 0xFFFF);
            g_data.light = lux;
        os_mut_release(&g_data_mutex);

        os_dly_wait(50);                         /* 500 ms                    */
    }
}

/*===========================================================================
 *  TASK: task_rtc
 *  Priority : 4 (highest of the sensor tasks – RTC data is timing-critical)
 *  Period   : 1 s
 *
 *  Reads DS1307 over I2C and stores a consistent Time+Date snapshot.
 *===========================================================================*/
__task void task_rtc(void)
{
    Time t;
    Date d;

    for (;;)
    {
        get_time(&t);
        get_date(&d);

        os_mut_wait(&g_data_mutex, 0xFFFF);
            g_data.now   = t;
            g_data.today = d;
        os_mut_release(&g_data_mutex);

        os_dly_wait(100);                        /* 100 ticks × 10 ms = 1 s   */
    }
}

/*===========================================================================
 *  TASK: task_lcd
 *  Priority : 2 (display – lower than sensor tasks so sensors run first)
 *  Period   : 1 s
 *
 *  Row 0: HH:MM:SS AM/PM
 *  Row 1: V=x.xx L=xxx T=xx.xx°C
 *===========================================================================*/
__task void task_lcd(void)
{
    SensorData snap;   /* local copy so we hold the mutex for minimum time   */

    for (;;)
    {
        /* Take a consistent snapshot ----------------------------------------*/
        os_mut_wait(&g_data_mutex, 0xFFFF);
            snap = g_data;
        os_mut_release(&g_data_mutex);

        /* Write to LCD (serialised by lcd_mutex) ----------------------------*/
        os_mut_wait(&g_lcd_mutex, 0xFFFF);

            /* Line 1: time ------------------------------------------------- */
            lcd_cmd(0x80);   /* DDRAM address: row 0, col 0                  */
            sprintf(lcd_buf, "%02d:%02d:%02d%s",
                    snap.now.hours,
                    snap.now.min,
                    snap.now.sec,
                    snap.now.pm ? "PM" : "AM");
            lcd_string((u8 *)lcd_buf);

            /* Line 2: sensors (use lcd_string with sprintf buffer;           *
             *  lcd_float() takes only one arg so we avoid it for formatted   *
             *  multi-value lines and use lcd_string throughout)               */
            lcd_cmd(0xC0);   /* DDRAM address: row 1, col 0                  */
            sprintf(lcd_buf, "V=%.2fL=%dT=%.1fC",
                    snap.voltage,
                    (int)snap.light,
                    snap.temperature);
            lcd_string((u8 *)lcd_buf);

        os_mut_release(&g_lcd_mutex);

        os_dly_wait(100);                        /* 1 s                       */
    }
}

/*===========================================================================
 *  TASK: task_uart
 *  Priority : 2 (same as LCD; they take turns on the 1-second cadence)
 *  Period   : 1 s
 *
 *  Sends two lines over UART0 in the same format as the original project:
 *    TIME:HH:MM:SS AM/PM  TEMPERATURE=xx.xxC  voltage=x.xxv  Light intensity=xxx
 *    DATE:MM-DD-20YY  DAY
 *===========================================================================*/
__task void task_uart(void)
{
    SensorData snap;

    for (;;)
    {
        os_mut_wait(&g_data_mutex, 0xFFFF);
            snap = g_data;
        os_mut_release(&g_data_mutex);

        os_mut_wait(&g_uart_mutex, 0xFFFF);

            /* Line 1: time + sensors --------------------------------------- */
            sprintf(uart_buf,
                    "TIME:%02d:%02d:%02d %s  TEMPERATURE=%.2fC  voltage=%.2fv  Light intensity=%d\r\n",
                    snap.now.hours,
                    snap.now.min,
                    snap.now.sec,
                    snap.now.pm ? "PM" : "AM",
                    snap.temperature,
                    snap.voltage,
                    (int)snap.light);
            uart0_string((u8 *)uart_buf);

            /* Line 2: date ------------------------------------------------- */
            sprintf(uart_buf,
                    "DATE:%02d-%02d-20%02d  %s\r\n",
                    snap.today.date,
                    snap.today.month,
                    snap.today.year,
                    day_name[snap.today.day]);
            uart0_string((u8 *)uart_buf);

        os_mut_release(&g_uart_mutex);

        os_dly_wait(100);                        /* 1 s                       */
    }
}

/*===========================================================================
 *  TASK: task_init  (runs first, then deletes itself)
 *  Priority : 1 (lowest – only active during startup)
 *
 *  Initialises all hardware peripherals, sets the initial RTC time/date,
 *  creates all RTOS objects and worker tasks, then deletes itself.
 *===========================================================================*/
__task void task_init(void)
{
    /* ---- Default time/date (edit as required) ---------------------------- */
    Time t_init = { 9, 00, 00, 0 };     /* 09:00:00 AM                       */
    Date d_init = { 4, 21,  5, 25 };    /* Wednesday 21-May-2025             */
                                         /* day: 1=SUN…7=SAT → 4=WED         */

    /* ---- Peripheral initialisation --------------------------------------- */
    i2c_init();
    lcd_init();
    adc_init();
    spi0_init();
    uart0_init(9600);

    /* ---- Set initial RTC time / date ------------------------------------- */
    set_time(&t_init);
    set_date(&d_init);

    /* ---- Zero the shared data structure ---------------------------------- */
    memset(&g_data, 0, sizeof(g_data));

    /* ---- Initialise RTX mutexes ------------------------------------------ */
    os_mut_init(&g_data_mutex);
    os_mut_init(&g_lcd_mutex);
    os_mut_init(&g_uart_mutex);

    /* ---- Splash screen on LCD -------------------------------------------- */
    os_mut_wait(&g_lcd_mutex, 0xFFFF);
        lcd_cmd(0x01);                           /* clear display             */
        lcd_cmd(0x80);
        lcd_string((u8 *)"  DAQ System    ");
        lcd_cmd(0xC0);
        lcd_string((u8 *)"  LPC21xx RTX   ");
    os_mut_release(&g_lcd_mutex);

    delay_ms(1500);                              /* show splash for 1.5 s     */

    /* ---- Create worker tasks --------------------------------------------- */
    /*  os_tsk_create(function, priority)
        Priority range: 1 (lowest user) … 254 (highest user)
        Do NOT use priority 0 (idle) or 255 (reserved).              */
    os_tsk_create(task_rtc,  4);  /* RTC  – highest: time must be consistent */
    os_tsk_create(task_adc,  3);  /* ADC  – sensor read                      */
    os_tsk_create(task_ldr,  3);  /* LDR  – SPI sensor read                  */
    os_tsk_create(task_lcd,  2);  /* LCD  – display output                   */
    os_tsk_create(task_uart, 2);  /* UART – serial output                    */

    /* ---- Init task has finished; remove it from the scheduler ------------ */
    os_tsk_delete_self();
}

/*===========================================================================
 *  ENTRY POINT
 *  os_sys_init() never returns; it transfers control to the RTX scheduler
 *  which runs task_init first (it is the only task at startup).
 *===========================================================================*/
int main(void)
{
    os_sys_init(task_init);   /* hand control to RTX, start with task_init   */

    /* os_sys_init() never returns for ARM7/Cortex targets               */
    for (;;) {}
}
