PROJECT = chassis

# CAN configuration
CAN_FLAGS = -DNET_TX_CMD_BODY \
            -DNET_TX_CMD_ENGINE \
            -DNET_TX_CMD_TIME \
            -DNET_TX_CMD_DIAG \
            -DNET_RX_STS_BODY \
            -DNET_RX_STS_DIAG
# task configuration
TASK_FLAGS = -DCLOCK_THREAD_PERIOD=1000 \
             -DENGINE_THREAD_PERIOD=250 \
             -DBODY_THREAD_PERIOD=500 \
             -DDIAG_THREAD_PERIOD=500 \
             -DCAN_THREAD_PERIOD=100

# objects
OBJECTS = ../source/can.o \
          ./src/body.o \
          ./src/clock.o \
          ./src/diag.o \
          ./src/engine.o \
          ./src/led.o \
          ./main.o

INCLUDE_PATHS = -I. -I./inc -I../include -I../lib/Servo -I../lib/mbed-rtos \
                -I../lib/mbed-rtos/rtos -I../lib/mbed-rtos/rtx \
                -I../lib/mbed-rtos/rtx/TARGET_CORTEX_M \
                -I../lib/mbed-rtos/rtx/TARGET_CORTEX_M/TARGET_M3 \
                -I../lib/mbed-rtos/rtx/TARGET_CORTEX_M/TARGET_M3/TOOLCHAIN_GCC \
                -I../lib/mbed -I../lib/mbed/TARGET_LPC1768 \
                -I../lib/mbed/TARGET_LPC1768/TARGET_NXP \
                -I../lib/mbed/TARGET_LPC1768/TARGET_NXP/TARGET_LPC176X \
                -I../lib/mbed/TARGET_LPC1768/TARGET_NXP/TARGET_LPC176X/TARGET_MBED_LPC1768 \
                -I../lib/mbed/TARGET_LPC1768/TOOLCHAIN_GCC_ARM 
LIBRARY_PATHS = -L../lib/mbed/TARGET_LPC1768/TOOLCHAIN_GCC_ARM \
                -L../lib
LIBRARIES = -lmbed -lservo -lmbed-rtos -llpc1768
LINKER_SCRIPT = ../lib/mbed/TARGET_LPC1768/TOOLCHAIN_GCC_ARM/LPC1768.ld

###############################################################################
GCC_BIN = /usr/bin/
AS      = $(GCC_BIN)arm-none-eabi-as
CC      = $(GCC_BIN)arm-none-eabi-gcc
CPP     = $(GCC_BIN)arm-none-eabi-g++
LD      = $(GCC_BIN)arm-none-eabi-gcc
OBJCOPY = $(GCC_BIN)arm-none-eabi-objcopy
OBJDUMP = $(GCC_BIN)arm-none-eabi-objdump
SIZE    = $(GCC_BIN)arm-none-eabi-size 

# compile arguments
CPU = -mcpu=cortex-m3 -mthumb 
CC_FLAGS = $(CPU) -c -g -fno-common -fmessage-length=0 -Wall -Wextra \
                  -fno-exceptions -ffunction-sections -fdata-sections \
                  -fomit-frame-pointer -MMD -MP
CC_SYMBOLS = -DTOOLCHAIN_GCC_ARM -DTOOLCHAIN_GCC \
             -DMBED_BUILD_TIMESTAMP=1446927176.42 -DARM_MATH_CM3 \
             -DTARGET_CORTEX_M -DTARGET_LPC176X -DTARGET_NXP \
             -DTARGET_MBED_LPC1768 -DTARGET_LPC1768 -D__CORTEX_M3 \
             -DTARGET_M3 -D__MBED__=1 

# link arguments
LD_FLAGS = $(CPU) -Wl,--gc-sections --specs=nano.specs -u _printf_float \
                  -u _scanf_float -Wl,--wrap,main \
                  -Wl,-Map=$(PROJECT).map,--cref
LD_SYS_LIBS = -lstdc++ -lsupc++ -lm -lc -lgcc -lnosys

# debug option
ifeq ($(DEBUG), 1)
  CC_FLAGS += -DDEBUG -O0
endif

.PHONY: all clean lst size

all: $(PROJECT).bin $(PROJECT).hex size

clean:
	@rm -f $(PROJECT).bin $(PROJECT).elf $(PROJECT).hex $(PROJECT).map $(PROJECT).lst $(OBJECTS) $(DEPS)

# source
.asm.o:
	$(CC) $(CPU) -c -x assembler-with-cpp -o $@ $<
.s.o:
	$(CC) $(CPU) -c -x assembler-with-cpp -o $@ $<
.S.o:
	$(CC) $(CPU) -c -x assembler-with-cpp -o $@ $<
.c.o:
	$(CC)  $(CC_FLAGS) $(CC_SYMBOLS) $(CAN_FLAGS) $(TASK_FLAGS) -std=gnu99   $(INCLUDE_PATHS) -o $@ $<
.cpp.o:
	$(CPP) $(CC_FLAGS) $(CC_SYMBOLS) $(CAN_FLAGS) $(TASK_FLAGS) -std=gnu++98 -fno-rtti $(INCLUDE_PATHS) -o $@ $<

# linking
$(PROJECT).elf: $(OBJECTS)
	$(LD) $(LD_FLAGS) -T$(LINKER_SCRIPT) $(LIBRARY_PATHS) -o $@ $^ $(LIBRARIES) $(LD_SYS_LIBS) $(LIBRARIES) $(LD_SYS_LIBS)

# binaries
$(PROJECT).bin: $(PROJECT).elf
	$(OBJCOPY) -O binary $< $@
$(PROJECT).hex: $(PROJECT).elf
	@$(OBJCOPY) -O ihex $< $@

# other files
$(PROJECT).lst: $(PROJECT).elf
	@$(OBJDUMP) -Sdh $< > $@
size: $(PROJECT).elf
	@echo "SIZE CHASSIS:"  >> ../sizes
	$(SIZE) $(PROJECT).elf >> ../sizes
lst: $(PROJECT).lst

# dependencies
DEPS = $(OBJECTS:.o=.d)
-include $(DEPS)
