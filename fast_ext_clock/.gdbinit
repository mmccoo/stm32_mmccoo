file build/fast_ext_clock.elf
# if you have openocd install via apt install, then you'll want export=OPENOCD=/usr
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
monitor program build/fast_ext_clock.elf verify
monitor reset halt
