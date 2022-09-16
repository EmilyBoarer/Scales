#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"

// from https://github.com/endail/hx711-pico-c
#include "extern/hx711-pico-c/include/hx711.h"
#include "extern/hx711-pico-c/include/hx711_noblock.pio.h" // for hx711_noblock_program and hx711_noblock_program_init

int main() {
    // init serial connection
    stdio_init_all();

    sleep_ms(2000);
    printf("Hello World!\n");
    
    // test code to check it works ----------- \/

    hx711_t hx;

    // 1. Initialise the HX711
    hx711_init(
        &hx,
        27, // Pico GPIO pin connected to HX711's clock pin
        26, // Pico GPIO pin connected to HX711's data pin
        pio0, // the RP2040 PIO to use (either pio0 or pio1)
        &hx711_noblock_program, // the state machine program
        &hx711_noblock_program_init); // the state machine program init function

    // 2. Power up
    hx711_set_power(&hx, hx711_pwr_up);

    // 3. [OPTIONAL] set gain and save it to the HX711
    // chip by powering down then back up
    hx711_set_gain(&hx, hx711_gain_128);
    hx711_set_power(&hx, hx711_pwr_down);
    hx711_wait_power_down();
    hx711_set_power(&hx, hx711_pwr_up);

    // 4. Wait for readings to settle
    hx711_wait_settle(hx711_rate_10); // or hx711_rate_80 depending on your chip's config

    // 5. Read values
    int32_t val;

    // wait (block) until a value is read
    val = hx711_get_value(&hx);

    // or use a timeout
    if(hx711_get_value_timeout(&hx, 250000, &val)) {
        // value was obtained within the timeout period
        // in this case, within 250 milliseconds
        printf("%li\n", val);
    }
}