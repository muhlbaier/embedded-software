################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include xip/subdir.mk
-include utilities/subdir.mk
-include source/hal/subdir.mk
-include source/bootloader/subdir.mk
-include source/alencon_armcm_core/commonlib/freemodbus/modbus/tcp/subdir.mk
-include source/alencon_armcm_core/commonlib/freemodbus/modbus/rtu/subdir.mk
-include source/alencon_armcm_core/commonlib/freemodbus/modbus/functions/subdir.mk
-include source/alencon_armcm_core/commonlib/freemodbus/modbus/ascii/subdir.mk
-include source/alencon_armcm_core/commonlib/freemodbus/modbus/subdir.mk
-include source/Modbus/subdir.mk
-include source/DCDCComms/subdir.mk
-include source/DCDCBoard/src/subdir.mk
-include source/subdir.mk
-include drivers/subdir.mk
-include device/subdir.mk
-include component/uart/subdir.mk
-include component/silicon_id/subdir.mk
-include component/lists/subdir.mk
-include board/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := bootloader
BUILD_ARTIFACT_EXTENSION := axf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all:
	+@$(MAKE) --no-print-directory main-build && $(MAKE) --no-print-directory post-build

# Main-build Target
main-build: bootloader.axf

# Tool invocations
bootloader.axf: $(OBJS) $(USER_OBJS) makefile objects.mk $(OPTIONAL_TOOL_DEPS)
	@echo 'Building target: $@'
	@echo 'Invoking: MCU Linker'
	arm-none-eabi-gcc -Xlinker -Map="bootloader.map" -Xlinker --gc-sections -Xlinker -print-memory-usage -Xlinker --sort-section=alignment -Xlinker --cref -mcpu=cortex-m7 -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -T MIMXRT1021xxxxx_flexspi_nor.ld -L "/home/asmir/Desktop/Dropbox/MCU/nxp/RT1021/projects/alencon-systems/embedded-software/examples/MIMXRT1xxx/MIMXRT1021/mcuxpresso/rs485-firmware-upgrade/bootloader/source" -o "bootloader.axf" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) bootloader.axf
	-@echo ' '

post-build:
	-@echo 'Performing post-build steps'
	-arm-none-eabi-size "bootloader.axf" ; arm-none-eabi-objcopy -O ihex "bootloader.axf" "bootloader.hex" ; #checksum -p MIMXRT1021xxxxx -d "bootloader.hex"
	-@echo ' '

.PHONY: all clean dependents main-build post-build

-include ../makefile.targets
