#export GDBEXEC=/bubba/electronicsDS/stm32/gcc/gcc-arm-none-eabi-6-2017-q1-update/bin/arm-none-eabi-gdb-py
#export OPENOCD=/bubba/electronicsDS/stm32/openocd-0.10.0/install


file build/basic_uart.elf
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
monitor program build/basic_uart.elf verify
monitor reset halt
end

reload
break main
break HAL_UART_RxCpltCallback
continue
