################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../SouthSideEngine/SouthSideAudioProcessor.cpp 

OBJS += \
./SouthSideEngine/SouthSideAudioProcessor.o 

CPP_DEPS += \
./SouthSideEngine/SouthSideAudioProcessor.d 


# Each subdirectory must supply rules for building sources it contributes
SouthSideEngine/%.o SouthSideEngine/%.su SouthSideEngine/%.cyclo: ../SouthSideEngine/%.cpp SouthSideEngine/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m7 -std=gnu++14 -g3 -DDEBUG -DUSE_PWR_LDO_SUPPLY -DUSE_HAL_DRIVER -DSTM32H723xx -DSOUTH_SIDE -DNVM_DRIVER_SUPPORT -DTLV_DRIVER_SUPPORT -DUSE_AUDIO_BUFFER -DCASUALNOISES_NERVENET_THREAD -DCASUALNOISES_ADC_THREAD -DCASUALNOISES_ADC_CALLBACKS -DCASUALNOISES_TIMER_CALLBACKS -DCASUALNOISES_EXTI_CALLBACKS -DCASUALNOISES_TRIGGER_THREAD -DFELLHORN_SOUTH_SIDE -D__PROGRAM_START -DCASUALNOISES_NVM_DRIVER_SUPPORT -DCASUALNOISES_NERVENET_SLAVE_AUDIO_SUPPORT -c -I../Core/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/CMSIS/Include -I"/Users/dirktjantele/Desktop/Hardware Projects/Hardware Synth/CasualNoises Framework/CasualNoises" -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I"/Users/dirktjantele/Desktop/Hardware Projects/Hardware Synth/_Current Projects/Fellhorn rev 2/Fellhorn Firmware/FH_SouthSide/SouthSideEngine" -I"/Users/dirktjantele/Desktop/Hardware Projects/Hardware Synth/CasualNoises Framework/CasualNoises" -I"/Users/dirktjantele/Desktop/Hardware Projects/Hardware Synth/_Current Projects/Fellhorn rev 2/Fellhorn Firmware/FH_CommonSources" -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-SouthSideEngine

clean-SouthSideEngine:
	-$(RM) ./SouthSideEngine/SouthSideAudioProcessor.cyclo ./SouthSideEngine/SouthSideAudioProcessor.d ./SouthSideEngine/SouthSideAudioProcessor.o ./SouthSideEngine/SouthSideAudioProcessor.su

.PHONY: clean-SouthSideEngine

