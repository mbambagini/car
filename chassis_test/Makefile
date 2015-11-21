PROJECT = chassis_test

CAN_FLAGS = -DNET_TX_STS_BODY \
            -DNET_TX_STS_DIAG \
            -DNET_RX_CMD_BODY \
            -DNET_RX_CMD_ENGINE \
            -DNET_RX_CMD_TIME \
            -DNET_RX_CMD_DIAG
TASK_FLAGS = -DCAN_THREAD_PERIOD=100

GCC_BIN = /usr/bin/

OBJECTS = ../source/can.o \
          ./main.o

SYS_OBJECTS = ../lib/mbed-rtos/rtx/TARGET_CORTEX_M/TARGET_M3/TOOLCHAIN_GCC/SVC_Table.o \
              ../lib/mbed-rtos/rtx/TARGET_CORTEX_M/TARGET_M3/TOOLCHAIN_GCC/HAL_CM3.o \
              ../lib/mbed-rtos/rtx/TARGET_CORTEX_M/rt_Task.o \
              ../lib/mbed-rtos/rtx/TARGET_CORTEX_M/rt_System.o \
              ../lib/mbed-rtos/rtx/TARGET_CORTEX_M/rt_List.o \
              ../lib/mbed-rtos/rtx/TARGET_CORTEX_M/rt_Time.o \
              ../lib/mbed-rtos/rtx/TARGET_CORTEX_M/rt_Semaphore.o \
              ../lib/mbed-rtos/rtx/TARGET_CORTEX_M/rt_Mailbox.o \
              ../lib/mbed-rtos/rtx/TARGET_CORTEX_M/rt_CMSIS.o \
              ../lib/mbed-rtos/rtx/TARGET_CORTEX_M/HAL_CM.o \
              ../lib/mbed-rtos/rtx/TARGET_CORTEX_M/rt_Robin.o \
              ../lib/mbed-rtos/rtx/TARGET_CORTEX_M/rt_MemBox.o \
              ../lib/mbed-rtos/rtx/TARGET_CORTEX_M/RTX_Conf_CM.o \
              ../lib/mbed-rtos/rtx/TARGET_CORTEX_M/rt_Event.o \
              ../lib/mbed-rtos/rtx/TARGET_CORTEX_M/rt_Mutex.o \
              ../lib/mbed/TARGET_LPC1768/TOOLCHAIN_GCC_ARM/board.o \
              ../lib/mbed/TARGET_LPC1768/TOOLCHAIN_GCC_ARM/cmsis_nvic.o \
              ../lib/mbed/TARGET_LPC1768/TOOLCHAIN_GCC_ARM/retarget.o \
              ../lib/mbed/TARGET_LPC1768/TOOLCHAIN_GCC_ARM/startup_LPC17xx.o \
              ../lib/mbed/TARGET_LPC1768/TOOLCHAIN_GCC_ARM/system_LPC17xx.o \
              ../lib/mbed-rtos/rtos/Thread.o \
              ../lib/mbed-rtos/rtos/RtosTimer.o \
              ../lib/mbed-rtos/rtos/Semaphore.o \
              ../lib/mbed-rtos/rtos/Mutex.o
INCLUDE_PATHS = -I. -I../include -I../lib/mbed-rtos -I../lib/mbed-rtos/rtx \
                -I../lib/mbed-rtos/rtx/TARGET_CORTEX_M \
                -I../lib/mbed-rtos/rtx/TARGET_CORTEX_M/TARGET_M3 \
                -I../lib/mbed-rtos/rtx/TARGET_CORTEX_M/TARGET_M3/TOOLCHAIN_GCC \
                -I../lib/mbed-rtos/rtos -I../lib/mbed -I../lib/mbed/TARGET_LPC1768 \
                -I../lib/mbed/TARGET_LPC1768/TARGET_NXP \
                -I../lib/mbed/TARGET_LPC1768/TARGET_NXP/TARGET_LPC176X \
                -I../lib/mbed/TARGET_LPC1768/TARGET_NXP/TARGET_LPC176X/TARGET_MBED_LPC1768 \
                -I../lib/mbed/TARGET_LPC1768/TOOLCHAIN_GCC_ARM 
