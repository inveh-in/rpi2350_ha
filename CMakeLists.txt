cmake_minimum_required(VERSION 3.5)

set(FREERTOS_KERNEL_PATH "/home/priyanka/FreeRTOS-Kernel")
set(PICO_SDK_PATH "/home/priyanka/pico-sdk")
set(PICOTOOL_FETCH_FROM_GIT_PATH "/home/priyanka/picotool")

# Pull in SDK (must be before project)
include(${PICO_SDK_PATH}/external/pico_sdk_import.cmake)

# Pull in FreeRTOS
include(${FREERTOS_KERNEL_PATH}/portable/ThirdParty/Community-Supported-Ports/GCC/RP2350_ARM_NTZ/FreeRTOS_Kernel_import.cmake)

set(PICO_PLATFORM=rp2350)

project(rpi2350_ha C CXX ASM)
set(CMAKE_C_STANDARD 11)
set(CMAKE_CXX_STANDARD 17)

# initialize the Raspberry Pi Pico SDK
pico_sdk_init()

# rest of your project
add_executable(rpi2350_ha
    rpi2350_ha_ble.c
    rpi2350_ha_wifi.c
    rpi2350_ha_main.c
)

# Add include directories
target_include_directories(rpi2350_ha
	PRIVATE ${CMAKE_CURRENT_LIST_DIR}
    PUBLIC ${FREERTOS_KERNEL_PATH}/include
)

# Add pico_stdlib library which aggregates commonly used features
target_link_libraries(rpi2350_ha FreeRTOS-Kernel 
FreeRTOS-Kernel-Heap4 
pico_stdlib
pico_cyw43_arch_lwip_poll
pico_btstack_ble
pico_btstack_cyw43
)

# enable usb output, disable uart output
pico_enable_stdio_usb(rpi2350_ha 1)
pico_enable_stdio_uart(rpi2350_ha 0)

# create map/bin/hex/uf2 file in addition to ELF.
pico_add_extra_outputs(rpi2350_ha)
