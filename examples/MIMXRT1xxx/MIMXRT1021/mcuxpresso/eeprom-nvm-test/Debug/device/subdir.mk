################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../device/system_MIMXRT1021.c 

C_DEPS += \
./device/system_MIMXRT1021.d 

OBJS += \
./device/system_MIMXRT1021.o 


# Each subdirectory must supply rules for building sources it contributes
device/%.o: ../device/%.c device/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -std=gnu99 -DCPU_MIMXRT1021DAG5A -DCPU_MIMXRT1021DAG5A_cm7 -DXIP_EXTERNAL_FLASH=1 -DXIP_BOOT_HEADER_ENABLE=1 -D__USE_CMSIS -DSKIP_SYSCLK_INIT -D__ICACHE_PRESENT -D__FPU_PRESENT -DDISABLE_WDOG=0 -I"/home/asmir/Desktop/Dropbox/MCU/nxp/RT1021/projects/alencon-systems/embedded-software/examples/MIMXRT1xxx/MIMXRT1021/mcuxpresso/eeprom-nvm-test/source" -I"/home/asmir/Desktop/Dropbox/MCU/nxp/RT1021/projects/alencon-systems/embedded-software/examples/MIMXRT1xxx/MIMXRT1021/mcuxpresso/eeprom-nvm-test/utilities" -I"/home/asmir/Desktop/Dropbox/MCU/nxp/RT1021/projects/alencon-systems/embedded-software/examples/MIMXRT1xxx/MIMXRT1021/mcuxpresso/eeprom-nvm-test/drivers" -I"/home/asmir/Desktop/Dropbox/MCU/nxp/RT1021/projects/alencon-systems/embedded-software/examples/MIMXRT1xxx/MIMXRT1021/mcuxpresso/eeprom-nvm-test/device" -I"/home/asmir/Desktop/Dropbox/MCU/nxp/RT1021/projects/alencon-systems/embedded-software/examples/MIMXRT1xxx/MIMXRT1021/mcuxpresso/eeprom-nvm-test/component/uart" -I"/home/asmir/Desktop/Dropbox/MCU/nxp/RT1021/projects/alencon-systems/embedded-software/examples/MIMXRT1xxx/MIMXRT1021/mcuxpresso/eeprom-nvm-test/component/lists" -I"/home/asmir/Desktop/Dropbox/MCU/nxp/RT1021/projects/alencon-systems/embedded-software/examples/MIMXRT1xxx/MIMXRT1021/mcuxpresso/eeprom-nvm-test/xip" -I"/home/asmir/Desktop/Dropbox/MCU/nxp/RT1021/projects/alencon-systems/embedded-software/examples/MIMXRT1xxx/MIMXRT1021/mcuxpresso/eeprom-nvm-test/component/silicon_id" -I"/home/asmir/Desktop/Dropbox/MCU/nxp/RT1021/projects/alencon-systems/embedded-software/examples/MIMXRT1xxx/MIMXRT1021/mcuxpresso/eeprom-nvm-test/CMSIS" -I"/home/asmir/Desktop/Dropbox/MCU/nxp/RT1021/projects/alencon-systems/embedded-software/examples/MIMXRT1xxx/MIMXRT1021/mcuxpresso/eeprom-nvm-test/board" -I"/home/asmir/Desktop/Dropbox/MCU/nxp/RT1021/projects/alencon-systems/embedded-software/examples/MIMXRT1xxx/MIMXRT1021/mcuxpresso/eeprom-nvm-test/evkmimxrt1020/driver_examples/gpio/led_output" -O2 -fno-common -g3 -Wall -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m7 -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -fstack-usage -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-device

clean-device:
	-$(RM) ./device/system_MIMXRT1021.d ./device/system_MIMXRT1021.o

.PHONY: clean-device

