export PICO_SDK_PATH=/home/priyanka/pico-sdk
export PICOTOOL_FETCH_FROM_GIT_PATH=/home/priyanka/picotool
export FREERTOS_KERNEL_PATH=/home/priyanka/FreeRTOS-Kernel

mkdir build
cd build
cmake -DPICO_BOARD=pico2_w ..
make rpi2350_ha