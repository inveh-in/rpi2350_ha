cmake_minimum_required(VERSION 3.13...3.27)

set(FREERTOS_KERNEL_PATH "/home/priyanka/FreeRTOS-Kernel")
set(PICO_SDK_PATH "/home/priyanka/pico-sdk")
set(PICOTOOL_FETCH_FROM_GIT_PATH "/home/priyanka/picotool")

# Add the freertos_config for FreeRTOS-Kernel
add_library(freertos_config INTERFACE)

target_include_directories(freertos_config
    INTERFACE
    "rtos_config/"
)

# initialize the SDK based on PICO_SDK_PATH
# note: this must happen before project()
include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)
include(rtos_config/FreeRTOS_Kernel_import.cmake)

set(PICO_PLATFORM=rp2350)

project(my_project)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# rest of your project
add_executable(rpi2350_ha
    rpi2350_ha_main.c
)

# Add include directories
target_include_directories(rpi2350_ha
    PUBLIC ${FREERTOS_KERNEL_PATH}/include
)

# Add pico_stdlib library which aggregates commonly used features
target_link_libraries(rpi2350_ha pico_stdlib pico_cyw43_arch_none)

# enable usb output, disable uart output
pico_enable_stdio_usb(rpi2350_ha 1)
pico_enable_stdio_uart(rpi2350_ha 0)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(rpi2350_ha)