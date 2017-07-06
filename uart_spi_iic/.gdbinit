
#export GDBEXEC=/bubba/electronicsDS/stm32/gcc/gcc-arm-none-eabi-6-2017-q1-update/bin/arm-none-eabi-gdb-py
#export OPENOCD=/bubba/electronicsDS/stm32/openocd-0.10.0/install

file build/uart_spi_iic.elf
target remote | $OPENOCD/bin/openocd -f $OPENOCD/share/openocd/scripts/interface/stlink-v2.cfg -f $OPENOCD/share/openocd/scripts/target/stm32f1x.cfg -c "gdb_port pipe; log_output openocd.log"

# target remote localhost:3333

python
import sys;
sys.path.append("../gdb_pretty_printer")
import stm32_prettyprint
stm32_prettyprint.register_printers(None)
end

define restart
  monitor reset halt
end

define reload
monitor reset halt
monitor stm32f1x mass_erase 0
monitor program build/uart_spi_iic.elf verify
monitor reset halt
end

reload
break main
#break Src/stm32f1xx_hal_i2c.c:340
#break u8x8_gpio_and_delay_mine
#break u8x8_byte_my_hw_i2c
continue
