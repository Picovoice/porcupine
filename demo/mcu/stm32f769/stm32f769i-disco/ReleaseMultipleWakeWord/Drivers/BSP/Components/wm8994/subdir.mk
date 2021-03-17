################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/BSP/Components/wm8994/wm8994.c 

OBJS += \
./Drivers/BSP/Components/wm8994/wm8994.o 

C_DEPS += \
./Drivers/BSP/Components/wm8994/wm8994.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/BSP/Components/wm8994/wm8994.o: ../Drivers/BSP/Components/wm8994/wm8994.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -DSTM32 -DSTM32F769xx -DUSE_STM32F769I_DISCO -DUSE_HAL_DRIVER -DSTM32F769I_DISCO -DSTM32F7 -DSTM32F769NIHx -c -I../../../../../include -I../Inc -I../Middlewares/ST/STM32_Audio/Addons/PDM/Inc -I../Drivers/CMSIS/Include -I../Drivers/CMSIS/Device/ST/STM32F7xx/Include -I../Drivers/STM32F7xx_HAL_Driver/Inc -I../Drivers/BSP/STM32F769I-Discovery -O2 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/BSP/Components/wm8994/wm8994.d" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

