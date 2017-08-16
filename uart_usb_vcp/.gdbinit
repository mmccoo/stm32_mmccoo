#export GDBEXEC=/bubba/electronicsDS/stm32/gcc/gcc-arm-none-eabi-6-2017-q1-update/bin/arm-none-eabi-gdb-py
#export OPENOCD=/bubba/electronicsDS/stm32/openocd-0.10.0/install

file build/uart_usb_vcp.elf
target remote | $OPENOCD/bin/openocd -f $OPENOCD/share/openocd/scripts/interface/stlink-v2.cfg -f $OPENOCD/share/openocd/scripts/target/stm32f1x.cfg -c "gdb_port pipe; log_output openocd.log"

# target remote localhost:3333

python
import sys;
sys.path.append("../gdb_pretty_printer")
import stm32_prettyprint
stm32_prettyprint.register_printers(None)
end

monitor reset halt
monitor stm32f1x mass_erase 0
monitor program build/uart_usb_vcp.elf verify
monitor reset halt
#break main


# break main.c:93
# commands
#   silent
#   printf "send1 %d %d %s\n", tosend_begin, end-tosend_begin, (tosend+tosend_begin)[0]@(end-tosend_begin)
#   continue
# end

#ignore $bpnum 1


continue
