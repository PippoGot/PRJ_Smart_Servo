################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Core/Legacy/AS5600.cpp \
../Core/Legacy/_i2c_device.cpp \
../Core/Legacy/prev_main.cpp 

OBJS += \
./Core/Legacy/AS5600.o \
./Core/Legacy/_i2c_device.o \
./Core/Legacy/prev_main.o 

CPP_DEPS += \
./Core/Legacy/AS5600.d \
./Core/Legacy/_i2c_device.d \
./Core/Legacy/prev_main.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Legacy/%.o Core/Legacy/%.su Core/Legacy/%.cyclo: ../Core/Legacy/%.cpp Core/Legacy/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m3 -std=gnu++14 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-Core-2f-Legacy

clean-Core-2f-Legacy:
	-$(RM) ./Core/Legacy/AS5600.cyclo ./Core/Legacy/AS5600.d ./Core/Legacy/AS5600.o ./Core/Legacy/AS5600.su ./Core/Legacy/_i2c_device.cyclo ./Core/Legacy/_i2c_device.d ./Core/Legacy/_i2c_device.o ./Core/Legacy/_i2c_device.su ./Core/Legacy/prev_main.cyclo ./Core/Legacy/prev_main.d ./Core/Legacy/prev_main.o ./Core/Legacy/prev_main.su

.PHONY: clean-Core-2f-Legacy

