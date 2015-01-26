#include <p24FJ128GB206.h>
#include "config.h"
#include "common.h"
#include "ui.h"
#include "timer.h"

int16_t main(void) {
    init_clock();
    init_ui();
    init_timer();

    led_on(&led1);
    timer_setPeriod(&timer2, 0.5);
    timer_start(&timer2);

    int light = 0;

    while (1) {
        if (timer_flag(&timer2)) {
            timer_lower(&timer2);

            light = light % 3;
            
            led_write(&led1,!(light));
            led_write(&led2,!(light-1));
            led_write(&led3,!(light-2)); 

            light++;              
    }
}
}

