################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/BSP/STM32469I-Discovery/stm32469i_discovery.c \
../Drivers/BSP/STM32469I-Discovery/stm32469i_discovery_audio.c 

OBJS += \
./Drivers/BSP/STM32469I-Discovery/stm32469i_discovery.o \
./Drivers/BSP/STM32469I-Discovery/stm32469i_discovery_audio.o 

C_DEPS += \
./Drivers/BSP/STM32469I-Discovery/stm32469i_discovery.d \
./Drivers/BSP/STM32469I-Discovery/stm32469i_discovery_audio.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/BSP/STM32469I-Discovery/stm32469i_discovery.o: ../Drivers/BSP/STM32469I-Discovery/stm32469i_discovery.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu99 -DSTM32 -DSTM32F469NIHx -DSTM32F469xx -DSTM32F4 -DSTM32F469I_DISCO -c -I../../../../../include -I../Inc -I../Middlewares/ST/STM32_Audio/Addons/PDM/Inc -I../Drivers/CMSIS/Include -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/BSP/STM32469I-Discovery -O2 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/BSP/STM32469I-Discovery/stm32469i_discovery.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/BSP/STM32469I-Discovery/stm32469i_discovery_audio.o: ../Drivers/BSP/STM32469I-Discovery/stm32469i_discovery_audio.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu99 -DSTM32 -DSTM32F469NIHx -DSTM32F469xx -DSTM32F4 -DSTM32F469I_DISCO -c -I../../../../../include -I../Inc -I../Middlewares/ST/STM32_Audio/Addons/PDM/Inc -I../Drivers/CMSIS/Include -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/BSP/STM32469I-Discovery -O2 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/BSP/STM32469I-Discovery/stm32469i_discovery_audio.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

