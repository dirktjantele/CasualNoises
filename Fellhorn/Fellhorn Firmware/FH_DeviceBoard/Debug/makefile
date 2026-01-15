################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Middlewares/Third_Party/FreeRTOS/Source/portable/MemMang/subdir.mk
-include Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F/subdir.mk
-include Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2/subdir.mk
-include Middlewares/Third_Party/FreeRTOS/Source/subdir.mk
-include Drivers/STM32H7xx_HAL_Driver/Src/subdir.mk
-include DeviceBoardEngine/UI_Handlers/subdir.mk
-include DeviceBoardEngine/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/subdir.mk
-include CasualNoises/xml/subdir.mk
-include CasualNoises/Utilities/subdir.mk
-include CasualNoises/Threads/subdir.mk
-include CasualNoises/Synthesizer/Basics/subdir.mk
-include CasualNoises/NerveNet/subdir.mk
-include CasualNoises/Graphics/Fonts/subdir.mk
-include CasualNoises/Graphics/Contexts/subdir.mk
-include CasualNoises/GUI/GUI_Utilities/subdir.mk
-include CasualNoises/GUI/GUI_Basics/Components/subdir.mk
-include CasualNoises/Drivers/TLV\ Driver/subdir.mk
-include CasualNoises/Drivers/NVM\ Drivers/W25Q64\ Driver/subdir.mk
-include CasualNoises/Drivers/IO_Drivers/subdir.mk
-include CasualNoises/Drivers/ILI9341/subdir.mk
-include CasualNoises/DSP/Filter/subdir.mk
-include CasualNoises/AudioUtils/Players/subdir.mk
-include CasualNoises/AudioBasics/Processors/subdir.mk
-include CasualNoises/AudioBasics/Buffers/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(CC_DEPS)),)
-include $(CC_DEPS)
endif
ifneq ($(strip $(C++_DEPS)),)
-include $(C++_DEPS)
endif
ifneq ($(strip $(CCM_DEPS)),)
-include $(CCM_DEPS)
endif
ifneq ($(strip $(C_UPPER_DEPS)),)
-include $(C_UPPER_DEPS)
endif
ifneq ($(strip $(CXX_DEPS)),)
-include $(CXX_DEPS)
endif
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(CXXM_DEPS)),)
-include $(CXXM_DEPS)
endif
ifneq ($(strip $(C++M_DEPS)),)
-include $(C++M_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
ifneq ($(strip $(CPP_DEPS)),)
-include $(CPP_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := FH_DeviceBoard
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
FH_DeviceBoard.elf \

MAP_FILES += \
FH_DeviceBoard.map \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
FH_DeviceBoard.list \


# All Target
all: main-build

# Main-build Target
main-build: FH_DeviceBoard.elf secondary-outputs

# Tool invocations
FH_DeviceBoard.elf FH_DeviceBoard.map: $(OBJS) $(USER_OBJS) /Users/dirktjantele/Desktop/Hardware\ Projects/Hardware\ Synth/_Current\ Projects/Fellhorn\ rev\ 2/Fellhorn\ Firmware/FH_DeviceBoard/STM32H723ZGTX_FLASH.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-g++ -o "FH_DeviceBoard.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m7 -T"/Users/dirktjantele/Desktop/Hardware Projects/Hardware Synth/_Current Projects/Fellhorn rev 2/Fellhorn Firmware/FH_DeviceBoard/STM32H723ZGTX_FLASH.ld" --specs=nosys.specs -Wl,-Map="FH_DeviceBoard.map" -Wl,--gc-sections -static --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -u _printf_float -u _scanf_float -Wl,--start-group -lc -lm -lstdc++ -lsupc++ -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

FH_DeviceBoard.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "FH_DeviceBoard.list"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) FH_DeviceBoard.elf FH_DeviceBoard.list FH_DeviceBoard.map default.size.stdout
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents main-build fail-specified-linker-script-missing warn-no-linker-script-specified

-include ../makefile.targets
