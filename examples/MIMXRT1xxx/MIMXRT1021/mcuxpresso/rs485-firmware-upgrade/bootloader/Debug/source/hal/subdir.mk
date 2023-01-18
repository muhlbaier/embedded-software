################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../source/hal/buffer.c \
../source/hal/buffer_printf.c \
../source/hal/charReceiverList.c \
../source/hal/crc16.c \
../source/hal/eeprom.c \
../source/hal/flash.c \
../source/hal/hal_eeprom.c \
../source/hal/hal_timing.c \
../source/hal/hal_uart.c \
../source/hal/list.c \
../source/hal/nvm_objects.c \
../source/hal/subsys.c \
../source/hal/task.c \
../source/hal/timing.c \
../source/hal/uart.c 

C_DEPS += \
./source/hal/buffer.d \
./source/hal/buffer_printf.d \
./source/hal/charReceiverList.d \
./source/hal/crc16.d \
./source/hal/eeprom.d \
./source/hal/flash.d \
./source/hal/hal_eeprom.d \
./source/hal/hal_timing.d \
./source/hal/hal_uart.d \
./source/hal/list.d \
./source/hal/nvm_objects.d \
./source/hal/subsys.d \
./source/hal/task.d \
./source/hal/timing.d \
./source/hal/uart.d 

OBJS += \
./source/hal/buffer.o \
./source/hal/buffer_printf.o \
./source/hal/charReceiverList.o \
./source/hal/crc16.o \
./source/hal/eeprom.o \
./source/hal/flash.o \
./source/hal/hal_eeprom.o \
./source/hal/hal_timing.o \
./source/hal/hal_uart.o \
./source/hal/list.o \
./source/hal/nvm_objects.o \
./source/hal/subsys.o \
./source/hal/task.o \
./source/hal/timing.o \
./source/hal/uart.o 


