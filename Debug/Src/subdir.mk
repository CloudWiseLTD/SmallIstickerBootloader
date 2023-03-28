################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (10.3-2021.10)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../Src/startup_stm32l433xx.s 

C_SRCS += \
../Src/ADC.c \
../Src/Common.c \
../Src/Download.c \
../Src/DvcPrm.c \
../Src/FlashApp.c \
../Src/FlashDrv.c \
../Src/FlashInnApp.c \
../Src/HostApp.c \
../Src/LED.c \
../Src/RST.c \
../Src/RTC.c \
../Src/SPI.c \
../Src/TMRS.c \
../Src/UART.c \
../Src/UARTDrv.c \
../Src/Ymodem.c \
../Src/main.c \
../Src/stm32l4xx_it.c \
../Src/system_stm32l4xx.c \
../Src/tiny_printf.c 

OBJS += \
./Src/ADC.o \
./Src/Common.o \
./Src/Download.o \
./Src/DvcPrm.o \
./Src/FlashApp.o \
./Src/FlashDrv.o \
./Src/FlashInnApp.o \
./Src/HostApp.o \
./Src/LED.o \
./Src/RST.o \
./Src/RTC.o \
./Src/SPI.o \
./Src/TMRS.o \
./Src/UART.o \
./Src/UARTDrv.o \
./Src/Ymodem.o \
./Src/main.o \
./Src/startup_stm32l433xx.o \
./Src/stm32l4xx_it.o \
./Src/system_stm32l4xx.o \
./Src/tiny_printf.o 

S_DEPS += \
./Src/startup_stm32l433xx.d 

C_DEPS += \
./Src/ADC.d \
./Src/Common.d \
./Src/Download.d \
./Src/DvcPrm.d \
./Src/FlashApp.d \
./Src/FlashDrv.d \
./Src/FlashInnApp.d \
./Src/HostApp.d \
./Src/LED.d \
./Src/RST.d \
./Src/RTC.d \
./Src/SPI.d \
./Src/TMRS.d \
./Src/UART.d \
./Src/UARTDrv.d \
./Src/Ymodem.d \
./Src/main.d \
./Src/stm32l4xx_it.d \
./Src/system_stm32l4xx.d \
./Src/tiny_printf.d 


# Each subdirectory must supply rules for building sources it contributes
Src/%.o Src/%.su Src/%.cyclo: ../Src/%.c Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g -c -I"C:/Projects/CloudWise/IStickerBootloaderOpt/TinyBoot/Hdr" -I"C:/Projects/CloudWise/IStickerBootloaderOpt/TinyBoot/Src" -I"C:/Projects/CloudWise/IStickerBootloaderOpt/TinyBoot/Device/CMSIS" -I"C:/Projects/CloudWise/IStickerBootloaderOpt/TinyBoot/HAL/inc" -I"C:/Projects/CloudWise/IStickerBootloaderOpt/TinyBoot/Device/ST" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"
Src/%.o: ../Src/%.s Src/subdir.mk
	arm-none-eabi-gcc -mcpu=cortex-m4 -g -c -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@" "$<"

clean: clean-Src

clean-Src:
	-$(RM) ./Src/ADC.cyclo ./Src/ADC.d ./Src/ADC.o ./Src/ADC.su ./Src/Common.cyclo ./Src/Common.d ./Src/Common.o ./Src/Common.su ./Src/Download.cyclo ./Src/Download.d ./Src/Download.o ./Src/Download.su ./Src/DvcPrm.cyclo ./Src/DvcPrm.d ./Src/DvcPrm.o ./Src/DvcPrm.su ./Src/FlashApp.cyclo ./Src/FlashApp.d ./Src/FlashApp.o ./Src/FlashApp.su ./Src/FlashDrv.cyclo ./Src/FlashDrv.d ./Src/FlashDrv.o ./Src/FlashDrv.su ./Src/FlashInnApp.cyclo ./Src/FlashInnApp.d ./Src/FlashInnApp.o ./Src/FlashInnApp.su ./Src/HostApp.cyclo ./Src/HostApp.d ./Src/HostApp.o ./Src/HostApp.su ./Src/LED.cyclo ./Src/LED.d ./Src/LED.o ./Src/LED.su ./Src/RST.cyclo ./Src/RST.d ./Src/RST.o ./Src/RST.su ./Src/RTC.cyclo ./Src/RTC.d ./Src/RTC.o ./Src/RTC.su ./Src/SPI.cyclo ./Src/SPI.d ./Src/SPI.o ./Src/SPI.su ./Src/TMRS.cyclo ./Src/TMRS.d ./Src/TMRS.o ./Src/TMRS.su ./Src/UART.cyclo ./Src/UART.d ./Src/UART.o ./Src/UART.su ./Src/UARTDrv.cyclo ./Src/UARTDrv.d ./Src/UARTDrv.o ./Src/UARTDrv.su ./Src/Ymodem.cyclo ./Src/Ymodem.d ./Src/Ymodem.o ./Src/Ymodem.su ./Src/main.cyclo ./Src/main.d ./Src/main.o ./Src/main.su ./Src/startup_stm32l433xx.d ./Src/startup_stm32l433xx.o ./Src/stm32l4xx_it.cyclo ./Src/stm32l4xx_it.d ./Src/stm32l4xx_it.o ./Src/stm32l4xx_it.su ./Src/system_stm32l4xx.cyclo ./Src/system_stm32l4xx.d ./Src/system_stm32l4xx.o ./Src/system_stm32l4xx.su ./Src/tiny_printf.cyclo ./Src/tiny_printf.d ./Src/tiny_printf.o ./Src/tiny_printf.su

.PHONY: clean-Src

