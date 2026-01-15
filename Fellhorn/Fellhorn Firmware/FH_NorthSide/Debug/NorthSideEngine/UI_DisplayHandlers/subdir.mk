################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../NorthSideEngine/UI_DisplayHandlers/MainDisplayHandler.cpp 

OBJS += \
./NorthSideEngine/UI_DisplayHandlers/MainDisplayHandler.o 

CPP_DEPS += \
./NorthSideEngine/UI_DisplayHandlers/MainDisplayHandler.d 


# Each subdirectory must supply rules for building sources it contributes
NorthSideEngine/UI_DisplayHandlers/%.o NorthSideEngine/UI_DisplayHandlers/%.su NorthSideEngine/UI_DisplayHandlers/%.cyclo: ../NorthSideEngine/UI_DisplayHandlers/%.cpp NorthSideEngine/UI_DisplayHandlers/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m7 -std=gnu++14 -g3 -DDEBUG -DUSE_PWR_LDO_SUPPLY -DUSE_HAL_DRIVER -DSTM32H743xx -DNORTH_SIDE -DNVM_DRIVER_SUPPORT -DTLV_DRIVER_SUPPORT -DCASUALNOISES_TRIGGER_THREAD -DCASUALNOISES_POTENTIOMETER_THREAD -DCASUALNOISES_ENCODER_THREAD_SPI -DCASUALNOISES_AUDIO_THREAD -DUSE_AUDIO_PROCESSOR_PLAYER -DUSE_AUDIO_BUFFER -DUSE_SSD1309_DRIVER -DUSE_CS4270_Driver -DCASUALNOISES_LED_THREAD_SPI -DCASUALNOISES_NERVENET_THREAD -DCASUALNOISES_ADC_THREAD -DCASUALNOISES_ADC_CALLBACKS -DCASUALNOISES_EXTI_CALLBACKS -DCASUALNOISES_TxCplt_CALLBACKS -DCASUALNOISES_TxRxCplt_CALLBACKS -DCASUALNOISES_TIMER_CALLBACKS -c -I../Core/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I"/Users/dirktjantele/Desktop/Hardware Projects/Hardware Synth/CasualNoises Framework/CasualNoises" -I"/Users/dirktjantele/Desktop/Hardware Projects/Hardware Synth/_Current Projects/OpFour/Firmware/NorthSide/NorthSideEngine" -I"/Users/dirktjantele/Desktop/Hardware Projects/Hardware Synth/CasualNoises Framework/CasualNoises" -I"/Users/dirktjantele/Desktop/Hardware Projects/Hardware Synth/_Current Projects/OpFour/Firmware/Common Sources" -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-NorthSideEngine-2f-UI_DisplayHandlers

clean-NorthSideEngine-2f-UI_DisplayHandlers:
	-$(RM) ./NorthSideEngine/UI_DisplayHandlers/MainDisplayHandler.cyclo ./NorthSideEngine/UI_DisplayHandlers/MainDisplayHandler.d ./NorthSideEngine/UI_DisplayHandlers/MainDisplayHandler.o ./NorthSideEngine/UI_DisplayHandlers/MainDisplayHandler.su

.PHONY: clean-NorthSideEngine-2f-UI_DisplayHandlers

