#include <stdio.h>
#include "pico/stdlib.h"
#include "hardware/gpio.h"
#include "hardware/pio.h"
#include "hardware/i2c.h"

// from https://github.com/endail/pico-scale hx711-pico-c
#include "extern/pico-scale/include/scale.h"
#include "extern/pico-scale/extern/hx711-pico-c/include/hx711_noblock.pio.h"

// from https://github.com/daschr/pico-ssd1306 (owner doesn't have a proper way to add project, instructs to just copy files in manually)
#include "extern/pico-ssd1306/src/ssd1306.h"

int main() {
    // init serial connection
    stdio_init_all();

    sleep_ms(2000);
    printf("\nHello World!\n");
    
    // init display --------------------------------------------------------------

    i2c_init(i2c1, 400000);
    gpio_set_function(20, GPIO_FUNC_I2C);
    gpio_set_function(21, GPIO_FUNC_I2C);
    gpio_pull_up(20);
    gpio_pull_up(21);

    ssd1306_t disp;
    disp.external_vcc=false;
    ssd1306_init(&disp, 128, 64, 0x3C, i2c1);
    ssd1306_clear(&disp);

    // TEMP just show something to show it works
    ssd1306_draw_string(&disp, 8, 24, 2, "Hello, World!");
    ssd1306_show(&disp);

    // init HX711 ----------------------------------------------------------------

    hx711_t hx;

    { // 1. Initialise the HX711
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
    // init scales --------------------------------------------------------------
    
    scale_t sc;
 
    // the values obtained when calibrating the scale
    // if you don't know them, read the following section How to Calibrate
    mass_unit_t scaleUnit = mass_g;
    int32_t refUnit = -247;
    int32_t offset = -353047;

    scale_init(
        &sc,
        &hx, // pass a pointer to the hx711_t
        scaleUnit,
        refUnit,
        offset);

    // 3. Set options for how the scale will read and interpret values

    // SCALE_DEFAULT_OPTIONS will give some default settings which you
    // do not have to use
    scale_options_t opt = SCALE_DEFAULT_OPTIONS;

    // scale_options_t has the following options
    //
    // opt.strat, which defines how the scale will collect data. By default,
    // data is collected according to the number of samples. So opt.strat
    // is set to strategy_type_samples. opt.samples defines how many samples
    // to obtain. You can also set opt.strat to read_type_time which will
    // collect as many samples as possible within the timeout period. The
    // timeout period is defined by opt.timeout and is given in microseconds
    // (us). For example, 1 second is equal to 1,000,000 us.
    //
    // opt.read, which defines how the scale will interpret data. By default,
    // data is interpreted according to the median value. So opt.read is set
    // to read_type_median. You can also set opt.read to read_type_average
    // which will calculate the average value.
    //
    // Example:
    //
    // opt.strat = strategy_type_time;
    // opt.read = read_type_average;
    // opt.timeout = 250000;
    //
    // These options mean... collect as many samples as possible within 250ms
    // and then use the average of all those samples.

    // 4. Zero the scale (OPTIONAL) (aka. tare)

    if(scale_zero(&sc, &opt)) {
        printf("Scale zeroed successfully\n");
    }
    else {
        printf("Scale failed to zero\n");
    }

    // 5. Obtain the weight
    mass_t mass;

    while (1) {
        if(scale_weight(&sc, &mass, &opt)) {

            // mass will contain the weight on the scale obtanined and interpreted
            // according to the given options and be in the unit defined by the
            // mass_unit_t 'scaleUnit' variable above
            //
            // you can now:

            // get the weight as a numeric value according to the mass_unit_t
            double val;
            mass_get_value(&mass, &val);

            // convert the mass to a string
            char buff[MASS_TO_STRING_BUFF_SIZE];
            mass_to_string(&mass, buff);
            printf("%s\n", buff);

            // or do other operations (see: mass.h file)

        }
        else {
            printf("Failed to read weight\n");
        }

        sleep_ms(500); // read again in half a second
    }

}