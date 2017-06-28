



CINCLUDE=-IInc -IDrivers/CMSIS/Device/ST/STM32F1xx/Include -IDrivers/CMSIS/Include -IDrivers/STM32F1xx_HAL_Driver/Inc

SOURCES = $(wildcard Src/*.c)
SOURCES += $(wildcard Drivers/STM32F1xx_HAL_Driver/Src/*.c)

OBJECTS_FULL = $(notdir $(SOURCES))
OBJECTS = $(addprefix build/,$(OBJECTS_FULL:.c=.o))

EFILES = $(addprefix preprocessed/,$(notdir $(SOURCES:.c=.E)))

VPATH += Src:Drivers/STM32F1xx_HAL_Driver/Src

TARGET_NAME = $(notdir $(CURDIR))

# ,--print-gc-sections
# 2>&1 | grep -v '.debug_macro'
$(TARGET_NAME).elf: build/startup_stm32f103xb.o $(OBJECTS)
	@echo curdir $(notdir $(CURDIR))
	@echo soruces $(SOURCES)
	@echo object  $(OBJECTS)
	$(BUILD_TOOLS_ROOT)/arm-none-eabi-gcc $(FLAGS) -specs=nosys.specs -specs=nano.specs -TSTM32F103C8Tx_FLASH.ld -Wl,-Map=output.map -Wl,--gc-sections -lm -o $@ $^ 


include ../Make.common
