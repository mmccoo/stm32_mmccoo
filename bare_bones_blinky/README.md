
This directory contains a bare bones blinky program for STM32F103
# Prerequisites
From a virgin ubuntu 16.04, the following need to be installed:

## openocd
```sudo apt install openocd```

### udev rule
The following needs to go into the file /etc/udev/rules.d/49-stlinkv2.rules

```
# stm32 discovery boards, with onboard st/linkv2
# ie, STM32L, STM32F4.
# STM32VL has st/linkv1, which is quite different

SUBSYSTEMS=="usb", ATTRS{idVendor}=="0483", ATTRS{idProduct}=="3748", \
    MODE:="0666", \
SYMLINK+="stlinkv2_%n"
```

## Basic test
```openocd -f /usr/share/openocd/scripts/interface/stlink-v2.cfg -f /usr/share/openocd/scripts/target/stm32f1x.cfg```

If you get this ```Info : stm32f1x.cpu: hardware has 6 breakpoints, 4 watchpoints```, you're probably fine.


## ARM GNU toolchain (g++, gdb,...)

Download the GNU toolchain from the [ARM download area](https://developer.arm.com/open-source/gnu-toolchain/gnu-rm/downloads)
# I imagine this link will change over time.
# wget https://armkeil.blob.core.windows.net/developer/Files/downloads/gnu-rm/6_1-2017q1/gcc-arm-none-eabi-6-2017-q1-update-linux.tar.bz2

Untar it:
```tar -xf gcc-arm-none-eabi-6-2017-q1-update-linux.tar.bz2```

# Build bare bones blinky

Modify the Makefile to point at wherever you untarred the gcc stuff. It's the first line of the Makefile

```make```

This should yield the file bareblink.elf


# install elf via gdb

In this directory, there is a .gdbinit file. Upon invocation of gdb, it'll load your elf file onto your BluePill/STM32 board.

```~/Downloads/gcc-arm-none-eabi-6-2017-q1-update/bin/arm-none-eabi-gdb-py```

Make sure you're running the ARM version of gdb. You may also have linux gdb on your system, which won't work.

If you get the error ```Scripting in the "Python" language is not supported in this copy of GDB.```, it's because you didn't run the py version of gdb. It's in the same directory.


You may need to add a line like this to your $HOME/.gdbinit
```add-auto-load-safe-path /home/mmccoo/share/stm32_mmccoo/bare_bones_blinky/.gdbinit```


