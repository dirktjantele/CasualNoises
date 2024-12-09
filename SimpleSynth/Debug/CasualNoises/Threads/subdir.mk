################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../CasualNoises/Threads/AudioThread.cpp \
../CasualNoises/Threads/EncoderThread.cpp \
../CasualNoises/Threads/PotentiometerThread.cpp \
../CasualNoises/Threads/TriggerThread.cpp 

OBJS += \
./CasualNoises/Threads/AudioThread.o \
./CasualNoises/Threads/EncoderThread.o \
./CasualNoises/Threads/PotentiometerThread.o \
./CasualNoises/Threads/TriggerThread.o 

CPP_DEPS += \
./CasualNoises/Threads/AudioThread.d \
./CasualNoises/Threads/EncoderThread.d \
./CasualNoises/Threads/PotentiometerThread.d \
./CasualNoises/Threads/TriggerThread.d 


# Each subdirectory must supply rules for building sources it contributes
CasualNoises/Threads/%.o CasualNoises/Threads/%.su CasualNoises/Threads/%.cyclo: ../CasualNoises/Threads/%.cpp CasualNoises/Threads/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m7 -std=gnu++14 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32H723xx -DSIMPLE_SYNTH_0_1 -DNVM_DRIVER_SUPPORT -DTLV_DRIVER_SUPPORT -c -I../Core/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc -I../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../Drivers/CMSIS/Include -I../Middlewares/Third_Party/FreeRTOS/Source/include -I../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS_V2 -I../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I"/Users/dirktjantele/Desktop/Hardware Projects/Hardware Synth/_Current Projects/BreakOut_0.1/Firmware/CasualNoises" -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-CasualNoises-2f-Threads

clean-CasualNoises-2f-Threads:
	-$(RM) ./CasualNoises/Threads/AudioThread.cyclo ./CasualNoises/Threads/AudioThread.d ./CasualNoises/Threads/AudioThread.o ./CasualNoises/Threads/AudioThread.su ./CasualNoises/Threads/EncoderThread.cyclo ./CasualNoises/Threads/EncoderThread.d ./CasualNoises/Threads/EncoderThread.o ./CasualNoises/Threads/EncoderThread.su ./CasualNoises/Threads/PotentiometerThread.cyclo ./CasualNoises/Threads/PotentiometerThread.d ./CasualNoises/Threads/PotentiometerThread.o ./CasualNoises/Threads/PotentiometerThread.su ./CasualNoises/Threads/TriggerThread.cyclo ./CasualNoises/Threads/TriggerThread.d ./CasualNoises/Threads/TriggerThread.o ./CasualNoises/Threads/TriggerThread.su

.PHONY: clean-CasualNoises-2f-Threads

