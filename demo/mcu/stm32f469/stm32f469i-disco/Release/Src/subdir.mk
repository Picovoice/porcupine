################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Src/main.c \
../Src/pv_audio_rec.c \
../Src/pv_stm32f469.c \
../Src/stm32f4xx_it.c \
../Src/syscalls.c \
../Src/sysmem.c \
../Src/system_stm32f4xx.c 

OBJS += \
./Src/main.o \
./Src/pv_audio_rec.o \
./Src/pv_stm32f469.o \
./Src/stm32f4xx_it.o \
./Src/syscalls.o \
./Src/sysmem.o \
./Src/system_stm32f4xx.o 

C_DEPS += \
./Src/main.d \
./Src/pv_audio_rec.d \
./Src/pv_stm32f469.d \
./Src/stm32f4xx_it.d \
./Src/syscalls.d \
./Src/sysmem.d \
./Src/system_stm32f4xx.d 


# Each subdirectory must supply rules for building sources it contributes
Src/main.o: ../Src/main.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu99 -DSTM32 -DSTM32F469NIHx -DSTM32F469xx -DSTM32F4 -DSTM32F469I_DISCO -c -I../../../../../include -I../Inc -I../Middlewares/ST/STM32_Audio/Addons/PDM/Inc -I../Drivers/CMSIS/Include -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/BSP/STM32469I-Discovery -O2 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Src/main.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Src/pv_audio_rec.o: ../Src/pv_audio_rec.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu99 -DSTM32 -DSTM32F469NIHx -DSTM32F469xx -DSTM32F4 -DSTM32F469I_DISCO -c -I../../../../../include -I../Inc -I../Middlewares/ST/STM32_Audio/Addons/PDM/Inc -I../Drivers/CMSIS/Include -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/BSP/STM32469I-Discovery -O2 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Src/pv_audio_rec.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Src/pv_stm32f469.o: ../Src/pv_stm32f469.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu99 -DSTM32 -DSTM32F469NIHx -DSTM32F469xx -DSTM32F4 -DSTM32F469I_DISCO -c -I../../../../../include -I../Inc -I../Middlewares/ST/STM32_Audio/Addons/PDM/Inc -I../Drivers/CMSIS/Include -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/BSP/STM32469I-Discovery -O2 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Src/pv_stm32f469.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Src/stm32f4xx_it.o: ../Src/stm32f4xx_it.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu99 -DSTM32 -DSTM32F469NIHx -DSTM32F469xx -DSTM32F4 -DSTM32F469I_DISCO -c -I../../../../../include -I../Inc -I../Middlewares/ST/STM32_Audio/Addons/PDM/Inc -I../Drivers/CMSIS/Include -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/BSP/STM32469I-Discovery -O2 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Src/stm32f4xx_it.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Src/syscalls.o: ../Src/syscalls.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu99 -DSTM32 -DSTM32F469NIHx -DSTM32F469xx -DSTM32F4 -DSTM32F469I_DISCO -c -I../../../../../include -I../Inc -I../Middlewares/ST/STM32_Audio/Addons/PDM/Inc -I../Drivers/CMSIS/Include -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/BSP/STM32469I-Discovery -O2 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Src/syscalls.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Src/sysmem.o: ../Src/sysmem.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu99 -DSTM32 -DSTM32F469NIHx -DSTM32F469xx -DSTM32F4 -DSTM32F469I_DISCO -c -I../../../../../include -I../Inc -I../Middlewares/ST/STM32_Audio/Addons/PDM/Inc -I../Drivers/CMSIS/Include -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/BSP/STM32469I-Discovery -O2 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Src/sysmem.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"
Src/system_stm32f4xx.o: ../Src/system_stm32f4xx.c
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu99 -DSTM32 -DSTM32F469NIHx -DSTM32F469xx -DSTM32F4 -DSTM32F469I_DISCO -c -I../../../../../include -I../Inc -I../Middlewares/ST/STM32_Audio/Addons/PDM/Inc -I../Drivers/CMSIS/Include -I../Drivers/CMSIS/Device/ST/STM32F4xx/Include -I../Drivers/STM32F4xx_HAL_Driver/Inc -I../Drivers/BSP/STM32469I-Discovery -O2 -ffunction-sections -fdata-sections -Wall -fstack-usage -MMD -MP -MF"Src/system_stm32f4xx.d" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

