

include ../Make.common

CINCLUDE=-IInc -IDrivers/CMSIS/Device/ST/STM32F1xx/Include -IDrivers/CMSIS/Include

bareblink.elf: build/startup_stm32f103xb.o build/main.o build/system_stm32f1xx.o
	$(BUILD_TOOLS_ROOT)/arm-none-eabi-gcc $(FLAGS) -specs=nosys.specs -specs=nano.specs -TSTM32F103C8Tx_FLASH.ld -Wl,-Map=output.map -Wl,--gc-sections -lm -o $@ $^ 



