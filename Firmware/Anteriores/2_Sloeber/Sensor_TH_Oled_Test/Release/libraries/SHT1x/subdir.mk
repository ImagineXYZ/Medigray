################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
/home/bryanmtdt/Arduino/libraries/SHT1x/SHT1x.cpp 

LINK_OBJ += \
./libraries/SHT1x/SHT1x.cpp.o 

CPP_DEPS += \
./libraries/SHT1x/SHT1x.cpp.d 


# Each subdirectory must supply rules for building sources it contributes
libraries/SHT1x/SHT1x.cpp.o: /home/bryanmtdt/Arduino/libraries/SHT1x/SHT1x.cpp
	@echo 'Building file: $<'
	@echo 'Starting C++ compile'
	"/home/bryanmtdt/Documents/sloeber4.1//arduinoPlugin/packages/espressif/esp32/tools/xtensa-esp32-elf/bin/xtensa-esp32-elf-g++" -DESP_PLATFORM '-DMBEDTLS_CONFIG_FILE="mbedtls/esp_config.h"' -DHAVE_CONFIG_H "-I/home/bryanmtdt/Documents/sloeber4.1//arduinoPlugin/packages/espressif/esp32/tools/sdk/include/config" "-I/home/bryanmtdt/Documents/sloeber4.1//arduinoPlugin/packages/espressif/esp32/tools/sdk/include/bluedroid" "-I/home/bryanmtdt/Documents/sloeber4.1//arduinoPlugin/packages/espressif/esp32/tools/sdk/include/app_trace" "-I/home/bryanmtdt/Documents/sloeber4.1//arduinoPlugin/packages/espressif/esp32/tools/sdk/include/app_update" "-I/home/bryanmtdt/Documents/sloeber4.1//arduinoPlugin/packages/espressif/esp32/tools/sdk/include/bootloader_support" "-I/home/bryanmtdt/Documents/sloeber4.1//arduinoPlugin/packages/espressif/esp32/tools/sdk/include/bt" "-I/home/bryanmtdt/Documents/sloeber4.1//arduinoPlugin/packages/espressif/esp32/tools/sdk/include/driver" "-I/home/bryanmtdt/Documents/sloeber4.1//arduinoPlugin/packages/espressif/esp32/tools/sdk/include/esp32" "-I/home/bryanmtdt/Documents/sloeber4.1//arduinoPlugin/packages/espressif/esp32/tools/sdk/include/ethernet" "-I/home/bryanmtdt/Documents/sloeber4.1//arduinoPlugin/packages/espressif/esp32/tools/sdk/include/fatfs" "-I/home/bryanmtdt/Documents/sloeber4.1//arduinoPlugin/packages/espressif/esp32/tools/sdk/include/freertos" "-I/home/bryanmtdt/Documents/sloeber4.1//arduinoPlugin/packages/espressif/esp32/tools/sdk/include/heap" "-I/home/bryanmtdt/Documents/sloeber4.1//arduinoPlugin/packages/espressif/esp32/tools/sdk/include/jsmn" "-I/home/bryanmtdt/Documents/sloeber4.1//arduinoPlugin/packages/espressif/esp32/tools/sdk/include/log" "-I/home/bryanmtdt/Documents/sloeber4.1//arduinoPlugin/packages/espressif/esp32/tools/sdk/include/mdns" "-I/home/bryanmtdt/Documents/sloeber4.1//arduinoPlugin/packages/espressif/esp32/tools/sdk/include/mbedtls" "-I/home/bryanmtdt/Documents/sloeber4.1//arduinoPlugin/packages/espressif/esp32/tools/sdk/include/mbedtls_port" "-I/home/bryanmtdt/Documents/sloeber4.1//arduinoPlugin/packages/espressif/esp32/tools/sdk/include/newlib" "-I/home/bryanmtdt/Documents/sloeber4.1//arduinoPlugin/packages/espressif/esp32/tools/sdk/include/nvs_flash" "-I/home/bryanmtdt/Documents/sloeber4.1//arduinoPlugin/packages/espressif/esp32/tools/sdk/include/openssl" "-I/home/bryanmtdt/Documents/sloeber4.1//arduinoPlugin/packages/espressif/esp32/tools/sdk/include/soc" "-I/home/bryanmtdt/Documents/sloeber4.1//arduinoPlugin/packages/espressif/esp32/tools/sdk/include/spi_flash" "-I/home/bryanmtdt/Documents/sloeber4.1//arduinoPlugin/packages/espressif/esp32/tools/sdk/include/sdmmc" "-I/home/bryanmtdt/Documents/sloeber4.1//arduinoPlugin/packages/espressif/esp32/tools/sdk/include/tcpip_adapter" "-I/home/bryanmtdt/Documents/sloeber4.1//arduinoPlugin/packages/espressif/esp32/tools/sdk/include/ulp" "-I/home/bryanmtdt/Documents/sloeber4.1//arduinoPlugin/packages/espressif/esp32/tools/sdk/include/vfs" "-I/home/bryanmtdt/Documents/sloeber4.1//arduinoPlugin/packages/espressif/esp32/tools/sdk/include/wear_levelling" "-I/home/bryanmtdt/Documents/sloeber4.1//arduinoPlugin/packages/espressif/esp32/tools/sdk/include/xtensa-debug-module" "-I/home/bryanmtdt/Documents/sloeber4.1//arduinoPlugin/packages/espressif/esp32/tools/sdk/include/newlib" "-I/home/bryanmtdt/Documents/sloeber4.1//arduinoPlugin/packages/espressif/esp32/tools/sdk/include/coap" "-I/home/bryanmtdt/Documents/sloeber4.1//arduinoPlugin/packages/espressif/esp32/tools/sdk/include/wpa_supplicant" "-I/home/bryanmtdt/Documents/sloeber4.1//arduinoPlugin/packages/espressif/esp32/tools/sdk/include/expat" "-I/home/bryanmtdt/Documents/sloeber4.1//arduinoPlugin/packages/espressif/esp32/tools/sdk/include/json" "-I/home/bryanmtdt/Documents/sloeber4.1//arduinoPlugin/packages/espressif/esp32/tools/sdk/include/nghttp" "-I/home/bryanmtdt/Documents/sloeber4.1//arduinoPlugin/packages/espressif/esp32/tools/sdk/include/lwip" -std=gnu++11 -fno-exceptions -fno-rtti -Os -g3 -Wpointer-arith -ffunction-sections -fdata-sections -fstrict-volatile-bitfields -mlongcalls -nostdlib -Wall -Werror=all -Wextra -Wno-error=unused-function -Wno-error=unused-but-set-variable -Wno-error=unused-variable -Wno-error=deprecated-declarations -Wno-unused-parameter -Wno-sign-compare -fno-rtti -c -DF_CPU=240000000L -DARDUINO=10802 -DARDUINO_ESP32_THING -DARDUINO_ARCH_ESP32 -DESP32 -DCORE_DEBUG_LEVEL=0  -I"/home/bryanmtdt/Documents/sloeber4.1/arduinoPlugin/packages/espressif/esp32/cores/esp32" -I"/home/bryanmtdt/Documents/sloeber4.1/arduinoPlugin/packages/espressif/esp32/variants/esp32thing" -I"/home/bryanmtdt/Documents/sloeber4.1/arduinoPlugin/packages/espressif/esp32/libraries/SPI" -I"/home/bryanmtdt/Documents/sloeber4.1/arduinoPlugin/packages/espressif/esp32/libraries/SPI/src" -I"/home/bryanmtdt/Documents/sloeber4.1/arduinoPlugin/packages/espressif/esp32/libraries/Wire" -I"/home/bryanmtdt/Documents/sloeber4.1/arduinoPlugin/packages/espressif/esp32/libraries/Wire/src" -I"/home/bryanmtdt/Arduino/libraries/ESP8266_XYZ_StandAlone" -I"/home/bryanmtdt/Arduino/libraries/PubSubClient" -I"/home/bryanmtdt/Documents/sloeber4.1/arduinoPlugin/packages/espressif/esp32/libraries/WiFi" -I"/home/bryanmtdt/Documents/sloeber4.1/arduinoPlugin/packages/espressif/esp32/libraries/WiFi/src" -I"/home/bryanmtdt/Arduino/libraries/SHT1x" -I"/home/bryanmtdt/Arduino/libraries/pubsubclient" -I"/home/bryanmtdt/Arduino/libraries/pubsubclient/src" -I"/home/bryanmtdt/Arduino/libraries/Adafruit_SSD1306" -I"/home/bryanmtdt/Arduino/libraries/Adafruit-GFX" -I"/home/bryanmtdt/Documents/sloeber4.1/arduinoPlugin/packages/espressif/esp32/libraries/ArduinoOTA" -I"/home/bryanmtdt/Documents/sloeber4.1/arduinoPlugin/packages/espressif/esp32/libraries/ArduinoOTA/src" -I"/home/bryanmtdt/Documents/sloeber4.1/arduinoPlugin/packages/espressif/esp32/libraries/ESPmDNS" -I"/home/bryanmtdt/Documents/sloeber4.1/arduinoPlugin/packages/espressif/esp32/libraries/ESPmDNS/src" -I"/home/bryanmtdt/Documents/sloeber4.1/arduinoPlugin/packages/espressif/esp32/libraries/Update" -I"/home/bryanmtdt/Documents/sloeber4.1/arduinoPlugin/packages/espressif/esp32/libraries/Update/src" -I"/home/bryanmtdt/Documents/sloeber4.1/arduinoPlugin/packages/espressif/esp32/cores/esp32" -I"/home/bryanmtdt/Documents/sloeber4.1/arduinoPlugin/packages/espressif/esp32/variants/esp32thing" -I"/home/bryanmtdt/Arduino/libraries/Adafruit-GFX" -I"/home/bryanmtdt/Arduino/libraries/Adafruit_SSD1306" -I"/home/bryanmtdt/Documents/sloeber4.1/arduinoPlugin/packages/espressif/esp32/libraries/ArduinoOTA" -I"/home/bryanmtdt/Documents/sloeber4.1/arduinoPlugin/packages/espressif/esp32/libraries/ArduinoOTA/src" -I"/home/bryanmtdt/Arduino/libraries/ESP8266_XYZ_StandAlone" -I"/home/bryanmtdt/Documents/sloeber4.1/arduinoPlugin/packages/espressif/esp32/libraries/ESPmDNS" -I"/home/bryanmtdt/Documents/sloeber4.1/arduinoPlugin/packages/espressif/esp32/libraries/ESPmDNS/src" -I"/home/bryanmtdt/Arduino/libraries/pubsubclient" -I"/home/bryanmtdt/Arduino/libraries/pubsubclient/src" -I"/home/bryanmtdt/Arduino/libraries/SHT1x" -I"/home/bryanmtdt/Documents/sloeber4.1/arduinoPlugin/packages/espressif/esp32/libraries/SPI" -I"/home/bryanmtdt/Documents/sloeber4.1/arduinoPlugin/packages/espressif/esp32/libraries/SPI/src" -I"/home/bryanmtdt/Documents/sloeber4.1/arduinoPlugin/packages/espressif/esp32/libraries/Update" -I"/home/bryanmtdt/Documents/sloeber4.1/arduinoPlugin/packages/espressif/esp32/libraries/Update/src" -I"/home/bryanmtdt/Documents/sloeber4.1/arduinoPlugin/packages/espressif/esp32/libraries/WiFi" -I"/home/bryanmtdt/Documents/sloeber4.1/arduinoPlugin/packages/espressif/esp32/libraries/WiFi/src" -I"/home/bryanmtdt/Documents/sloeber4.1/arduinoPlugin/packages/espressif/esp32/libraries/Wire" -I"/home/bryanmtdt/Documents/sloeber4.1/arduinoPlugin/packages/espressif/esp32/libraries/Wire/src" -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -D__IN_ECLIPSE__=1 -x c++ "$<"  -o  "$@"
	@echo 'Finished building: $<'
	@echo ' '


