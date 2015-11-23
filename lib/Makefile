GCC_BIN = /usr/bin/

OBJECT_LPC = ../lib/mbed/TARGET_LPC1768/TOOLCHAIN_GCC_ARM/board.o \
             ../lib/mbed/TARGET_LPC1768/TOOLCHAIN_GCC_ARM/cmsis_nvic.o \
             ../lib/mbed/TARGET_LPC1768/TOOLCHAIN_GCC_ARM/retarget.o \
             ../lib/mbed/TARGET_LPC1768/TOOLCHAIN_GCC_ARM/startup_LPC17xx.o \
             ../lib/mbed/TARGET_LPC1768/TOOLCHAIN_GCC_ARM/system_LPC17xx.o

OBJECT_OS = ./mbed-rtos/rtx/TARGET_CORTEX_M/TARGET_M3/TOOLCHAIN_GCC/HAL_CM3.o \
            ./mbed-rtos/rtx/TARGET_CORTEX_M/TARGET_M3/TOOLCHAIN_GCC/SVC_Table.o \
            ./mbed-rtos/rtx/TARGET_CORTEX_M/HAL_CM.o \
            ./mbed-rtos/rtx/TARGET_CORTEX_M/RTX_Conf_CM.o \
            ./mbed-rtos/rtx/TARGET_CORTEX_M/rt_CMSIS.o \
            ./mbed-rtos/rtx/TARGET_CORTEX_M/rt_Event.o \
            ./mbed-rtos/rtx/TARGET_CORTEX_M/rt_List.o \
            ./mbed-rtos/rtx/TARGET_CORTEX_M/rt_Mailbox.o \
            ./mbed-rtos/rtx/TARGET_CORTEX_M/rt_MemBox.o \
            ./mbed-rtos/rtx/TARGET_CORTEX_M/rt_Mutex.o \
            ./mbed-rtos/rtx/TARGET_CORTEX_M/rt_Robin.o \
            ./mbed-rtos/rtx/TARGET_CORTEX_M/rt_Semaphore.o \
            ./mbed-rtos/rtx/TARGET_CORTEX_M/rt_System.o \
            ./mbed-rtos/rtx/TARGET_CORTEX_M/rt_Task.o \
            ./mbed-rtos/rtx/TARGET_CORTEX_M/rt_Time.o \
            ./mbed-rtos/rtos/Mutex.o \
            ./mbed-rtos/rtos/RtosTimer.o \
            ./mbed-rtos/rtos/Semaphore.o \
            ./mbed-rtos/rtos/Thread.o \
            ./mbed-rtos/rtx/TARGET_CORTEX_M/HAL_CM.o \
            ./mbed-rtos/rtx/TARGET_CORTEX_M/RTX_Conf_CM.o

OBJECT_ANDROID = ./AndroidAccessory/AndroidAccessory.o \
                 ./AndroidAccessory/USBHost/USBHost.o \
                 ./AndroidAccessory/USBHost/USBHost_log.o

OBJECT_SERVO = ./Servo/Servo.o

INCLUDE_PATHS = -I./Servo \
                -I./mbed-rtos \
                -I./mbed-rtos/rtos \
                -I./mbed-rtos/rtx \
                -I./mbed-rtos/rtx/TARGET_CORTEX_M \
                -I./mbed-rtos/rtx/TARGET_CORTEX_M/TARGET_M3 \
                -I./mbed-rtos/rtx/TARGET_CORTEX_M/TARGET_M3/TOOLCHAIN_GCC \
                -I./mbed -I./mbed/TARGET_LPC1768 \
                -I./mbed/TARGET_LPC1768/TARGET_NXP \
                -I./mbed/TARGET_LPC1768/TARGET_NXP/TARGET_LPC176X \
                -I./mbed/TARGET_LPC1768/TARGET_NXP/TARGET_LPC176X/TARGET_MBED_LPC1768 \
                -I./mbed/TARGET_LPC1768/TOOLCHAIN_GCC_ARM \
                -I./AndroidAccessory/ \
                -I./AndroidAccessory/USBHost/

############################################################################### 
AR      = $(GCC_BIN)arm-none-eabi-ar
AS      = $(GCC_BIN)arm-none-eabi-as
CC      = $(GCC_BIN)arm-none-eabi-gcc
CPP     = $(GCC_BIN)arm-none-eabi-g++
LD      = $(GCC_BIN)arm-none-eabi-gcc
OBJCOPY = $(GCC_BIN)arm-none-eabi-objcopy
OBJDUMP = $(GCC_BIN)arm-none-eabi-objdump
SIZE    = $(GCC_BIN)arm-none-eabi-size 


CPU = -mcpu=cortex-m3 -mthumb 
CC_FLAGS = $(CPU) -c -g -fno-common -fmessage-length=0 -Wall -Wextra \
                  -fno-exceptions -ffunction-sections -fdata-sections \
                  -fomit-frame-pointer -MMD -MP
CC_SYMBOLS = -DTOOLCHAIN_GCC_ARM -DTOOLCHAIN_GCC \
             -DMBED_BUILD_TIMESTAMP=1446927176.42 -DARM_MATH_CM3 \
             -DTARGET_CORTEX_M -DTARGET_LPC176X -DTARGET_NXP \
             -DTARGET_MBED_LPC1768 -DTARGET_LPC1768 -D__CORTEX_M3 \
             -DTARGET_M3 -D__MBED__=1 

.PHONY: all clean

all: libmbed-rtos.a libservo.a liblpc1768.a
#libandroid.a

liblpc1768.a: $(OBJECT_LPC)
	@$(AR) rcs $@ $^

libmbed-rtos.a: $(OBJECT_OS)
	@$(AR) rcs $@ $^

#libandroid.a: $(OBJECT_ANDROID)
#	@$(AR) rcs $@ $^

libservo.a: $(OBJECT_SERVO)
	@$(AR) rcs $@ $^

clean:
	@rm -f $(OBJECT_OS) $(OBJECT_ANDROID) $(OBJECT_SERVO) $(DEPS) libmbed-rtos.a libandroid.a libservo.a

.asm.o:
	$(CC) $(CPU) -c -x assembler-with-cpp -o $@ $<
.s.o:
	$(CC) $(CPU) -c -x assembler-with-cpp -o $@ $<
.S.o:
	$(CC) $(CPU) -c -x assembler-with-cpp -o $@ $<

.c.o:
	$(CC)  $(CC_FLAGS) $(CC_SYMBOLS) $(CAN_FLAGS) $(TASK_FLAGS) -std=gnu99 $(INCLUDE_PATHS) -o $@ $<

.cpp.o:
	$(CPP) $(CC_FLAGS) $(CC_SYMBOLS) $(CAN_FLAGS) $(TASK_FLAGS) -std=gnu++98 -fno-rtti $(INCLUDE_PATHS) -o $@ $<

DEPS = $(OBJECTS:.o=.d)
-include $(DEPS)

