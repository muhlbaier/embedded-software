################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../drivers/flexspi_nor_flash_ops.c \
../drivers/fsl_cache.c \
../drivers/fsl_clock.c \
../drivers/fsl_common.c \
../drivers/fsl_common_arm.c \
../drivers/fsl_flexspi.c \
../drivers/fsl_gpio.c \
../drivers/fsl_lpuart.c 

C_DEPS += \
./drivers/flexspi_nor_flash_ops.d \
./drivers/fsl_cache.d \
./drivers/fsl_clock.d \
./drivers/fsl_common.d \
./drivers/fsl_common_arm.d \
./drivers/fsl_flexspi.d \
./drivers/fsl_gpio.d \
./drivers/fsl_lpuart.d 

OBJS += \
./drivers/flexspi_nor_flash_ops.o \
./drivers/fsl_cache.o \
./drivers/fsl_clock.o \
./drivers/fsl_common.o \
./drivers/fsl_common_arm.o \
./drivers/fsl_flexspi.o \
./drivers/fsl_gpio.o \
./drivers/fsl_lpuart.o 


# Each subdirectory must supply rules for building sources it contributes
drivers/%.o: ../drivers/%.c drivers/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -std=gnu99 -DCPU_MIMXRT1021DAG5A -DDCDC_BOARD -DCPU_MIMXRT1021DAG5A_cm7 -DXIP_EXTERNAL_FLASH=1 -DXIP_BOOT_HEADER_ENABLE=1 -D__USE_CMSIS -DSKIP_SYSCLK_INIT -D__ICACHE_PRESENT -D__FPU_PRESENT -DDISABLE_WDOG=0 -I"/home/asmir/Desktop/Dropbox/MCU/nxp/RT1021/projects/alencon-systems/embedded-software/examples/MIMXRT1xxx/MIMXRT1021/mcuxpresso/rs485-firmware-upgrade/bootloader/source" -I"/home/asmir/Desktop/Dropbox/MCU/nxp/RT1021/projects/alencon-systems/embedded-software/examples/MIMXRT1xxx/MIMXRT1021/mcuxpresso/rs485-firmware-upgrade/bootloader/utilities" -I"/home/asmir/Desktop/Dropbox/MCU/nxp/RT1021/projects/alencon-systems/embedded-software/examples/MIMXRT1xxx/MIMXRT1021/mcuxpresso/rs485-firmware-upgrade/bootloader/drivers" -I"/home/asmir/Desktop/Dropbox/MCU/nxp/RT1021/projects/alencon-systems/embedded-software/examples/MIMXRT1xxx/MIMXRT1021/mcuxpresso/rs485-firmware-upgrade/bootloader/device" -I"/home/asmir/Desktop/Dropbox/MCU/nxp/RT1021/projects/alencon-systems/embedded-software/examples/MIMXRT1xxx/MIMXRT1021/mcuxpresso/rs485-firmware-upgrade/bootloader/component/uart" -I"/home/asmir/Desktop/Dropbox/MCU/nxp/RT1021/projects/alencon-systems/embedded-software/examples/MIMXRT1xxx/MIMXRT1021/mcuxpresso/rs485-firmware-upgrade/bootloader/component/lists" -I"/home/asmir/Desktop/Dropbox/MCU/nxp/RT1021/projects/alencon-systems/embedded-software/examples/MIMXRT1xxx/MIMXRT1021/mcuxpresso/rs485-firmware-upgrade/bootloader/xip" -I"/home/asmir/Desktop/Dropbox/MCU/nxp/RT1021/projects/alencon-systems/embedded-software/examples/MIMXRT1xxx/MIMXRT1021/mcuxpresso/rs485-firmware-upgrade/bootloader/component/silicon_id" -I"/home/asmir/Desktop/Dropbox/MCU/nxp/RT1021/projects/alencon-systems/embedded-software/examples/MIMXRT1xxx/MIMXRT1021/mcuxpresso/rs485-firmware-upgrade/bootloader/CMSIS" -I"/home/asmir/Desktop/Dropbox/MCU/nxp/RT1021/projects/alencon-systems/embedded-software/examples/MIMXRT1xxx/MIMXRT1021/mcuxpresso/rs485-firmware-upgrade/bootloader/board" -I"/home/asmir/Desktop/Dropbox/MCU/nxp/RT1021/projects/alencon-systems/embedded-software/examples/MIMXRT1xxx/MIMXRT1021/mcuxpresso/rs485-firmware-upgrade/bootloader/evkmimxrt1020/driver_examples/gpio/led_output" -I"/home/asmir/Desktop/Dropbox/MCU/nxp/RT1021/projects/alencon-systems/embedded-software/examples/MIMXRT1xxx/MIMXRT1021/mcuxpresso/rs485-firmware-upgrade/bootloader/source/bootloader" -I"/home/asmir/Desktop/Dropbox/MCU/nxp/RT1021/projects/alencon-systems/embedded-software/examples/MIMXRT1xxx/MIMXRT1021/mcuxpresso/rs485-firmware-upgrade/bootloader/source/DCDCBoard/include" -I"/home/asmir/Desktop/Dropbox/MCU/nxp/RT1021/projects/alencon-systems/embedded-software/examples/MIMXRT1xxx/MIMXRT1021/mcuxpresso/rs485-firmware-upgrade/bootloader/source/DCDCComms" -I"/home/asmir/Desktop/Dropbox/MCU/nxp/RT1021/projects/alencon-systems/embedded-software/examples/MIMXRT1xxx/MIMXRT1021/mcuxpresso/rs485-firmware-upgrade/bootloader/source/DCDCModel" -I"/home/asmir/Desktop/Dropbox/MCU/nxp/RT1021/projects/alencon-systems/embedded-software/examples/MIMXRT1xxx/MIMXRT1021/mcuxpresso/rs485-firmware-upgrade/bootloader/source/DCDCConfig" -I"/home/asmir/Desktop/Dropbox/MCU/nxp/RT1021/projects/alencon-systems/embedded-software/examples/MIMXRT1xxx/MIMXRT1021/mcuxpresso/rs485-firmware-upgrade/bootloader/source/Modbus" -I"/home/asmir/Desktop/Dropbox/MCU/nxp/RT1021/projects/alencon-systems/embedded-software/examples/MIMXRT1xxx/MIMXRT1021/mcuxpresso/rs485-firmware-upgrade/bootloader/source/alencon_armcm_core" -I"/home/asmir/Desktop/Dropbox/MCU/nxp/RT1021/projects/alencon-systems/embedded-software/examples/MIMXRT1xxx/MIMXRT1021/mcuxpresso/rs485-firmware-upgrade/bootloader/source/alencon_armcm_core/commonlib/freemodbus/modbus/include" -I"/home/asmir/Desktop/Dropbox/MCU/nxp/RT1021/projects/alencon-systems/embedded-software/examples/MIMXRT1xxx/MIMXRT1021/mcuxpresso/rs485-firmware-upgrade/bootloader/source/alencon_armcm_core/commonlib/freemodbus/modbus/rtu" -I"/home/asmir/Desktop/Dropbox/MCU/nxp/RT1021/projects/alencon-systems/embedded-software/examples/MIMXRT1xxx/MIMXRT1021/mcuxpresso/rs485-firmware-upgrade/bootloader/source/alencon_armcm_core/commonlib/RS485/include/RS485Client" -I"/home/asmir/Desktop/Dropbox/MCU/nxp/RT1021/projects/alencon-systems/embedded-software/examples/MIMXRT1xxx/MIMXRT1021/mcuxpresso/rs485-firmware-upgrade/bootloader/source/hal" -O2 -fno-common -g3 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m7 -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-drivers

clean-drivers:
	-$(RM) ./drivers/flexspi_nor_flash_ops.d ./drivers/flexspi_nor_flash_ops.o ./drivers/fsl_cache.d ./drivers/fsl_cache.o ./drivers/fsl_clock.d ./drivers/fsl_clock.o ./drivers/fsl_common.d ./drivers/fsl_common.o ./drivers/fsl_common_arm.d ./drivers/fsl_common_arm.o ./drivers/fsl_flexspi.d ./drivers/fsl_flexspi.o ./drivers/fsl_gpio.d ./drivers/fsl_gpio.o ./drivers/fsl_lpuart.d ./drivers/fsl_lpuart.o

.PHONY: clean-drivers

