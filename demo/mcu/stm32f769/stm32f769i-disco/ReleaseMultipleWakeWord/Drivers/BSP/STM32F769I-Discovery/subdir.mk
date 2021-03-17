################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Drivers/BSP/STM32F769I-Discovery/stm32f769i_discovery.c \
../Drivers/BSP/STM32F769I-Discovery/stm32f769i_discovery_audio.c \
../Drivers/BSP/STM32F769I-Discovery/stm32f769i_discovery_eeprom.c \
../Drivers/BSP/STM32F769I-Discovery/stm32f769i_discovery_sd.c \
../Drivers/BSP/STM32F769I-Discovery/stm32f769i_discovery_sdram.c 

OBJS += \
./Drivers/BSP/STM32F769I-Discovery/stm32f769i_discovery.o \
./Drivers/BSP/STM32F769I-Discovery/stm32f769i_discovery_audio.o \
./Drivers/BSP/STM32F769I-Discovery/stm32f769i_discovery_eeprom.o \
./Drivers/BSP/STM32F769I-Discovery/stm32f769i_discovery_sd.o \
./Drivers/BSP/STM32F769I-Discovery/stm32f769i_discovery_sdram.o 

C_DEPS += \
./Drivers/BSP/STM32F769I-Discovery/stm32f769i_discovery.d \
./Drivers/BSP/STM32F769I-Discovery/stm32f769i_discovery_audio.d \
./Drivers/BSP/STM32F769I-Discovery/stm32f769i_discovery_eeprom.d \
./Drivers/BSP/STM32F769I-Discovery/stm32f769i_discovery_sd.d \
./Drivers/BSP/STM32F769I-Discovery/stm32f769i_discovery_sdram.d 


# Each subdirectory must supply rules for building sources it contributes
Drivers/BSP/STM32F769I-Discovery/stm32f769i_discovery.o: ../Drivers/BSP/STM32F769I-Discovery/stm32f769i_discovery.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -DSTM32 -DSTM32F769xx -DUSE_STM32F769I_DISCO -DUSE_HAL_DRIVER -DSTM32F769I_DISCO -DSTM32F7 -DSTM32F769NIHx -c -I../../../../../include -I../Inc -I../Middlewares/ST/STM32_Audio/Addons/PDM/Inc -I../Drivers/CMSIS/Include -I../Drivers/CMSIS/Device/ST/STM32F7xx/Include -I../Drivers/STM32F7xx_HAL_Driver/Inc -I../Drivers/BSP/STM32F769I-Discovery -O2 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/BSP/STM32F769I-Discovery/stm32f769i_discovery.d" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/BSP/STM32F769I-Discovery/stm32f769i_discovery_audio.o: ../Drivers/BSP/STM32F769I-Discovery/stm32f769i_discovery_audio.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -DSTM32 -DSTM32F769xx -DUSE_STM32F769I_DISCO -DUSE_HAL_DRIVER -DSTM32F769I_DISCO -DSTM32F7 -DSTM32F769NIHx -c -I../../../../../include -I../Inc -I../Middlewares/ST/STM32_Audio/Addons/PDM/Inc -I../Drivers/CMSIS/Include -I../Drivers/CMSIS/Device/ST/STM32F7xx/Include -I../Drivers/STM32F7xx_HAL_Driver/Inc -I../Drivers/BSP/STM32F769I-Discovery -O2 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/BSP/STM32F769I-Discovery/stm32f769i_discovery_audio.d" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/BSP/STM32F769I-Discovery/stm32f769i_discovery_eeprom.o: ../Drivers/BSP/STM32F769I-Discovery/stm32f769i_discovery_eeprom.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -DSTM32 -DSTM32F769xx -DUSE_STM32F769I_DISCO -DUSE_HAL_DRIVER -DSTM32F769I_DISCO -DSTM32F7 -DSTM32F769NIHx -c -I../../../../../include -I../Inc -I../Middlewares/ST/STM32_Audio/Addons/PDM/Inc -I../Drivers/CMSIS/Include -I../Drivers/CMSIS/Device/ST/STM32F7xx/Include -I../Drivers/STM32F7xx_HAL_Driver/Inc -I../Drivers/BSP/STM32F769I-Discovery -O2 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/BSP/STM32F769I-Discovery/stm32f769i_discovery_eeprom.d" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/BSP/STM32F769I-Discovery/stm32f769i_discovery_sd.o: ../Drivers/BSP/STM32F769I-Discovery/stm32f769i_discovery_sd.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -DSTM32 -DSTM32F769xx -DUSE_STM32F769I_DISCO -DUSE_HAL_DRIVER -DSTM32F769I_DISCO -DSTM32F7 -DSTM32F769NIHx -c -I../../../../../include -I../Inc -I../Middlewares/ST/STM32_Audio/Addons/PDM/Inc -I../Drivers/CMSIS/Include -I../Drivers/CMSIS/Device/ST/STM32F7xx/Include -I../Drivers/STM32F7xx_HAL_Driver/Inc -I../Drivers/BSP/STM32F769I-Discovery -O2 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/BSP/STM32F769I-Discovery/stm32f769i_discovery_sd.d" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
Drivers/BSP/STM32F769I-Discovery/stm32f769i_discovery_sdram.o: ../Drivers/BSP/STM32F769I-Discovery/stm32f769i_discovery_sdram.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -DSTM32 -DSTM32F769xx -DUSE_STM32F769I_DISCO -DUSE_HAL_DRIVER -DSTM32F769I_DISCO -DSTM32F7 -DSTM32F769NIHx -c -I../../../../../include -I../Inc -I../Middlewares/ST/STM32_Audio/Addons/PDM/Inc -I../Drivers/CMSIS/Include -I../Drivers/CMSIS/Device/ST/STM32F7xx/Include -I../Drivers/STM32F7xx_HAL_Driver/Inc -I../Drivers/BSP/STM32F769I-Discovery -O2 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Drivers/BSP/STM32F769I-Discovery/stm32f769i_discovery_sdram.d" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

