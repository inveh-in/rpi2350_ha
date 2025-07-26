# rpi2350_ha
Home Automation using RP2350 PICO2W

**To install MQTT:**
git clone https://github.com/eclipse/paho.mqtt.c.git
cd paho.mqtt.c

mkdir build
cd build

$ cmake -GNinja -DPAHO_WITH_SSL=FALSE -DPAHO_BUILD_SAMPLES=TRUE \
    -DPAHO_BUILD_DOCUMENTATION=TRUE \
    -DOPENSSL_LIB_SEARCH_PATH=/tmp/libssl-dev/usr/lib/arm-linux-gnueabihf \
    -DOPENSSL_INC_SEARCH_PATH="/tmp/libssl-dev/usr/include/openssl;/tmp/libssl-dev/usr/include/arm-linux-gnueabihf" \
    -DCMAKE_TOOLCHAIN_FILE=~/paho.mqtt.c/cmake/toolchain.linux-arm11.cmake \
    ~/paho.mqtt.c

cmake --build .
sudo cmake --build . --target install
sudo cmake --build . --target package
sudo ldconfig



