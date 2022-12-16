################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../board/board.c \
../board/clock_config.c \
../board/dcd.c \
../board/pin_mux.c 

C_DEPS += \
./board/board.d \
./board/clock_config.d \
./board/dcd.d \
./board/pin_mux.d 

OBJS += \
./board/board.o \
./board/clock_config.o \
./board/dcd.o \
./board/pin_mux.o 


# Each subdirectory must supply rules for building sources it contributes
board/%.o: ../board/%.c board/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -std=gnu99 -D__REDLIB__ -DCPU_MIMXRT1021DAG5A -DCPU_MIMXRT1021DAG5A_cm7 -DSDK_DEBUGCONSOLE=1 -DXIP_EXTERNAL_FLASH=1 -DXIP_BOOT_HEADER_ENABLE=1 -DMCUXPRESSO_SDK -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -I"/home/asmir/Documents/MCUXpresso_11.6.1_8255/workspace/uart-timing-demo/source" -I"/home/asmir/Documents/MCUXpresso_11.6.1_8255/workspace/uart-timing-demo/utilities" -I"/home/asmir/Documents/MCUXpresso_11.6.1_8255/workspace/uart-timing-demo/drivers" -I"/home/asmir/Documents/MCUXpresso_11.6.1_8255/workspace/uart-timing-demo/device" -I"/home/asmir/Documents/MCUXpresso_11.6.1_8255/workspace/uart-timing-demo/component/uart" -I"/home/asmir/Documents/MCUXpresso_11.6.1_8255/workspace/uart-timing-demo/component/lists" -I"/home/asmir/Documents/MCUXpresso_11.6.1_8255/workspace/uart-timing-demo/xip" -I"/home/asmir/Documents/MCUXpresso_11.6.1_8255/workspace/uart-timing-demo/component/silicon_id" -I"/home/asmir/Documents/MCUXpresso_11.6.1_8255/workspace/uart-timing-demo/CMSIS" -I"/home/asmir/Documents/MCUXpresso_11.6.1_8255/workspace/uart-timing-demo/board" -I"/home/asmir/Documents/MCUXpresso_11.6.1_8255/workspace/uart-timing-demo/evkmimxrt1020/driver_examples/gpio/led_output" -O0 -fno-common -g3 -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m7 -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-board

clean-board:
	-$(RM) ./board/board.d ./board/board.o ./board/clock_config.d ./board/clock_config.o ./board/dcd.d ./board/dcd.o ./board/pin_mux.d ./board/pin_mux.o

.PHONY: clean-board

