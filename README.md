# msp430-libraries

A bunch of libraries i have written for MSP430 devices while working on a
students project during my studies. Everything has ben written from scratch,
using none of the TI libraries or TI-RTOS. You can basically start with an
empty project.

For low footprint embedded devices without extensive library usage, feel free
to check out my other Repos, TTDelay and lovelyFSM (specifically the
light version).

Some of these libraries have flown to space (well, almost. As high as a weather
balloon can fly). However, they are not battle tested and provided as is. Feel
free to use them as a base, but make sure you test before using any of these.

Contains C libraries for the following interfaces:
- Low Power Sleep
- I2C
- OneWire
- ADC (only F5529)
- On-Chip Flash Memory (only G2553)

Contains C libraries for the following devices:
- BQ32002 RTC
- NRF24L01 2.4 GHz RF Module (this was dangled together until it kind of worked)
- WS2811B programmable LED
- BMP085 Athmospheric Pressure + Temperature Sensor
- MS5803 Athmospheric Pressure + Temperature Sensor
- Si7021 Athmospheric Pressure + Temperature Sensor
- DS1820 Temperature Sensor
- SSD1306 I2C OLED Display
- 24xx EEPROMs

You might need to adjust some of the libraries to use proper function names for
hardware interfaces (e.g. on the F5529 the I2C interface functions are prefixed
I2C0_... and I2C1_..., while on the G2553 with a single interface they are
prefixed using I2C_).
