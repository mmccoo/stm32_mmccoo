# Introduction

This repo contains some stuff I'm finding useful in trying to use the cheap STM32F103C8T6
AKA Blue Pill (I believe this is a reference from the movie The Matrix).
Cost is $1.75 on AliExpress. The only other thing you'll need is an STMLink V2 USB programmer costing $1.91 on AliExpress. Both prices are with shipping, so you're looking at an investment of <$4

Each of the directories should have a README.md file with further instructions on what
that directory is about.

# What's in the directories?

## gdb_pretty_printer
contains a set of gdb pretty printers for displaying control
registers. Instead of this:
```
(gdb) print/r *GPIOC
$3 = {CRL = 1145324612, CRH = 1148470340, IDR = 57344, ODR = 8192, BSRR = 0, BRR = 0, LCKR = 0}
```
you can get this (pin 13 is set as output and high):
```
(gdb) print GPIOC
$1 = {0 = Input Floating 0, 1 = Input Floating 0, 2 = Input Floating 0, 3 = Input Floating 0, 4 = Input Floating 0, 5 = Input Floating 0, 
  6 = Input Floating 0, 7 = Input Floating 0, 8 = Input Floating 0, 9 = Input Floating 0, 10 = Input Floating 0, 11 = Input Floating 0, 
  12 = Input Floating 0, 13 = Output 50Mhz Open-drain:1, 14 = Input Floating 1, 15 = Input Floating 1}
```

Similarly for RCC
```
(gdb) print RCC
$1 = RCC: = {
  CR = {PLLRDY = "PLL unlocked", PLLON = "PLL OFF", CSSON = "Clock detector OFF", HSEBYP = "External 4-16 MHz osc is not bypassed", HSERDY = "Osc not ready", HSEON = "HSE Osc OFF", HSICAL = 74, HSITRIM = 16, HSIRDY = "Internal 8MHz RC Osc ready", HSION = "Internal 8MHz RC Osc ON"}, 
  CFGR = {MCO = No Val, USBPRE = "PLL clock is divided by 1.5", PLLMUL = "PLL input clock x 2", PLLXTPRE = "HSE clock not divided", PLLSRC = "HSI Osc clock/2 selected as PLL input", ADCPRE = "PCLK2 divided by 2", PPRE2 = No Val, PPRE1 = No Val, HPRE = No Val, SWS = "HSI osc used as system clock", SW = "PLL selected as system clock"}}
```

## barebones, blinky
contains a blinky program that requires only:
- The [ARM gcc tools](https://developer.arm.com/open-source/gnu-toolchain/gnu-rm)
- [OpenOcd](http://openocd.org/)

## blinky
A simple program to blink the LED. Unlike barebone blinky, this one uses the hardware abstraction layer provided by ST. Additionally, I used STMCubeMX to generate much of it.

## basic_uart
This builds on blinky by adding UART capabilities. It sends messages that can be received in a terminal window via a cheap ftdi adapter (also <$2 on AliExpress). It also echos back to you whatever you type in the terminal.

## uart_and_spi
While uart enables you to talk to your device, you probably want it to talk to other electronic components. SPI is a common way to talk to other devices.
This example demonstrates how to send SPI messages.

## uart_spi_iic
In addition to SPI, I2C is another common way to interface with electronic devices.


## uart_spi_usb
If you don't want to be dependent on a FTDI adapter, the blue pill module does have a usb port you can use.