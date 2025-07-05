export PICO_SDK_PATH=/home/priyanka/pico-sdk
mkdir build
cd build
cmake -DPICO_BOARD=pico2_w ..
make rpi2350_ha