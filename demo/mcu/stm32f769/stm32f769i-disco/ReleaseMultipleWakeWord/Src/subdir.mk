################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/main_multi.c \
../Src/pv_audio_rec.c \
../Src/pv_st_f769.c \
../Src/stm32f7xx_it.c \
../Src/syscalls.c \
../Src/system_stm32f7xx.c 

OBJS += \
./Src/main_multi.o \
./Src/pv_audio_rec.o \
./Src/pv_st_f769.o \
./Src/stm32f7xx_it.o \
./Src/syscalls.o \
./Src/system_stm32f7xx.o 

C_DEPS += \
./Src/main_multi.d \
./Src/pv_audio_rec.d \
./Src/pv_st_f769.d \
./Src/stm32f7xx_it.d \
./Src/syscalls.d \
./Src/system_stm32f7xx.d 


# Each subdirectory must supply rules for building sources it contributes
Src/main_multi.o: ../Src/main_multi.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -DSTM32 -DSTM32F769xx -DUSE_STM32F769I_DISCO -DUSE_HAL_DRIVER -DSTM32F769I_DISCO -DSTM32F7 -DSTM32F769NIHx -c -I../../../../../include -I../Inc -I../Middlewares/ST/STM32_Audio/Addons/PDM/Inc -I../Drivers/CMSIS/Include -I../Drivers/CMSIS/Device/ST/STM32F7xx/Include -I../Drivers/STM32F7xx_HAL_Driver/Inc -I../Drivers/BSP/STM32F769I-Discovery -O2 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Src/main_multi.d" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
Src/pv_audio_rec.o: ../Src/pv_audio_rec.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -DSTM32 -DSTM32F769xx -DUSE_STM32F769I_DISCO -DUSE_HAL_DRIVER -DSTM32F769I_DISCO -DSTM32F7 -DSTM32F769NIHx -c -I../../../../../include -I../Inc -I../Middlewares/ST/STM32_Audio/Addons/PDM/Inc -I../Drivers/CMSIS/Include -I../Drivers/CMSIS/Device/ST/STM32F7xx/Include -I../Drivers/STM32F7xx_HAL_Driver/Inc -I../Drivers/BSP/STM32F769I-Discovery -O2 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Src/pv_audio_rec.d" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
Src/pv_st_f769.o: ../Src/pv_st_f769.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -DSTM32 -DSTM32F769xx -DUSE_STM32F769I_DISCO -DUSE_HAL_DRIVER -DSTM32F769I_DISCO -DSTM32F7 -DSTM32F769NIHx -c -I../../../../../include -I../Inc -I../Middlewares/ST/STM32_Audio/Addons/PDM/Inc -I../Drivers/CMSIS/Include -I../Drivers/CMSIS/Device/ST/STM32F7xx/Include -I../Drivers/STM32F7xx_HAL_Driver/Inc -I../Drivers/BSP/STM32F769I-Discovery -O2 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Src/pv_st_f769.d" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
Src/stm32f7xx_it.o: ../Src/stm32f7xx_it.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -DSTM32 -DSTM32F769xx -DUSE_STM32F769I_DISCO -DUSE_HAL_DRIVER -DSTM32F769I_DISCO -DSTM32F7 -DSTM32F769NIHx -c -I../../../../../include -I../Inc -I../Middlewares/ST/STM32_Audio/Addons/PDM/Inc -I../Drivers/CMSIS/Include -I../Drivers/CMSIS/Device/ST/STM32F7xx/Include -I../Drivers/STM32F7xx_HAL_Driver/Inc -I../Drivers/BSP/STM32F769I-Discovery -O2 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Src/stm32f7xx_it.d" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
Src/syscalls.o: ../Src/syscalls.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -DSTM32 -DSTM32F769xx -DUSE_STM32F769I_DISCO -DUSE_HAL_DRIVER -DSTM32F769I_DISCO -DSTM32F7 -DSTM32F769NIHx -c -I../../../../../include -I../Inc -I../Middlewares/ST/STM32_Audio/Addons/PDM/Inc -I../Drivers/CMSIS/Include -I../Drivers/CMSIS/Device/ST/STM32F7xx/Include -I../Drivers/STM32F7xx_HAL_Driver/Inc -I../Drivers/BSP/STM32F769I-Discovery -O2 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Src/syscalls.d" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"
Src/system_stm32f7xx.o: ../Src/system_stm32f7xx.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -DSTM32 -DSTM32F769xx -DUSE_STM32F769I_DISCO -DUSE_HAL_DRIVER -DSTM32F769I_DISCO -DSTM32F7 -DSTM32F769NIHx -c -I../../../../../include -I../Inc -I../Middlewares/ST/STM32_Audio/Addons/PDM/Inc -I../Drivers/CMSIS/Include -I../Drivers/CMSIS/Device/ST/STM32F7xx/Include -I../Drivers/STM32F7xx_HAL_Driver/Inc -I../Drivers/BSP/STM32F769I-Discovery -O2 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Src/system_stm32f7xx.d" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

