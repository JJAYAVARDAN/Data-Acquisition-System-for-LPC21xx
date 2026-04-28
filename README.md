# Data Acquisition System (DAQ) – RTX RTOS Version for LPC21xx

## Overview

This project is a real-time embedded **Data Acquisition System (DAQ)** developed in **Embedded C** for the **LPC21xx microcontroller family (ARM7TDMI-S core)** using **Keil uVision IDE** and the **Keil RTX RTOS kernel**.

The system continuously acquires, processes, and displays environmental data from multiple sensors while leveraging multitasking through RTX for improved modularity, responsiveness, and scalability.

### Monitored Parameters

* **Temperature** using LM35 sensor via onboard ADC
* **Voltage** using voltage divider via onboard ADC
* **Light Intensity** using LDR via MCP3204 external SPI ADC
* **Date and Time** using DS1307 RTC via I2C

### Output Interfaces

* **16x2 LCD Display** for local monitoring
* **UART0 Serial Output** for PC serial terminal monitoring

---

# RTX RTOS Upgrade

## Bare-Metal Version

The original implementation used a single super-loop:

```c
while(1)
{
    Read sensors;
    Update display;
    Send UART output;
    delay_ms(1000);
}
```

## RTX Version

The updated system replaces the super-loop with **five concurrent tasks** managed by the RTX scheduler.

### Benefits

* Independent task execution
* Better timing precision
* Improved code modularity
* Easier debugging
* Scalable architecture
* Resource protection using mutexes

---

# Project File Structure

```txt
e:/project/
├── main.c                  # RTX task scheduler implementation
├── project.h               # Shared structs, prototypes, RTX includes
├── adc_driver.c            # Internal ADC driver
├── lcd_4bit_driver.c       # LCD driver
├── uart_driver.c           # UART driver
├── spi_driver.c            # SPI driver
├── mcp3204_driver.c        # External ADC driver
├── i2c_driver.c            # I2C driver
├── rtc_driver.c            # RTC driver
├── delay.c                 # Timer delay utilities
├── RTX_Config.c            # RTX kernel configuration
├── Startup.s               # ARM startup assembly
├── Objects/                # Build outputs
└── README.md               # Documentation
```

---

# Driver Reuse

All peripheral drivers from the bare-metal project remain unchanged.

| File              | Purpose                  |
| ----------------- | ------------------------ |
| adc_driver.c      | Internal 10-bit ADC      |
| lcd_4bit_driver.c | 16x2 LCD interface       |
| uart_driver.c     | UART0 communication      |
| spi_driver.c      | SPI0 communication       |
| mcp3204_driver.c  | MCP3204 ADC reading      |
| i2c_driver.c      | DS1307 communication     |
| rtc_driver.c      | RTC time/date management |
| delay.c           | Delay functions          |
| Startup.s         | MCU startup              |

---

# RTX Task Architecture

## Task Summary

| Task      | Priority | Period   | Responsibility                         |
| --------- | -------- | -------- | -------------------------------------- |
| task_init | 1        | Once     | Hardware initialization, task creation |
| task_rtc  | 4        | 1 second | Read RTC and update time/date          |
| task_adc  | 3        | 500 ms   | Read voltage and temperature           |
| task_ldr  | 3        | 500 ms   | Read light intensity                   |
| task_lcd  | 2        | 1 second | Update LCD display                     |
| task_uart | 2        | 1 second | Send UART reports                      |

---

# Shared Resources

## Mutex Protection

| Mutex        | Purpose                    |
| ------------ | -------------------------- |
| g_data_mutex | Protect shared sensor data |
| g_lcd_mutex  | Protect LCD hardware       |
| g_uart_mutex | Protect UART hardware      |

---

# Shared Sensor Data Structure

```c
typedef struct {
    float temperature;
    float voltage;
    unsigned int light;
    TIME now;
    DATE today;
} SensorData;
```

---

# Hardware Requirements

