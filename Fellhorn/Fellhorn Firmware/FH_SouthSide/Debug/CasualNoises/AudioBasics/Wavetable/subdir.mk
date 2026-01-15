################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
/Users/dirktjantele/Desktop/Hardware\ Projects/Hardware\ Synth/CasualNoises\ Framework/CasualNoises/AudioBasics/Wavetable/WaveTable.cpp 

OBJS += \
./CasualNoises/AudioBasics/Wavetable/WaveTable.o 

CPP_DEPS += \
./CasualNoises/AudioBasics/Wavetable/WaveTable.d 


# Each subdirectory must supply rules for building sources it contributes
CasualNoises/AudioBasics/Wavetable/WaveTable.o: /Users/dirktjantele/Desktop/Hardware\ Projects/Hardware\ Synth/CasualNoises\ Framework/CasualNoises/AudioBasics/Wavetable/WaveTable.cpp CasualNoises/AudioBasics/Wavetable/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m7 -std=gnu++14 -g3 -DDEBUG -DUSE_PWR_LDO_SUPPLY -DUSE_HAL_DRIVER -DSTM32H723xx -DSOUTH_SIDE -DNVM_DRIVER_SUPPORT -DTLV_DRIVER_SUPPORT -DUSE_AUDIO_BUFFER -DCASUALNOISES_NERVENET_THREAD -DCASUALNOISES_ADC_THREAD -DCASUALNOISES_ADC_CALLBACKS -DCASUALNOISES_TIMER_CALLBACKS -DCASUALNOISES_EXTI_CALLBACKS -c -I../Core/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/CMSIS/Include -I"/Users/dirktjantele/Desktop/Hardware Projects/Hardware Synth/CasualNoises Framework/CasualNoises" -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I"/Users/dirktjantele/Desktop/Hardware Projects/Hardware Synth/_Current Projects/OpFour/Firmware/SouthSide/SouthSideEngine" -I"/Users/dirktjantele/Desktop/Hardware Projects/Hardware Synth/CasualNoises Framework/CasualNoises" -I"/Users/dirktjantele/Desktop/Hardware Projects/Hardware Synth/_Current Projects/OpFour/Firmware/Common Sources" -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-CasualNoises-2f-AudioBasics-2f-Wavetable

clean-CasualNoises-2f-AudioBasics-2f-Wavetable:
	-$(RM) ./CasualNoises/AudioBasics/Wavetable/WaveTable.cyclo ./CasualNoises/AudioBasics/Wavetable/WaveTable.d ./CasualNoises/AudioBasics/Wavetable/WaveTable.o ./CasualNoises/AudioBasics/Wavetable/WaveTable.su

.PHONY: clean-CasualNoises-2f-AudioBasics-2f-Wavetable

