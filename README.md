# Data Acquisition System for LPC21xx

## Overview
This is an embedded C project developed for the **LPC21xx** microcontroller (ARM7TDMI-S core) using **Keil uVision IDE**. It implements a **Data Acquisition System (DAQ)** that monitors and displays environmental parameters in real-time:

- **Temperature** (from analog sensor, e.g., LM35).
- **Voltage** (from voltage divider).
- **Light Intensity** (from LDR via external ADC).
- **Real-time Clock** (date/time from DS1307 RTC).

Data is displayed on a **16x2 LCD** (local) and output via **UART0** (serial monitor) every second.

## Hardware Requirements
- LPC21xx development board (e.g., LPC2148/2194).
- 16x2 LCD (4-bit mode).
- DS1307 RTC module (I2C).
- MCP3204 12-bit external ADC (SPI).
- Sensors:
  - Temperature sensor (e.g., LM35 on ADC1).
  - Voltage sensor (divider on ADC2).
  - LDR + voltage divider on MCP3204 CH2.
- Crystal for RTC (32.768 kHz).
- Connections powered at 3.3V/5V as per MCU.

## Pin Connections
| Peripheral | LPC21xx Pins | Notes |
|------------|--------------|-------|
| **LCD (4-bit)** | P1.17 (RS), P1.18 (RW), P1.19 (EN), P1.20-23 (D4-D7) | RW grounded if write-only. |
| **UART0** | P0.0 (TXD), P0.1 (RXD) | 9600 baud. |
| **SPI0 (MCP3204)** | P0.4 (SCK0), P0.5 (MISO0), P0.6 (MOSI0), P0.7 (CS) | CS active low. |
| **I2C0 (DS1307)** | P0.2 (SCL0), P0.3 (SDA0) | Pull-ups required. |
| **Onboard ADC** | P0.27 (CH2 voltage), P0.28 (CH1 temp), P0.29/30 unused | 3.3V reference. |
| **Timer0** | Internal for delay | - |

## Features
- **Sensor Reading**: 10-bit onboard ADC (voltage/temp), 12-bit MCP3204 (LDR).
- **RTC**: DS1307 in 12-hour mode with AM/PM, BCD conversion.
- **Display**: LCD shows time on row1, V/Light/Temp on row2. UART sends full time/date + sensors.
- **Init**: Sets default time/date (e.g., 9:00 AM, 5/21/2025).
- **Calibration**: Temp = (Vout - 0.5)/0.01 °C; Light = (4095 - ADC)/40.95 lux.
- **Refresh**: 1 second interval.

## File Structure
```
e:/project/
├── main.c              # Main loop: sensor reads, display, UART output.
├── project.h           # Function prototypes & structs (Time/Date).
├── adc_driver.c        # Onboard ADC init/read/volt calc.
├── lcd_4bit_driver.c   # LCD 4-bit init/cmd/data/string/int/float.
├── uart_driver.c       # UART0 init/TX/RX/string/int/bin.
├── spi_driver.c        # SPI0 init/TX.
├── mcp3204_driver.c    # MCP3204 ADC read via SPI.
├── i2c_driver.c        # I2C0 init/write/read (with error handling).
├── rtc_driver.c        # DS1307 set/get time/date (BCD<->DEC).
├── delay.c             # ms delay using Timer0.
├── Startup.s           # Keil startup code.
├── data acquisition system.*  # Keil project files (uvproj, uvopt, etc.).
├── Objects/            # Build outputs.
└── README.md           # This file.
```

## Build & Flash Instructions
1. Open `data acquisition system.uvproj` in **Keil uVision 5**.
2. Ensure LPC21xx device selected (e.g., LPC2148).
3. Fix includes if needed: Add LPC21xx header path (usually automatic).
4. **Build** (F7): Resolve any lpc21xx.h errors by configuring includes.
5. **Flash** to MCU using JTAG/ISP (e.g., Flash Magic or Keil ULINK).
6. Connect UART to PC (9600 8N1).

## Usage
1. Power on the board.
2. Set initial time/date in `main.c` (hardcoded).
3. Open serial monitor (PuTTY/Tera Term) at **9600 baud** on COM port.
4. LCD shows: `HH:MM:SS AM/PM` (top), `V=xx.xx L=xxx T=xx.xxC` (bottom).
5. UART output example:
   ```
   TIME:09:00:01 AM TEMPERATURE=25.50C voltage=2.45v Light intensity=200
   DATE:05-21-2025 WED
   ```

## Sample Output
```
(time loops every 1s)
TIME:09:00:01 AM TEMPERATURE=25.50C voltage=2.45v Light intensity=200
DATE:05-21-2025 WED

TIME:09:00:02 AM TEMPERATURE=25.52C voltage=2.46v Light intensity=198
DATE:05-21-2025 WED
```

## Notes & Improvements
- Error handling in I2C (prints to UART).
- Temp assumes LM35 (10mV/°C, 0.5V offset).
- Light formula approximate; calibrate LDR.
- Extend: Add more channels, logging, alarms.
- Debug: Use UART for prints.

Project ready to build and run!