* LPC21xx development board (LPC2148 recommended)
* 16x2 LCD (4-bit mode)
* DS1307 RTC module
* MCP3204 12-bit ADC
* LM35 temperature sensor
* LDR sensor
* Voltage divider circuit
* UART serial monitor
* Pull-up resistors for I2C

---

# Pin Connections

| Peripheral | LPC21xx Pins       | Notes                 |
| ---------- | ------------------ | --------------------- |
| LCD        | P1.17–P1.23        | 4-bit mode            |
| UART0      | P0.0 TXD, P0.1 RXD | 9600 baud             |
| SPI0       | P0.4–P0.7          | MCP3204               |
| I2C0       | P0.2 SCL, P0.3 SDA | DS1307                |
| ADC        | P0.27, P0.28       | Voltage & Temperature |

---

# RTX Kernel Configuration

## Recommended Settings

```c
#define OS_TASKCNT   8
#define OS_STKSIZE   256
#define OS_CLOCK     60000000
#define OS_TICK      10000
#define OS_ROBIN     1
#define OS_MUTEXCNT  4
```

### Timing

* `os_dly_wait(50)` = 500 ms
* `os_dly_wait(100)` = 1 second

---

# Functional Features

## Sensor Reading

* Internal ADC for voltage and temperature
* External SPI ADC for LDR
* Independent periodic sampling

## RTC Management

* DS1307 support
* 12-hour format
* AM/PM support
* BCD to decimal conversion

## Display Output

### LCD:

```txt
09:00:01AM
V=2.45vL=200T=25.5C
```

### UART:

```txt
TIME:09:00:01 AM TEMPERATURE=25.50C voltage=2.45v Light intensity=200
DATE:21-05-2025 WED
```

---

# Calibration Equations

## Temperature

```txt
Temp = (Vout - 0.5) / 0.01 °C
```

## Light Intensity

```txt
Light = (4095 - ADC) / 40.95 lux
```

---

# Build Instructions

## Keil MDK5 / RTX Setup

1. Open project in **Keil uVision**
2. Navigate to:
   **Manage Run-Time Environment → CMSIS → RTOS → RTX**
3. Add all driver source files
4. Configure RTX settings
5. Build project (`F7`)
6. Flash to LPC21xx board
7. Open UART terminal at **9600 baud**

---

# Comparison: Bare-Metal vs RTX RTOS

| Feature         | Bare-Metal  | RTX RTOS     |
| --------------- | ----------- | ------------ |
| Execution Model | Single loop | Multitasking |
| Timing Accuracy | Moderate    | High         |
| Scalability     | Limited     | Excellent    |
| Debugging       | Harder      | Easier       |
| Responsiveness  | Lower       | Better       |
| Maintainability | Moderate    | High         |

---

# Sample Output

```txt
TIME:09:00:01 AM TEMPERATURE=25.50C voltage=2.45v Light intensity=200
DATE:21-05-2025 WED

TIME:09:00:02 AM TEMPERATURE=25.52C voltage=2.46v Light intensity=198
DATE:21-05-2025 WED
```

---

# Future Enhancements

* SD card data logging
* Alarm thresholds
* Wireless monitoring (ESP8266/ESP32)
* IoT dashboard integration
* Additional sensors
* Fault detection
* Data analytics

---

# Learning Outcomes

This project demonstrates:

* ARM7 embedded programming
* RTX RTOS task scheduling
* Mutex synchronization
* ADC, SPI, UART, and I2C interfacing
* Real-time firmware architecture
* Modular embedded software design

---

# Conclusion

The RTX RTOS version of the LPC21xx DAQ system transforms a basic sensor monitoring application into a professional real-time embedded system.

It provides:

* Better system organization
* Parallel task execution
* Accurate periodic sampling
* Improved maintainability
* Industrial-style firmware architecture

This project is highly suitable for:

* Embedded systems portfolios
* RTOS learning
* ARM7 development practice
* Academic demonstrations
* Industrial firmware foundations
