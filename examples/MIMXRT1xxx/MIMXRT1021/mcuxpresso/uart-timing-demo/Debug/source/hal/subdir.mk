################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/hal/buffer.c \
../source/hal/buffer_printf.c \
../source/hal/hal_timing.c \
../source/hal/hal_uart.c \
../source/hal/list.c \
../source/hal/task.c \
../source/hal/timing.c \
../source/hal/uart.c 

C_DEPS += \
./source/hal/buffer.d \
./source/hal/buffer_printf.d \
./source/hal/hal_timing.d \
./source/hal/hal_uart.d \
./source/hal/list.d \
./source/hal/task.d \
./source/hal/timing.d \
./source/hal/uart.d 

OBJS += \
./source/hal/buffer.o \
./source/hal/buffer_printf.o \
./source/hal/hal_timing.o \
./source/hal/hal_uart.o \
./source/hal/list.o \
./source/hal/task.o \
./source/hal/timing.o \
./source/hal/uart.o 


# Each subdirectory must supply rules for building sources it contributes
source/hal/%.o: ../source/hal/%.c source/hal/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -std=gnu99 -D__REDLIB__ -DCPU_MIMXRT1021DAG5A -DCPU_MIMXRT1021DAG5A_cm7 -DSDK_DEBUGCONSOLE=1 -DXIP_EXTERNAL_FLASH=1 -DXIP_BOOT_HEADER_ENABLE=1 -DMCUXPRESSO_SDK -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -I"/home/asmir/Documents/MCUXpresso_11.6.1_8255/workspace/uart-timing-demo/source" -I"/home/asmir/Documents/MCUXpresso_11.6.1_8255/workspace/uart-timing-demo/utilities" -I"/home/asmir/Documents/MCUXpresso_11.6.1_8255/workspace/uart-timing-demo/drivers" -I"/home/asmir/Documents/MCUXpresso_11.6.1_8255/workspace/uart-timing-demo/device" -I"/home/asmir/Documents/MCUXpresso_11.6.1_8255/workspace/uart-timing-demo/component/uart" -I"/home/asmir/Documents/MCUXpresso_11.6.1_8255/workspace/uart-timing-demo/component/lists" -I"/home/asmir/Documents/MCUXpresso_11.6.1_8255/workspace/uart-timing-demo/xip" -I"/home/asmir/Documents/MCUXpresso_11.6.1_8255/workspace/uart-timing-demo/component/silicon_id" -I"/home/asmir/Documents/MCUXpresso_11.6.1_8255/workspace/uart-timing-demo/CMSIS" -I"/home/asmir/Documents/MCUXpresso_11.6.1_8255/workspace/uart-timing-demo/board" -I"/home/asmir/Documents/MCUXpresso_11.6.1_8255/workspace/uart-timing-demo/evkmimxrt1020/driver_examples/gpio/led_output" -O0 -fno-common -g3 -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m7 -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-source-2f-hal

clean-source-2f-hal:
	-$(RM) ./source/hal/buffer.d ./source/hal/buffer.o ./source/hal/buffer_printf.d ./source/hal/buffer_printf.o ./source/hal/hal_timing.d ./source/hal/hal_timing.o ./source/hal/hal_uart.d ./source/hal/hal_uart.o ./source/hal/list.d ./source/hal/list.o ./source/hal/task.d ./source/hal/task.o ./source/hal/timing.d ./source/hal/timing.o ./source/hal/uart.d ./source/hal/uart.o

.PHONY: clean-source-2f-hal