# Each subdirectory must supply rules for building sources it contributes
source/hal/%.o: ../source/hal/%.c source/hal/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -std=gnu99 -DCPU_MIMXRT1021DAG5A -DDCDC_BOARD -DCPU_MIMXRT1021DAG5A_cm7 -DXIP_EXTERNAL_FLASH=1 -DXIP_BOOT_HEADER_ENABLE=1 -D__USE_CMSIS -DSKIP_SYSCLK_INIT -D__ICACHE_PRESENT -D__FPU_PRESENT -DDISABLE_WDOG=0 -I"/home/asmir/Desktop/Dropbox/MCU/nxp/RT1021/projects/alencon-systems/embedded-software/examples/MIMXRT1xxx/MIMXRT1021/mcuxpresso/rs485-firmware-upgrade/bootloader/source" -I"/home/asmir/Desktop/Dropbox/MCU/nxp/RT1021/projects/alencon-systems/embedded-software/examples/MIMXRT1xxx/MIMXRT1021/mcuxpresso/rs485-firmware-upgrade/bootloader/utilities" -I"/home/asmir/Desktop/Dropbox/MCU/nxp/RT1021/projects/alencon-systems/embedded-software/examples/MIMXRT1xxx/MIMXRT1021/mcuxpresso/rs485-firmware-upgrade/bootloader/drivers" -I"/home/asmir/Desktop/Dropbox/MCU/nxp/RT1021/projects/alencon-systems/embedded-software/examples/MIMXRT1xxx/MIMXRT1021/mcuxpresso/rs485-firmware-upgrade/bootloader/device" -I"/home/asmir/Desktop/Dropbox/MCU/nxp/RT1021/projects/alencon-systems/embedded-software/examples/MIMXRT1xxx/MIMXRT1021/mcuxpresso/rs485-firmware-upgrade/bootloader/component/uart" -I"/home/asmir/Desktop/Dropbox/MCU/nxp/RT1021/projects/alencon-systems/embedded-software/examples/MIMXRT1xxx/MIMXRT1021/mcuxpresso/rs485-firmware-upgrade/bootloader/component/lists" -I"/home/asmir/Desktop/Dropbox/MCU/nxp/RT1021/projects/alencon-systems/embedded-software/examples/MIMXRT1xxx/MIMXRT1021/mcuxpresso/rs485-firmware-upgrade/bootloader/xip" -I"/home/asmir/Desktop/Dropbox/MCU/nxp/RT1021/projects/alencon-systems/embedded-software/examples/MIMXRT1xxx/MIMXRT1021/mcuxpresso/rs485-firmware-upgrade/bootloader/component/silicon_id" -I"/home/asmir/Desktop/Dropbox/MCU/nxp/RT1021/projects/alencon-systems/embedded-software/examples/MIMXRT1xxx/MIMXRT1021/mcuxpresso/rs485-firmware-upgrade/bootloader/CMSIS" -I"/home/asmir/Desktop/Dropbox/MCU/nxp/RT1021/projects/alencon-systems/embedded-software/examples/MIMXRT1xxx/MIMXRT1021/mcuxpresso/rs485-firmware-upgrade/bootloader/board" -I"/home/asmir/Desktop/Dropbox/MCU/nxp/RT1021/projects/alencon-systems/embedded-software/examples/MIMXRT1xxx/MIMXRT1021/mcuxpresso/rs485-firmware-upgrade/bootloader/evkmimxrt1020/driver_examples/gpio/led_output" -I"/home/asmir/Desktop/Dropbox/MCU/nxp/RT1021/projects/alencon-systems/embedded-software/examples/MIMXRT1xxx/MIMXRT1021/mcuxpresso/rs485-firmware-upgrade/bootloader/source/bootloader" -I"/home/asmir/Desktop/Dropbox/MCU/nxp/RT1021/projects/alencon-systems/embedded-software/examples/MIMXRT1xxx/MIMXRT1021/mcuxpresso/rs485-firmware-upgrade/bootloader/source/DCDCBoard/include" -I"/home/asmir/Desktop/Dropbox/MCU/nxp/RT1021/projects/alencon-systems/embedded-software/examples/MIMXRT1xxx/MIMXRT1021/mcuxpresso/rs485-firmware-upgrade/bootloader/source/DCDCComms" -I"/home/asmir/Desktop/Dropbox/MCU/nxp/RT1021/projects/alencon-systems/embedded-software/examples/MIMXRT1xxx/MIMXRT1021/mcuxpresso/rs485-firmware-upgrade/bootloader/source/DCDCModel" -I"/home/asmir/Desktop/Dropbox/MCU/nxp/RT1021/projects/alencon-systems/embedded-software/examples/MIMXRT1xxx/MIMXRT1021/mcuxpresso/rs485-firmware-upgrade/bootloader/source/DCDCConfig" -I"/home/asmir/Desktop/Dropbox/MCU/nxp/RT1021/projects/alencon-systems/embedded-software/examples/MIMXRT1xxx/MIMXRT1021/mcuxpresso/rs485-firmware-upgrade/bootloader/source/Modbus" -I"/home/asmir/Desktop/Dropbox/MCU/nxp/RT1021/projects/alencon-systems/embedded-software/examples/MIMXRT1xxx/MIMXRT1021/mcuxpresso/rs485-firmware-upgrade/bootloader/source/alencon_armcm_core" -I"/home/asmir/Desktop/Dropbox/MCU/nxp/RT1021/projects/alencon-systems/embedded-software/examples/MIMXRT1xxx/MIMXRT1021/mcuxpresso/rs485-firmware-upgrade/bootloader/source/alencon_armcm_core/commonlib/freemodbus/modbus/include" -I"/home/asmir/Desktop/Dropbox/MCU/nxp/RT1021/projects/alencon-systems/embedded-software/examples/MIMXRT1xxx/MIMXRT1021/mcuxpresso/rs485-firmware-upgrade/bootloader/source/alencon_armcm_core/commonlib/freemodbus/modbus/rtu" -I"/home/asmir/Desktop/Dropbox/MCU/nxp/RT1021/projects/alencon-systems/embedded-software/examples/MIMXRT1xxx/MIMXRT1021/mcuxpresso/rs485-firmware-upgrade/bootloader/source/alencon_armcm_core/commonlib/RS485/include/RS485Client" -I"/home/asmir/Desktop/Dropbox/MCU/nxp/RT1021/projects/alencon-systems/embedded-software/examples/MIMXRT1xxx/MIMXRT1021/mcuxpresso/rs485-firmware-upgrade/bootloader/source/hal" -O2 -fno-common -g3 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m7 -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-source-2f-hal

clean-source-2f-hal:
	-$(RM) ./source/hal/buffer.d ./source/hal/buffer.o ./source/hal/buffer_printf.d ./source/hal/buffer_printf.o ./source/hal/charReceiverList.d ./source/hal/charReceiverList.o ./source/hal/crc16.d ./source/hal/crc16.o ./source/hal/eeprom.d ./source/hal/eeprom.o ./source/hal/flash.d ./source/hal/flash.o ./source/hal/hal_eeprom.d ./source/hal/hal_eeprom.o ./source/hal/hal_timing.d ./source/hal/hal_timing.o ./source/hal/hal_uart.d ./source/hal/hal_uart.o ./source/hal/list.d ./source/hal/list.o ./source/hal/nvm_objects.d ./source/hal/nvm_objects.o ./source/hal/subsys.d ./source/hal/subsys.o ./source/hal/task.d ./source/hal/task.o ./source/hal/timing.d ./source/hal/timing.o ./source/hal/uart.d ./source/hal/uart.o

.PHONY: clean-source-2f-hal

