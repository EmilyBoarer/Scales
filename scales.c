#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"

// from https://github.com/endail/pico-scale hx711-pico-c
#include "extern/pico-scale/include/scale.h"
#include "extern/pico-scale/extern/hx711-pico-c/include/hx711_noblock.pio.h"

int main() {
    // init serial connection
    stdio_init_all();

    sleep_ms(2000);
    printf("Hello World!\n");
    
    // test code to check it works ----------- \/

    
}