LIBRARY_PATHS = -L../lib/mbed/TARGET_LPC1768/TOOLCHAIN_GCC_ARM 
LIBRARIES = -lmbed 
LINKER_SCRIPT = ../lib/mbed/TARGET_LPC1768/TOOLCHAIN_GCC_ARM/LPC1768.ld

############################################################################### 
AS      = $(GCC_BIN)arm-none-eabi-as
CC      = $(GCC_BIN)arm-none-eabi-gcc
CPP     = $(GCC_BIN)arm-none-eabi-g++
LD      = $(GCC_BIN)arm-none-eabi-gcc
OBJCOPY = $(GCC_BIN)arm-none-eabi-objcopy
OBJDUMP = $(GCC_BIN)arm-none-eabi-objdump
SIZE    = $(GCC_BIN)arm-none-eabi-size 


CPU = -mcpu=cortex-m3 -mthumb 
CC_FLAGS = $(CPU) -c -g -fno-common -fmessage-length=0 -Wall -Wextra -fno-exceptions -ffunction-sections -fdata-sections -fomit-frame-pointer -MMD -MP
CC_SYMBOLS = -DTOOLCHAIN_GCC_ARM -DTOOLCHAIN_GCC -DMBED_BUILD_TIMESTAMP=1446927501.85 -DARM_MATH_CM3 -DTARGET_CORTEX_M -DTARGET_LPC176X -DTARGET_NXP -DTARGET_MBED_LPC1768 -DTARGET_LPC1768 -D__CORTEX_M3 -DTARGET_M3 -D__MBED__=1 

LD_FLAGS = $(CPU) -Wl,--gc-sections --specs=nano.specs -u _printf_float -u _scanf_float -Wl,--wrap,main -Wl,-Map=$(PROJECT).map,--cref
LD_SYS_LIBS = -lstdc++ -lsupc++ -lm -lc -lgcc -lnosys


ifeq ($(DEBUG), 1)
  CC_FLAGS += -DDEBUG -O0
else
  CC_FLAGS += -DNDEBUG -Os
endif

.PHONY: all clean lst size

all: $(PROJECT).bin $(PROJECT).hex size


clean:
	@rm -f $(PROJECT).bin $(PROJECT).elf $(PROJECT).hex $(PROJECT).map $(PROJECT).lst $(OBJECTS) $(DEPS)


.asm.o:
	@$(CC) $(CPU) -c -x assembler-with-cpp -o $@ $<
.s.o:
	@$(CC) $(CPU) -c -x assembler-with-cpp -o $@ $<
.S.o:
	@$(CC) $(CPU) -c -x assembler-with-cpp -o $@ $<

.c.o:
	@$(CC)  $(CC_FLAGS) $(CC_SYMBOLS) $(CAN_FLAGS) $(TASK_FLAGS) -std=gnu99   $(INCLUDE_PATHS) -o $@ $<

.cpp.o:
	@$(CPP) $(CC_FLAGS) $(CC_SYMBOLS) $(CAN_FLAGS) $(TASK_FLAGS) -std=gnu++98 -fno-rtti $(INCLUDE_PATHS) -o $@ $<


$(PROJECT).elf: $(OBJECTS) $(SYS_OBJECTS)
	@$(LD) $(LD_FLAGS) -T$(LINKER_SCRIPT) $(LIBRARY_PATHS) -o $@ $^ $(LIBRARIES) $(LD_SYS_LIBS) $(LIBRARIES) $(LD_SYS_LIBS)

	@echo ""
	@echo "*****"
	@echo "***** You must modify vector checksum value in *.bin and *.hex files."
	@echo "*****"
	@echo ""


$(PROJECT).bin: $(PROJECT).elf
	@$(OBJCOPY) -O binary $< $@

$(PROJECT).hex: $(PROJECT).elf
	@$(OBJCOPY) -O ihex $< $@

$(PROJECT).lst: $(PROJECT).elf
	@$(OBJDUMP) -Sdh $< > $@

lst: $(PROJECT).lst

size: $(PROJECT).elf
	$(SIZE) $(PROJECT).elf

DEPS = $(OBJECTS:.o=.d)
-include $(DEPS)


