################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/BSP/Components/cs43l22/cs43l22.c 

OBJS += \
./Drivers/BSP/Components/cs43l22/cs43l22.o 

C_DEPS += \
./Drivers/BSP/Components/cs43l22/cs43l22.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/BSP/Components/cs43l22/cs43l22.o: ../Drivers/BSP/Components/cs43l22/cs43l22.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu99 -DSTM32 -DSTM32F469NIHx -DSTM32F469xx -DSTM32F4 -DSTM32F469I_DISCO -c -I../../../../../include -I../Inc -I../Middlewares/ST/STM32_Audio/Addons/PDM/Inc -I../Drivers/CMSIS/Include -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/BSP/STM32469I-Discovery -O2 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/BSP/Components/cs43l22/cs43l22.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

