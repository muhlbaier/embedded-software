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
	arm-none-eabi-gcc -std=gnu99 -DCPU_MIMXRT1021DAG5A -DCPU_MIMXRT1021DAG5A_cm7 -DXIP_EXTERNAL_FLASH=1 -DXIP_BOOT_HEADER_ENABLE=1 -D__USE_CMSIS -DSKIP_SYSCLK_INIT -D__ICACHE_PRESENT -D__FPU_PRESENT -DDISABLE_WDOG=0 -I"/home/asmir/Desktop/Dropbox/MCU/nxp/RT1021/projects/alencon-systems/embedded-software/examples/MIMXRT1xxx/MIMXRT1021/mcuxpresso/rs485-firmware-upgrade/main-app/source" -I"/home/asmir/Desktop/Dropbox/MCU/nxp/RT1021/projects/alencon-systems/embedded-software/examples/MIMXRT1xxx/MIMXRT1021/mcuxpresso/rs485-firmware-upgrade/main-app/utilities" -I"/home/asmir/Desktop/Dropbox/MCU/nxp/RT1021/projects/alencon-systems/embedded-software/examples/MIMXRT1xxx/MIMXRT1021/mcuxpresso/rs485-firmware-upgrade/main-app/drivers" -I"/home/asmir/Desktop/Dropbox/MCU/nxp/RT1021/projects/alencon-systems/embedded-software/examples/MIMXRT1xxx/MIMXRT1021/mcuxpresso/rs485-firmware-upgrade/main-app/device" -I"/home/asmir/Desktop/Dropbox/MCU/nxp/RT1021/projects/alencon-systems/embedded-software/examples/MIMXRT1xxx/MIMXRT1021/mcuxpresso/rs485-firmware-upgrade/main-app/component/uart" -I"/home/asmir/Desktop/Dropbox/MCU/nxp/RT1021/projects/alencon-systems/embedded-software/examples/MIMXRT1xxx/MIMXRT1021/mcuxpresso/rs485-firmware-upgrade/main-app/component/lists" -I"/home/asmir/Desktop/Dropbox/MCU/nxp/RT1021/projects/alencon-systems/embedded-software/examples/MIMXRT1xxx/MIMXRT1021/mcuxpresso/rs485-firmware-upgrade/main-app/xip" -I"/home/asmir/Desktop/Dropbox/MCU/nxp/RT1021/projects/alencon-systems/embedded-software/examples/MIMXRT1xxx/MIMXRT1021/mcuxpresso/rs485-firmware-upgrade/main-app/component/silicon_id" -I"/home/asmir/Desktop/Dropbox/MCU/nxp/RT1021/projects/alencon-systems/embedded-software/examples/MIMXRT1xxx/MIMXRT1021/mcuxpresso/rs485-firmware-upgrade/main-app/CMSIS" -I"/home/asmir/Desktop/Dropbox/MCU/nxp/RT1021/projects/alencon-systems/embedded-software/examples/MIMXRT1xxx/MIMXRT1021/mcuxpresso/rs485-firmware-upgrade/main-app/board" -I"/home/asmir/Desktop/Dropbox/MCU/nxp/RT1021/projects/alencon-systems/embedded-software/examples/MIMXRT1xxx/MIMXRT1021/mcuxpresso/rs485-firmware-upgrade/main-app/evkmimxrt1020/driver_examples/gpio/led_output" -O2 -fno-common -g3 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m7 -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-board

clean-board:
	-$(RM) ./board/board.d ./board/board.o ./board/clock_config.d ./board/clock_config.o ./board/dcd.d ./board/dcd.o ./board/pin_mux.d ./board/pin_mux.o

.PHONY: clean-board

