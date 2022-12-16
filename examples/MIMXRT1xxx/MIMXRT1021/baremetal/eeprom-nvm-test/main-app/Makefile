include Makefile.variable

SDPHOST=../../flashloader/Tools/sdphost/linux/amd64/sdphost
BLHOST=../../flashloader/Tools/blhost/linux/amd64/blhost

#wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww
# Add your source files here
#wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww
# firmware output file name
PROJ_NAME=m7fw

# linker scipt
LINKER_SCRIPT=./sdk/MIMXRT1021xxxxx_flexspi_nor.ld 
#LINKER_SCRIPT=evkmimxrt1020_flexspi_nor_polling_transfer_Debug.ld

# startup file
STARTUP_FILE=./sdk/startup_MIMXRT1021.s 
#STARTUP_FILE=

# *.c file sources
C_SRCS = main.c systemc.c 
#startup_mimxrt1021.c
C_SRCS += delay.c fwver.c
C_SRCS += debug.c misc.c lpuart.c

C_SRCS += ./drivers/board.c
C_SRCS += ./drivers/evkmimxrt1020_flexspi_nor_config.c  
C_SRCS += ./drivers/fsl_clock.c       
C_SRCS += ./drivers/fsl_component_generic_list.c  
C_SRCS += ./drivers/fsl_gpio.c    
C_SRCS += ./drivers/pin_mux.c
C_SRCS += ./drivers/clock_config.c             
C_SRCS += ./drivers/fsl_common_arm.c
C_SRCS += ./drivers/system_MIMXRT1021.c
C_SRCS += ./drivers/dcd.c           
C_SRCS += ./drivers/fsl_assert.c                        
C_SRCS += ./drivers/fsl_common.c      
C_SRCS += ./drivers/fsl_flexspi_nor_boot.c        
C_SRCS += ./drivers/fsl_sbrk.c



C_SRCS += ./hal/timing.c
#C_SRCS += ./hal/hal_timing.c

C_SRCS += ./hal/hal_uart.c
C_SRCS += ./hal/uart.c
C_SRCS += ./hal/buffer.c
C_SRCS += ./hal/buffer_printf.c
C_SRCS += ./hal/task.c
C_SRCS += ./hal/list.c


C_SRCS += ./drivers/fsl_flexspi.c
C_SRCS += ./drivers/flexspi_nor_flash_ops.c
C_SRCS += ./drivers/fsl_cache.c
C_SRCS += ./hal/hal_eeprom.c
C_SRCS += ./hal/eeprom.c

C_SRCS += ./hal/nvm_objects.c



# *.c and *.h file paths 
INC_PATHS =  -I./
INC_PATHS += -I./sdk/
INC_PATHS += -I./drivers/  
INC_PATHS += -I./hal/  

#wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww
# Normally you shouldn't need to change anything below this line!
#wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww
CURRENT_DIR = $(shell pwd)
BUILD_DIR = $(CURRENT_DIR)/build

CFLAGS  = -g -O2 -Wall  -T$(LINKER_SCRIPT)
CFLAGS += -mcpu=cortex-m7 -mthumb  -mabi=aapcs --std=gnu99 -DCORE_CM7 -DARM_MATH_CM7
CFLAGS += -DCPU_MIMXRT1021DAG5A -DCPU_MIMXRT1021DAG5A_cm7  -DXIP_EXTERNAL_FLASH=1
CFLAGS += -D__USE_CMSIS
CFLAGS += -D__FPU_PRESENT -DDISABLE_WDOG=0 
CFLAGS += -D__ICACHE_PRESENT
CFLAGS += -DXIP_BOOT_HEADER_DCD_ENABLE=1 -DXIP_BOOT_HEADER_ENABLE=1 -DSKIP_SYSCLK_INIT
# -D__MCUXPRESSO 
#  -D__REDLIB__
#-DMCUXPRESSO_SDK  
#
#
#
#
#


#CFLAGS += -DNDEBUG -D__STARTUP_CLEAR_BSS -D__STARTUP_INITIALIZE_NONCACHEDATA 

CFLAGS += -mfloat-abi=hard -mfpu=fpv5-sp-d16
CFLAGS += -ffunction-sections -fdata-sections  
#CFLAGS += -fno-strict-aliasing
#CFLAGS += -fno-builtin --short-enums 
CFLAGS += -mlittle-endian -mthumb -mcpu=cortex-m7 -std=gnu99
CFLAGS += -mfloat-abi=hard -mfpu=fpv5-d16 -ffreestanding -fno-builtin -mapcs
CFLAGS += -fno-common -fdata-sections -ffunction-sections -Wl,--gc-sections

ASMFLAGS = -x assembler-with-cpp

# keep every function in separate section. This will allow linker to dump unused functions
#LDFLAGS = -Xlinker -Map=$(BUILD_DIR)/$(PROJ_NAME).map
LDFLAGS += -mthumb -mabi=aapcs -L./ -T$(LINKER_SCRIPT)
LDFLAGS += -mcpu=cortex-m7
LDFLAGS += -mfloat-abi=hard -mfpu=fpv5-sp-d16
# let linker to dump unused sections
LDFLAGS += -Wl,--gc-sections
# use newlib in nano version
#LDFLAGS += --specs=nano.specs -lm -lc -lnosys -lat_ble_api_sam4s 
#LDFLAGS += -lm -lc -lnosys -lbat_ble_api_sam4s.a
LDFLAGS += -g -Wl,-Map=$(PROJ_NAME).map -Wl,--cref  -Wa,-adhlns="$(@:%.o=%.lst)"


# add startup file to build
A_SRCS = $(STARTUP_FILE)

#wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww
# Makefile functions and constants
#wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww
#function for removing duplicates in a list
remduplicates = $(strip $(if $1,$(firstword $1) $(call remduplicates,$(filter-out $(firstword $1),$1))))


# coloring constants
ccblack=@echo "\033[0;30m"
ccred=@echo "\033[0;31m"
ccgreen=@echo "\033[0;32m"
ccyellow=@echo "\033[0;33m"
ccblue=@echo "\033[0;34m"
ccmagenta=@echo "\033[0;35m"
cccyan=@echo "\033[0;36m"
ccgray=@echo "\033[0;37m"
ccend="\033[0m"

CPU_COUNT = $(shell grep -c ^processor /proc/cpuinfo)


#wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww
# C source files processing
#wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww
# extract all file names from the C sources
C_SOURCE_FILE_NAMES = $(notdir $(C_SRCS))
# get all sub paths from the C sources
C_PATHS = $(call remduplicates, $(dir $(C_SRCS)))
# create object file file names for all C source files
C_OBJECTS = $(addprefix $(BUILD_DIR)/, $(C_SOURCE_FILE_NAMES:.c=.o) )

#wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww
# ASM source files processing
#wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww
# extract all file names from the ASM sources
ASM_SOURCE_FILE_NAMES = $(notdir $(A_SRCS))
# get all sub paths from the ASM sources
ASM_PATHS = $(call remduplicates, $(dir $(A_SRCS)))
# create object file file names for all ASM source files
ASM_OBJECTS = $(addprefix $(BUILD_DIR)/, $(ASM_SOURCE_FILE_NAMES:.s=.o) )

# get all C and ASM source full paths
vpath %.c $(C_PATHS)
vpath %.s $(ASM_PATHS)

OBJECTS = $(C_OBJECTS) $(ASM_OBJECTS)

#wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww
# compilation recipes
#wwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwwww
.PHONY: proj

all: 
	@$(MAKE) -s --no-print-directory preaction 
	@$(MAKE) -s -j$(CPU_COUNT) --no-print-directory $(OBJECTS) 
	@$(MAKE) -s -j$(CPU_COUNT) --no-print-directory compile 
	@$(MAKE) -s --no-print-directory postaction


compile:	
	$(ccyellow)linking target...$(ccend)
	@$(CC) $(LDFLAGS) $(OBJECTS) $(LIBS) -lm -lc -lnosys -o  $(BUILD_DIR)/$(PROJ_NAME).elf
#	@echo creating executables...
	@$(OBJCOPY) -O ihex $(BUILD_DIR)/$(PROJ_NAME).elf $(BUILD_DIR)/$(PROJ_NAME).hex
	@$(OBJCOPY) -O binary $(BUILD_DIR)/$(PROJ_NAME).elf $(BUILD_DIR)/$(PROJ_NAME).bin


preaction:
	@python3 $(BMODULES)/scripts/fwinfo.py

# override default make recipe for C files
$(BUILD_DIR)/%.o: %.c
	$(ccgray)compiling c file:$(ccend)   $(notdir $@)
	@$(CC) $(CFLAGS) $(INC_PATHS) -c -lm -lc -lnosys -o $@ $<

# override default make recipe for ASM files
$(BUILD_DIR)/%.o: %.s
	@echo compiling asm file: $(notdir $@)
	@$(CC) $(ASMFLAGS) $(INC_PATHS) -c -o $@ $<

postaction: 
#	@python2 fwgen.py
	@$(SIZE) -B  $(BUILD_DIR)/$(PROJ_NAME).elf

dump:
	@$(OBJDUMP) -D $(BUILD_DIR)/$(PROJ_NAME).elf > $(BUILD_DIR)/$(PROJ_NAME).lst
	  
clean:
	$(ccyellow)cleaning files...$(ccend)
	@rm -f $(BUILD_DIR)/*.o $(BUILD_DIR)/*.bin $(BUILD_DIR)/*.elf $(BUILD_DIR)/*.hex $(BUILD_DIR)/*.map $(BUILD_DIR)/*.lst
	@echo done

#
restore:
	$(SDPHOST) -u 0x1fc9,0x0130 -V -- write-file 0x60000000 ./jlink/rescue.bin
	$(SDPHOST) -u 0x1fc9,0x0130 -V -- jump-address 0x20208400
	@sleep 2
	$(BLHOST) -u 0x15a2,0x0073  -- fill-memory 0x2000 4 0xC0000006
	$(BLHOST) -u 0x15a2,0x0073 -- configure-memory 9 0x2000
	
load:
	$(SDPHOST) -u 0x1fc9,0x0130 -V -- write-file 0x20208000 ./sdk/ivt_flashloader.bin
	@sleep 1
	$(SDPHOST) -u 0x1fc9,0x0130 -V -- jump-address 0x20208400
	@sleep 2
	$(BLHOST) -u 0x15a2,0x0073  -- fill-memory 0x2000 4 0xC0000006
	$(BLHOST) -u 0x15a2,0x0073 -- configure-memory 9 0x2000
#	$(BLHOST)  -u 0x15a2,0x0073 -V -- write-file 0x20208400 ./sdk/iled_blinky.bin
#	$(BLHOST) -u 0x15a2,0x0073 -- flash-image ./sdk/iled_blinky.bin erase 9
	

upload:
	JLinkExe -autoconnect 1 -device MIMXRT1021XXX5A -if SWD -speed 4000 -CommandFile jlink/upload.jlink

			
reset:
	JLinkExe -autoconnect 1 -device MIMXRT1021XXX5A -if SWD -speed 4000 -CommandFile jlink/reset.jlink

	
erase:
	@$(STM32CUBEPROG) -e all
	@sleep 1
	@$(STM32CUBEPROG) -hardRst
 
help:
	$(STM32CUBEPROG) --help	

