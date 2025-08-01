################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Drivers/STM32F2xx_HAL_Driver/Src/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := Test_Uart_Thong3Cong
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
Test_Uart_Thong3Cong.elf \

MAP_FILES += \
Test_Uart_Thong3Cong.map \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
Test_Uart_Thong3Cong.list \


# All Target
all: main-build

# Main-build Target
main-build: Test_Uart_Thong3Cong.elf secondary-outputs

# Tool invocations
Test_Uart_Thong3Cong.elf Test_Uart_Thong3Cong.map: $(OBJS) $(USER_OBJS) E:\alta_media_maymoi\duan_2024\DCS\ USB\ Iot\ Box\DCS\ USB\ Iot\ Box\CODE\Serial\ Printer\ type\From\ Dat\Test_Uart_Thong3Cong\Test_Uart_Thong3Cong\STM32F205VGTX_FLASH.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-gcc -o "Test_Uart_Thong3Cong.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m3 -T"E:\alta_media_maymoi\duan_2024\DCS USB Iot Box\DCS USB Iot Box\CODE\Serial Printer type\From Dat\Test_Uart_Thong3Cong\Test_Uart_Thong3Cong\STM32F205VGTX_FLASH.ld" --specs=nosys.specs -Wl,-Map="Test_Uart_Thong3Cong.map" -Wl,--gc-sections -static --specs=nano.specs -mfloat-abi=soft -mthumb -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

Test_Uart_Thong3Cong.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "Test_Uart_Thong3Cong.list"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) Test_Uart_Thong3Cong.elf Test_Uart_Thong3Cong.list Test_Uart_Thong3Cong.map default.size.stdout
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents main-build fail-specified-linker-script-missing warn-no-linker-script-specified

-include ../makefile.targets
