################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../CasualNoises/GUI/GUI_Basics/Components/BoxComponent.cpp \
../CasualNoises/GUI/GUI_Basics/Components/Component.cpp 

OBJS += \
./CasualNoises/GUI/GUI_Basics/Components/BoxComponent.o \
./CasualNoises/GUI/GUI_Basics/Components/Component.o 

CPP_DEPS += \
./CasualNoises/GUI/GUI_Basics/Components/BoxComponent.d \
./CasualNoises/GUI/GUI_Basics/Components/Component.d 


# Each subdirectory must supply rules for building sources it contributes
CasualNoises/GUI/GUI_Basics/Components/%.o CasualNoises/GUI/GUI_Basics/Components/%.su CasualNoises/GUI/GUI_Basics/Components/%.cyclo: ../CasualNoises/GUI/GUI_Basics/Components/%.cpp CasualNoises/GUI/GUI_Basics/Components/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m7 -std=gnu++14 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32H723xx -DSIMPLE_SYNTH_0_1 -DNVM_DRIVER_SUPPORT -DTLV_DRIVER_SUPPORT -c -I../Core/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I"/Users/dirktjantele/Desktop/Hardware Projects/Hardware Synth/_Current Projects/BreakOut_0.1/Firmware/CasualNoises" -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-CasualNoises-2f-GUI-2f-GUI_Basics-2f-Components

clean-CasualNoises-2f-GUI-2f-GUI_Basics-2f-Components:
	-$(RM) ./CasualNoises/GUI/GUI_Basics/Components/BoxComponent.cyclo ./CasualNoises/GUI/GUI_Basics/Components/BoxComponent.d ./CasualNoises/GUI/GUI_Basics/Components/BoxComponent.o ./CasualNoises/GUI/GUI_Basics/Components/BoxComponent.su ./CasualNoises/GUI/GUI_Basics/Components/Component.cyclo ./CasualNoises/GUI/GUI_Basics/Components/Component.d ./CasualNoises/GUI/GUI_Basics/Components/Component.o ./CasualNoises/GUI/GUI_Basics/Components/Component.su

.PHONY: clean-CasualNoises-2f-GUI-2f-GUI_Basics-2f-Components

