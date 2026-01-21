################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../DeviceBoardEngine/UI_Handlers/LoadPage.cpp \
../DeviceBoardEngine/UI_Handlers/MainPage.cpp \
../DeviceBoardEngine/UI_Handlers/PageManager.cpp \
../DeviceBoardEngine/UI_Handlers/RootPage.cpp \
../DeviceBoardEngine/UI_Handlers/SetupPage.cpp 

OBJS += \
./DeviceBoardEngine/UI_Handlers/LoadPage.o \
./DeviceBoardEngine/UI_Handlers/MainPage.o \
./DeviceBoardEngine/UI_Handlers/PageManager.o \
./DeviceBoardEngine/UI_Handlers/RootPage.o \
./DeviceBoardEngine/UI_Handlers/SetupPage.o 

CPP_DEPS += \
./DeviceBoardEngine/UI_Handlers/LoadPage.d \
./DeviceBoardEngine/UI_Handlers/MainPage.d \
./DeviceBoardEngine/UI_Handlers/PageManager.d \
./DeviceBoardEngine/UI_Handlers/RootPage.d \
./DeviceBoardEngine/UI_Handlers/SetupPage.d 


# Each subdirectory must supply rules for building sources it contributes
DeviceBoardEngine/UI_Handlers/%.o DeviceBoardEngine/UI_Handlers/%.su DeviceBoardEngine/UI_Handlers/%.cyclo: ../DeviceBoardEngine/UI_Handlers/%.cpp DeviceBoardEngine/UI_Handlers/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m7 -std=gnu++14 -g3 -DDEBUG -DUSE_PWR_LDO_SUPPLY -DUSE_HAL_DRIVER -DSTM32H723xx -DFELLHORN_DEVICE_BOARD -D__PROGRAM_START -DCASUALNOISES_TRIGGER_THREAD -DCASUALNOISES_ENCODER_THREAD_SPI -DCASUALNOISES_SSD1309_DRIVER -DCASUALNOISES_NVM_DRIVER_SUPPORT -DCASUALNOISES_LED_THREAD_SPI -DCASUALNOISES_TxCplt_CALLBACKS -DCASUALNOISES_TxRxCplt_CALLBACKS -DCASUALNOISES_MULT_ADC_THREAD -DCASUALNOISES_ADC_CALLBACKS -DCASUALNOISES_DISPLAY_DRIVER -DCASUALNOISES_NERVENET_THREAD -DCASUALNOISES_TLV_DRIVER_THREAD_SUPPORT -DCASUALNOISES_EXTI_CALLBACKS -c -I../Core/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I"/Users/dirktjantele/Desktop/Hardware Projects/Hardware Synth/_Current Projects/Fellhorn rev 2/Fellhorn Firmware/FH_CommonSources" -I"/Users/dirktjantele/Desktop/Hardware Projects/Hardware Synth/CasualNoises Framework/CasualNoises" -I"/Users/dirktjantele/Desktop/Hardware Projects/Hardware Synth/_Current Projects/Fellhorn rev 2/Fellhorn Firmware/FH_DeviceBoard/DeviceBoardEngine" -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-DeviceBoardEngine-2f-UI_Handlers

clean-DeviceBoardEngine-2f-UI_Handlers:
	-$(RM) ./DeviceBoardEngine/UI_Handlers/LoadPage.cyclo ./DeviceBoardEngine/UI_Handlers/LoadPage.d ./DeviceBoardEngine/UI_Handlers/LoadPage.o ./DeviceBoardEngine/UI_Handlers/LoadPage.su ./DeviceBoardEngine/UI_Handlers/MainPage.cyclo ./DeviceBoardEngine/UI_Handlers/MainPage.d ./DeviceBoardEngine/UI_Handlers/MainPage.o ./DeviceBoardEngine/UI_Handlers/MainPage.su ./DeviceBoardEngine/UI_Handlers/PageManager.cyclo ./DeviceBoardEngine/UI_Handlers/PageManager.d ./DeviceBoardEngine/UI_Handlers/PageManager.o ./DeviceBoardEngine/UI_Handlers/PageManager.su ./DeviceBoardEngine/UI_Handlers/RootPage.cyclo ./DeviceBoardEngine/UI_Handlers/RootPage.d ./DeviceBoardEngine/UI_Handlers/RootPage.o ./DeviceBoardEngine/UI_Handlers/RootPage.su ./DeviceBoardEngine/UI_Handlers/SetupPage.cyclo ./DeviceBoardEngine/UI_Handlers/SetupPage.d ./DeviceBoardEngine/UI_Handlers/SetupPage.o ./DeviceBoardEngine/UI_Handlers/SetupPage.su

.PHONY: clean-DeviceBoardEngine-2f-UI_Handlers

