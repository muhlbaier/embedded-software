################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../component/lists/fsl_component_generic_list.c 

C_DEPS += \
./component/lists/fsl_component_generic_list.d 

OBJS += \
./component/lists/fsl_component_generic_list.o 


# Each subdirectory must supply rules for building sources it contributes
component/lists/%.o: ../component/lists/%.c component/lists/subdir.mk
	@echo 'Building file: $<'
	@echo 'Invoking: MCU C Compiler'
	arm-none-eabi-gcc -std=gnu99 -D__REDLIB__ -DCPU_MIMXRT1021DAG5A -DCPU_MIMXRT1021DAG5A_cm7 -DSDK_DEBUGCONSOLE=1 -DXIP_EXTERNAL_FLASH=1 -DXIP_BOOT_HEADER_ENABLE=1 -DMCUXPRESSO_SDK -DCR_INTEGER_PRINTF -DPRINTF_FLOAT_ENABLE=0 -D__MCUXPRESSO -D__USE_CMSIS -DDEBUG -I"/home/asmir/Documents/MCUXpresso_11.6.1_8255/workspace/uart-timing-demo/source" -I"/home/asmir/Documents/MCUXpresso_11.6.1_8255/workspace/uart-timing-demo/utilities" -I"/home/asmir/Documents/MCUXpresso_11.6.1_8255/workspace/uart-timing-demo/drivers" -I"/home/asmir/Documents/MCUXpresso_11.6.1_8255/workspace/uart-timing-demo/device" -I"/home/asmir/Documents/MCUXpresso_11.6.1_8255/workspace/uart-timing-demo/component/uart" -I"/home/asmir/Documents/MCUXpresso_11.6.1_8255/workspace/uart-timing-demo/component/lists" -I"/home/asmir/Documents/MCUXpresso_11.6.1_8255/workspace/uart-timing-demo/xip" -I"/home/asmir/Documents/MCUXpresso_11.6.1_8255/workspace/uart-timing-demo/component/silicon_id" -I"/home/asmir/Documents/MCUXpresso_11.6.1_8255/workspace/uart-timing-demo/CMSIS" -I"/home/asmir/Documents/MCUXpresso_11.6.1_8255/workspace/uart-timing-demo/board" -I"/home/asmir/Documents/MCUXpresso_11.6.1_8255/workspace/uart-timing-demo/evkmimxrt1020/driver_examples/gpio/led_output" -O0 -fno-common -g3 -c -ffunction-sections -fdata-sections -ffreestanding -fno-builtin -fmerge-constants -fmacro-prefix-map="$(<D)/"= -mcpu=cortex-m7 -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -D__REDLIB__ -fstack-usage -specs=redlib.specs -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.o)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


clean: clean-component-2f-lists

clean-component-2f-lists:
	-$(RM) ./component/lists/fsl_component_generic_list.d ./component/lists/fsl_component_generic_list.o

.PHONY: clean-component-2f-lists

