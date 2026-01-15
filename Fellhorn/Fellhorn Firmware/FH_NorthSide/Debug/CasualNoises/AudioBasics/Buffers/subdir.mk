################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
/Users/dirktjantele/Desktop/Hardware\ Projects/Hardware\ Synth/CasualNoises\ Framework/CasualNoises/AudioBasics/Buffers/AudioBuffer.cpp 

OBJS += \
./CasualNoises/AudioBasics/Buffers/AudioBuffer.o 

CPP_DEPS += \
./CasualNoises/AudioBasics/Buffers/AudioBuffer.d 


# Each subdirectory must supply rules for building sources it contributes
CasualNoises/AudioBasics/Buffers/AudioBuffer.o: /Users/dirktjantele/Desktop/Hardware\ Projects/Hardware\ Synth/CasualNoises\ Framework/CasualNoises/AudioBasics/Buffers/AudioBuffer.cpp CasualNoises/AudioBasics/Buffers/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m7 -std=gnu++14 -g3 -DDEBUG -DUSE_PWR_LDO_SUPPLY -DUSE_HAL_DRIVER -DSTM32H743xx -DTLV_DRIVER_SUPPORT -DCASUALNOISES_TRIGGER_THREAD -DCASUALNOISES_AUDIO_THREAD -DUSE_AUDIO_PROCESSOR_PLAYER -DUSE_AUDIO_BUFFER -DCASUALNOISES_NERVENET_THREAD -DCASUALNOISES_ADC_THREAD -DCASUALNOISES_ADC_CALLBACKS -DCASUALNOISES_EXTI_CALLBACKS -DCASUALNOISES_TxCplt_CALLBACKS -DCASUALNOISES_TxRxCplt_CALLBACKS -DCASUALNOISES_TIMER_CALLBACKS -DADC_NERVENET_SUPPORT -DFELLHORN_NORTH_SIDE -DUSE_CS4270_Driver -DCASUALNOISES_NVM_DRIVER_SUPPORT -D__PROGRAM_START -c -I../Core/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I"/Users/dirktjantele/Desktop/Hardware Projects/Hardware Synth/CasualNoises Framework/CasualNoises" -I"/Users/dirktjantele/Desktop/Hardware Projects/Hardware Synth/_Current Projects/Fellhorn rev 2/Fellhorn Firmware/FH_NorthSide/NorthSideEngine" -I"/Users/dirktjantele/Desktop/Hardware Projects/Hardware Synth/CasualNoises Framework/CasualNoises" -I"/Users/dirktjantele/Desktop/Hardware Projects/Hardware Synth/_Current Projects/OpFour/Firmware/Common Sources" -I"/Users/dirktjantele/Desktop/Hardware Projects/Hardware Synth/_Current Projects/Fellhorn rev 2/Fellhorn Firmware/FH_CommonSources" -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-CasualNoises-2f-AudioBasics-2f-Buffers

clean-CasualNoises-2f-AudioBasics-2f-Buffers:
	-$(RM) ./CasualNoises/AudioBasics/Buffers/AudioBuffer.cyclo ./CasualNoises/AudioBasics/Buffers/AudioBuffer.d ./CasualNoises/AudioBasics/Buffers/AudioBuffer.o ./CasualNoises/AudioBasics/Buffers/AudioBuffer.su

.PHONY: clean-CasualNoises-2f-AudioBasics-2f-Buffers

