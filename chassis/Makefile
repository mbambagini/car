PROJECT = chassis

# task configuration
TASK_FLAGS    = -DCAN_THREAD_PERIOD=25      \
                -DENGINE_THREAD_PERIOD=50   \
                -DBODY_THREAD_PERIOD=100    \
                -DDIAG_THREAD_PERIOD=500    \
                -DCLOCK_THREAD_PERIOD=1000

# objects
SOURCE_FILES  = body.cpp diag.cpp engine.cpp led.cpp clock.cpp main.cpp
SHARED_FILES  = can.cpp
OBJECT_DIR    = ./obj
SOURCE_DIR    = ./src
SHARED_DIR    = ../shared/src
SOURCE_OBJ    = ${SOURCE_FILES:%.cpp=$(OBJECT_DIR)/%.o}
SHARED_OBJ    = ${SHARED_FILES:%.cpp=$(OBJECT_DIR)/%.o}

# includes and libraries
INCLUDE_PATHS = -I.                                                                            \
                -I./include                                                                    \
                -I../shared/include                                                            \
                -I../shared/mbed                                                               \
                -I../shared/mbed/TARGET_LPC1768                                                \
                -I../shared/mbed/TARGET_LPC1768/TARGET_NXP                                     \
                -I../shared/mbed/TARGET_LPC1768/TARGET_NXP/TARGET_LPC176X                      \
                -I../shared/mbed/TARGET_LPC1768/TARGET_NXP/TARGET_LPC176X/TARGET_MBED_LPC1768  \
                -I../shared/mbed/TARGET_LPC1768/TOOLCHAIN_GCC_ARM                              \
                -I../shared/rtos                                                               \
                -I../shared/rtos/TARGET_CORTEX_M                                               \
                -I../shared/rtos/TARGET_LPC1768/TOOLCHAIN_GCC_ARM                              \
                -I/usr/lib/arm-none-eabi/include/                                              \
                -I/usr/lib/arm-none-eabi/include/c++/4.8/                                      \
                -I/usr/lib/arm-none-eabi/include/c++/4.8/arm-none-eabi

LIBRARY_PATHS = -L../shared/rtos/TARGET_LPC1768/TOOLCHAIN_GCC_ARM   \
                -L../shared/mbed/TARGET_LPC1768/TOOLCHAIN_GCC_ARM   \
                -L../shared/lib

LIBRARIES = -lmbed -lrtos -lrtx -lservo

LINKER_SCRIPT = ../shared/mbed/TARGET_LPC1768/TOOLCHAIN_GCC_ARM/LPC1768.ld

SYS_OBJECTS = ../shared/mbed/TARGET_LPC1768/TOOLCHAIN_GCC_ARM/board.o            \
              ../shared/mbed/TARGET_LPC1768/TOOLCHAIN_GCC_ARM/cmsis_nvic.o       \
              ../shared/mbed/TARGET_LPC1768/TOOLCHAIN_GCC_ARM/retarget.o         \
              ../shared/mbed/TARGET_LPC1768/TOOLCHAIN_GCC_ARM/startup_LPC17xx.o  \
              ../shared/mbed/TARGET_LPC1768/TOOLCHAIN_GCC_ARM/system_LPC17xx.o

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
CC_FLAGS = $(CPU) -c -g -fno-common -fmessage-length=0 -Wall -Wextra   \
                  -fno-exceptions -ffunction-sections -fdata-sections  \
                  -fomit-frame-pointer -MMD -MP
CC_SYMBOLS = -DTOOLCHAIN_GCC_ARM -DTOOLCHAIN_GCC                       \
             -DMBED_BUILD_TIMESTAMP=1446927176.42 -DARM_MATH_CM3       \
             -DTARGET_CORTEX_M -DTARGET_LPC176X -DTARGET_NXP           \
             -DTARGET_MBED_LPC1768 -DTARGET_LPC1768 -D__CORTEX_M3      \
             -DTARGET_M3 -D__MBED__=1 

# link arguments
LD_FLAGS = $(CPU) -Wl,--gc-sections --specs=nano.specs -u _printf_float \
                  -u _scanf_float -Wl,--wrap,main \
                  -Wl,-Map=$(PROJECT).map,--cref
LD_SYS_LIBS = -lstdc++ -lsupc++ -lm -lc -lgcc -lnosys

.PHONY: all clean lst size

all: $(OBJECT_DIR) $(PROJECT).bin $(PROJECT).hex size

clean:
	@rm -f $(PROJECT).bin $(PROJECT).elf $(PROJECT).hex $(PROJECT).map $(PROJECT).lst $(DEPS)
	@rm -rf $(OBJECT_DIR)

$(OBJECT_DIR):
	@mkdir $(OBJECT_DIR)

# compile
$(OBJECT_DIR)/%.o : $(SOURCE_DIR)/%.cpp
	@$(CPP) $(CC_FLAGS) $(CC_SYMBOLS) $(TASK_FLAGS) -std=gnu++98 -fno-rtti $(INCLUDE_PATHS) -o $@ $<

$(OBJECT_DIR)/%.o : $(SHARED_DIR)/%.cpp
	@$(CPP) $(CC_FLAGS) $(CC_SYMBOLS) $(TASK_FLAGS) -std=gnu++98 -fno-rtti $(INCLUDE_PATHS) -o $@ $<

# linking
$(PROJECT).elf: $(SOURCE_OBJ) $(SHARED_OBJ) $(SYS_OBJECTS)
	@$(LD) $(LD_FLAGS) -T$(LINKER_SCRIPT) $(LIBRARY_PATHS) -o $@ $^ $(LIBRARIES) $(LD_SYS_LIBS) $(LIBRARIES) $(LD_SYS_LIBS)

# binaries
$(PROJECT).bin: $(PROJECT).elf
	@$(OBJCOPY) -O binary $< $@

$(PROJECT).hex: $(PROJECT).elf
	@$(OBJCOPY) -O ihex $< $@

# other files
$(PROJECT).lst: $(PROJECT).elf
	@$(OBJDUMP) -Sdh $< > $@

size: $(PROJECT).elf
	@echo "SIZE: $(PROJECT)"
	@$(SIZE) $(PROJECT).elf

lst: $(PROJECT).lst

# dependencies
DEPS = $(OBJECTS:.o=.d) $(SYS_OBJECTS:.o=.d)
-include $(DEPS)

