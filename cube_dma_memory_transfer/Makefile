

BUILD_TOOLS_ROOT=~/Downloads/gcc-arm-none-eabi-6-2017-q1-update/bin
ifeq (,$(wildcard $(BUILD_TOOLS_ROOT)))
$(error Please set BUILD_TOOLS_ROOT to point to your copy of ARM gnu toolchain)
endif

FLAGS=-mcpu=cortex-m3 -mthumb -mfloat-abi=soft
CFLAGS='-D__weak=__attribute__((weak))' '-D__packed="__attribute__((__packed__))"' -DSTM32F103xB
CFLAGS+=-Og -g3
# report all warnings. unlimited message length per line. put each function into its
# own section.
CFLAGS+=-Wall -fmessage-length=0 -ffunction-sections -DUSE_FULL_ASSERT 

# generate .d dependencies. MMD is only list user includes; no system files.
# MP is to add phony dependcencies. helps if you remove a file.
# -MMD -MP -MF"Src/main.d" -MT"Src/main.o"

CINCLUDE=-IInc -IDrivers/CMSIS/Device/ST/STM32F1xx/Include -IDrivers/CMSIS/Include -IDrivers/STM32F1xx_HAL_Driver/Inc

SOURCES = $(wildcard Src/*.c)
SOURCES += $(wildcard Drivers/STM32F1xx_HAL_Driver/Src/*.c)

OBJECTS_FULL = $(notdir $(SOURCES))
OBJECTS = $(addprefix build/,$(OBJECTS_FULL:.c=.o))

EFILES = $(addprefix preprocessed/,$(notdir $(SOURCES:.c=.E)))

VPATH = Src:Drivers/STM32F1xx_HAL_Driver/Src

TARGET_NAME = $(notdir $(CURDIR))

# ,--print-gc-sections
# 2>&1 | grep -v '.debug_macro'
$(TARGET_NAME).elf: build/startup_stm32f103xb.o $(OBJECTS)
	@echo curdir $(notdir $(CURDIR))
	@echo soruces $(SOURCES)
	@echo object  $(OBJECTS)
	$(BUILD_TOOLS_ROOT)/arm-none-eabi-gcc $(FLAGS) -specs=nosys.specs -specs=nano.specs -TSTM32F103C8Tx_FLASH.ld -Wl,-Map=output.map -Wl,--gc-sections -lm -o $@ $^ 


build/startup_stm32f103xb.o: startup/startup_stm32f103xb.s
	mkdir -p build
	$(BUILD_TOOLS_ROOT)/arm-none-eabi-as $(FLAGS) -g -o $@ $^

build/%.o: %.c
	mkdir -p build
	$(BUILD_TOOLS_ROOT)/arm-none-eabi-gcc $(FLAGS) $(CFLAGS) $(CINCLUDE) -c -o $@ $^

# -fdirectives-only just process ifdefs
# -C keep the comments
# -imacros get the macros from the file but don't include other stuff in preprocess output
PREPROC_OPTIONS = -fdirectives-only -C

# here's what's happening below:
# 1 get a list of all defines in the file and it's includes. file.DU
# 2 get rid of the standard defines file.DUNS
# 3 rename #includes from hte file being processed .noinc
# 4 remove ifs from the noinc file > .noifs
# 5 remove standard defines from the noifs (cpp has an option for this but it didn't work for me)
# 6 remove non-function defines. remove assert_param and IS_NVIC/IS_MPU functions
preprocessed/%.E: %.c
	mkdir -p preprocessed
	$(BUILD_TOOLS_ROOT)/arm-none-eabi-gcc $(FLAGS) $(CFLAGS) $(CINCLUDE) -E -o $(@:.E=.DU) $^
	# remove standard stuff
	# .. comes from here: http://stackoverflow.com/questions/22228924/how-to-use-sed-to-remove-all-lines-until-first-match-in-first-column
	perl -nle 'print if (/#define STM32F103xB/..0)' $(@:.E=.DU) > $(@:.E=.DUNS)
	perl -pe 's/#include/POUND_INCLUDE/' $^ > $(@:.E=.noinc)
	$(BUILD_TOOLS_ROOT)/arm-none-eabi-cpp $(FLAGS) $(CFLAGS) $(CINCLUDE) -E $(PREPROC_OPTIONS) -imacros $(@:.E=.DUNS) -o $(@:.E=.noifs) $(@:.E=.noinc)
	perl -nle 'print if (/1 "<command-line>" 2/..0)' $(@:.E=.noifs) > $(@:.E=.noifsNS)
	perl -ne 'print if(/#define [a-zA-Z0-9_]+\(/)' $(@:.E=.DUNS) | egrep -v "define (assert_param|IS_MPU|IS_NVIC|IS_SYSTICK|IS_RCC|CLEAR_BIT|CLEAR_REG|MODIFY_REG|READ_BIT)" > $(@:.E=.DUFNS)
	dos2unix $(@:.E=.noifsNS)
	$(BUILD_TOOLS_ROOT)/arm-none-eabi-cpp $(FLAGS) $(CFLAGS) $(CINCLUDE) -E -C -imacros $(@:.E=.DUFNS) -o $(@:.E=.nofns) $(@:.E=.noifsNS)
	perl -nle 's/POUND_INCLUDE/#include/; print if (/1 "<command-line>" 2/..0)' $(@:.E=.nofns) > $(@:.E=.nofnsNS)
	cp $(@:.E=.nofnsNS) $@
	dos2unix $@

clean:
	rm -rf build
	rm -rf preprocessed

preproc: $(EFILES)

vars:
	@echo soruces $(SOURCES)
	@echo object  $(OBJECTS)
	@echo efiles  $(EFILES)
