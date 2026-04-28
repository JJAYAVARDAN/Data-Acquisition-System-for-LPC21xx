/*===========================================================================
 * RTX_Config.c  –  Keil RTX RTOS Configuration for LPC21xx (ARM7TDMI-S)
 *
 * Adjust the constants below to tune stack sizes, tick rate, and task count
 * for your specific build.  This file is compiled alongside the RTX kernel
 * sources included by Keil uVision's Run-Time Environment.
 *
 * Location in Keil pack (example):
 *   C:\Keil\ARM\PACK\Keil\MDK-Middleware\<ver>\Source\RTX_Conf_CM.c
 *   –or– the legacy ARM7 version shipped with MDK 4.x / 5.x under
 *   C:\Keil\ARM\RV31\INC\RTX_Config.c
 *
 * Reference: Keil RTX User Guide (MDK-ARM)
 *===========================================================================*/

#include <RTL.h>

/*---------------------------------------------------------------------------
 *  OS_TASKCNT  –  Maximum number of concurrent tasks (including idle)
 *                 We need 6 user tasks + 1 idle = 7.  Add headroom → 8.
 *---------------------------------------------------------------------------*/
#define OS_TASKCNT      8

/*---------------------------------------------------------------------------
 *  OS_PRIVCNT  –  Number of tasks running in privileged mode (ARM7: ignored,
 *                 all tasks run in privileged SVC mode)
 *---------------------------------------------------------------------------*/
#define OS_PRIVCNT      0

/*---------------------------------------------------------------------------
 *  OS_STKSIZE  –  Default task stack size in WORDS (4 bytes each)
 *                 sprintf() for float needs ~512 bytes minimum.
 *                 256 words = 1024 bytes  (safe margin)
 *---------------------------------------------------------------------------*/
#define OS_STKSIZE      256

/*---------------------------------------------------------------------------
 *  OS_TICK  –  RTX system tick period in µs.
 *              10 000 µs = 10 ms per tick.
 *              os_dly_wait(100) then equals 1 second.
 *---------------------------------------------------------------------------*/
#define OS_TICK         10000

/*---------------------------------------------------------------------------
 *  OS_ROBIN  –  Round-robin scheduling for tasks of equal priority.
 *               Set to 1 to enable (recommended for task_lcd / task_uart
 *               which share priority 2).
 *---------------------------------------------------------------------------*/
#define OS_ROBIN        1
#define OS_ROBINTOUT    5   /* round-robin timeslice = 5 ticks = 50 ms       */

/*---------------------------------------------------------------------------
 *  OS_MUTEXCNT  –  Maximum number of mutexes in the system.
 *                  We create 3: g_data_mutex, g_lcd_mutex, g_uart_mutex.
 *---------------------------------------------------------------------------*/
#define OS_MUTEXCNT     4

/*---------------------------------------------------------------------------
 *  OS_SEMACNT  –  Maximum number of semaphores (not used in this project
 *                 but set a safe minimum).
 *---------------------------------------------------------------------------*/
#define OS_SEMACNT      2

/*---------------------------------------------------------------------------
 *  OS_TIMERCNT –  Number of software timers (not used here).
 *---------------------------------------------------------------------------*/
#define OS_TIMERCNT     0

/*---------------------------------------------------------------------------
 *  OS_CLOCK  –  Core clock frequency in Hz for the ARM7 timer ISR.
 *               LPC2148 typical CCLK = 60 MHz.
 *---------------------------------------------------------------------------*/
#define OS_CLOCK        60000000

/*---------------------------------------------------------------------------
 *  OS_LOCKUPDET  –  Lockup detection (1 = enabled).
 *---------------------------------------------------------------------------*/
#define OS_LOCKUPDET    0

/*===========================================================================
 *  STACK ALLOCATION TABLE
 *  Explicitly size each task stack if needed (overrides OS_STKSIZE).
 *  Format: { task_function, stack_size_in_words }
 *  Leave empty to use the default OS_STKSIZE for all tasks.
 *===========================================================================*/
/*
   If Keil's RTX supports per-task stack sizing in this config file you can
   list them here.  Otherwise adjust OS_STKSIZE to the largest requirement.

   task_init  : ~256 words (calls multiple init functions)
   task_adc   : ~128 words
   task_ldr   : ~128 words
   task_rtc   : ~128 words
   task_lcd   : ~256 words (sprintf with float)
   task_uart  : ~256 words (sprintf with float)
*/

/*---------------------------------------------------------------------------
 *  NOTE: In Keil MDK 5 using the CMSIS-RTOS RTX wrapper you would instead
 *  edit RTX_Conf_CM.c and set:
 *
 *    #define OS_TASKCNT   8
 *    #define OS_STKSIZE   256     // in words
 *    #define OS_CLOCK     60000000
 *    #define OS_TICK      10000   // 10 ms
 *    #define OS_ROBIN     1
 *    #define OS_MUTEXCNT  4
 *
 *  The values above serve as a reference for both config file styles.
 *---------------------------------------------------------------------------*/
