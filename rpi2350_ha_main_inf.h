/**
 * Copyright (c) inveh.
 *
 */

#include <stdio.h>

#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "pico/cyw43_arch.h"    // Pico W devices use a GPIO on the WIFI chip for the LED

#include "FreeRTOS.h"
#include "task.h"
