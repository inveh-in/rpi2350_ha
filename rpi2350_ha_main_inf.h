/**
 * Copyright (c) inveh.
 *
 */

#include <stdio.h>

#include "pico/stdlib.h"
#include "pico/multicore.h"
#include "pico/cyw43_arch.h"    // Pico W devices use a GPIO on the WIFI chip for the LED
#include "pico/async_context_freertos.h"

#include "FreeRTOS.h"
#include "task.h"
#include "FreeRTOSConfig.h
