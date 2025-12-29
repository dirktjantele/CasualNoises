################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
/Users/dirktjantele/Desktop/Hardware\ Projects/Hardware\ Synth/CasualNoises\ Framework/CasualNoises/GUI/GUI_Basics/Components/Box.cpp \
/Users/dirktjantele/Desktop/Hardware\ Projects/Hardware\ Synth/CasualNoises\ Framework/CasualNoises/GUI/GUI_Basics/Components/ComboBox.cpp \
/Users/dirktjantele/Desktop/Hardware\ Projects/Hardware\ Synth/CasualNoises\ Framework/CasualNoises/GUI/GUI_Basics/Components/Component.cpp 

OBJS += \
./CasualNoises/GUI/GUI_Basics/Components/Box.o \
./CasualNoises/GUI/GUI_Basics/Components/ComboBox.o \
./CasualNoises/GUI/GUI_Basics/Components/Component.o 

CPP_DEPS += \
./CasualNoises/GUI/GUI_Basics/Components/Box.d \
./CasualNoises/GUI/GUI_Basics/Components/ComboBox.d \
./CasualNoises/GUI/GUI_Basics/Components/Component.d 


# Each subdirectory must supply rules for building sources it contributes
CasualNoises/GUI/GUI_Basics/Components/Box.o: /Users/dirktjantele/Desktop/Hardware\ Projects/Hardware\ Synth/CasualNoises\ Framework/CasualNoises/GUI/GUI_Basics/Components/Box.cpp CasualNoises/GUI/GUI_Basics/Components/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m7 -std=gnu++14 -g3 -DDEBUG -DUSE_PWR_LDO_SUPPLY -DUSE_HAL_DRIVER -DSTM32H723xx -DFELLHORN_DEVICE_BOARD -D__PROGRAM_START -DCASUALNOISES_TRIGGER_THREAD -DCASUALNOISES_ENCODER_THREAD_SPI -DCASUALNOISES_SSD1309_DRIVER -DCASUALNOISES_NVM_DRIVER_SUPPORT -DCASUALNOISES_LED_THREAD_SPI -DCASUALNOISES_TxCplt_CALLBACKS -DCASUALNOISES_TxRxCplt_CALLBACKS -DCASUALNOISES_MULT_ADC_THREAD -DCASUALNOISES_ADC_CALLBACKS -c -I../Core/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I"/Users/dirktjantele/Desktop/Hardware Projects/Hardware Synth/_Current Projects/Fellhorn rev 2/Fellhorn Firmware/FH_DeviceBoard/FH_CommonSources" -I"/Users/dirktjantele/Desktop/Hardware Projects/Hardware Synth/CasualNoises Framework/CasualNoises" -I"/Users/dirktjantele/Desktop/Hardware Projects/Hardware Synth/_Current Projects/Fellhorn rev 2/Fellhorn Firmware/FH_DeviceBoard/DeviceBoardEngine" -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
CasualNoises/GUI/GUI_Basics/Components/ComboBox.o: /Users/dirktjantele/Desktop/Hardware\ Projects/Hardware\ Synth/CasualNoises\ Framework/CasualNoises/GUI/GUI_Basics/Components/ComboBox.cpp CasualNoises/GUI/GUI_Basics/Components/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m7 -std=gnu++14 -g3 -DDEBUG -DUSE_PWR_LDO_SUPPLY -DUSE_HAL_DRIVER -DSTM32H723xx -DFELLHORN_DEVICE_BOARD -D__PROGRAM_START -DCASUALNOISES_TRIGGER_THREAD -DCASUALNOISES_ENCODER_THREAD_SPI -DCASUALNOISES_SSD1309_DRIVER -DCASUALNOISES_NVM_DRIVER_SUPPORT -DCASUALNOISES_LED_THREAD_SPI -DCASUALNOISES_TxCplt_CALLBACKS -DCASUALNOISES_TxRxCplt_CALLBACKS -DCASUALNOISES_MULT_ADC_THREAD -DCASUALNOISES_ADC_CALLBACKS -c -I../Core/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I"/Users/dirktjantele/Desktop/Hardware Projects/Hardware Synth/_Current Projects/Fellhorn rev 2/Fellhorn Firmware/FH_DeviceBoard/FH_CommonSources" -I"/Users/dirktjantele/Desktop/Hardware Projects/Hardware Synth/CasualNoises Framework/CasualNoises" -I"/Users/dirktjantele/Desktop/Hardware Projects/Hardware Synth/_Current Projects/Fellhorn rev 2/Fellhorn Firmware/FH_DeviceBoard/DeviceBoardEngine" -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
CasualNoises/GUI/GUI_Basics/Components/Component.o: /Users/dirktjantele/Desktop/Hardware\ Projects/Hardware\ Synth/CasualNoises\ Framework/CasualNoises/GUI/GUI_Basics/Components/Component.cpp CasualNoises/GUI/GUI_Basics/Components/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m7 -std=gnu++14 -g3 -DDEBUG -DUSE_PWR_LDO_SUPPLY -DUSE_HAL_DRIVER -DSTM32H723xx -DFELLHORN_DEVICE_BOARD -D__PROGRAM_START -DCASUALNOISES_TRIGGER_THREAD -DCASUALNOISES_ENCODER_THREAD_SPI -DCASUALNOISES_SSD1309_DRIVER -DCASUALNOISES_NVM_DRIVER_SUPPORT -DCASUALNOISES_LED_THREAD_SPI -DCASUALNOISES_TxCplt_CALLBACKS -DCASUALNOISES_TxRxCplt_CALLBACKS -DCASUALNOISES_MULT_ADC_THREAD -DCASUALNOISES_ADC_CALLBACKS -c -I../Core/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I"/Users/dirktjantele/Desktop/Hardware Projects/Hardware Synth/_Current Projects/Fellhorn rev 2/Fellhorn Firmware/FH_DeviceBoard/FH_CommonSources" -I"/Users/dirktjantele/Desktop/Hardware Projects/Hardware Synth/CasualNoises Framework/CasualNoises" -I"/Users/dirktjantele/Desktop/Hardware Projects/Hardware Synth/_Current Projects/Fellhorn rev 2/Fellhorn Firmware/FH_DeviceBoard/DeviceBoardEngine" -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-CasualNoises-2f-GUI-2f-GUI_Basics-2f-Components

clean-CasualNoises-2f-GUI-2f-GUI_Basics-2f-Components:
	-$(RM) ./CasualNoises/GUI/GUI_Basics/Components/Box.cyclo ./CasualNoises/GUI/GUI_Basics/Components/Box.d ./CasualNoises/GUI/GUI_Basics/Components/Box.o ./CasualNoises/GUI/GUI_Basics/Components/Box.su ./CasualNoises/GUI/GUI_Basics/Components/ComboBox.cyclo ./CasualNoises/GUI/GUI_Basics/Components/ComboBox.d ./CasualNoises/GUI/GUI_Basics/Components/ComboBox.o ./CasualNoises/GUI/GUI_Basics/Components/ComboBox.su ./CasualNoises/GUI/GUI_Basics/Components/Component.cyclo ./CasualNoises/GUI/GUI_Basics/Components/Component.d ./CasualNoises/GUI/GUI_Basics/Components/Component.o ./CasualNoises/GUI/GUI_Basics/Components/Component.su

.PHONY: clean-CasualNoises-2f-GUI-2f-GUI_Basics-2f-Components

