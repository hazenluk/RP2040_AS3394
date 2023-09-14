#include "ADSR.h"
#include "LFO.h"
#include "pico/stdlib.h"

bool realtime(repeating_timer_t *rt, void* user_data);

int main(){
    repeating_timer_t timer;
    add_repeating_timer_us(-100, realtime, NULL, &timer);
    sleep_ms(10000); //run for 10 seconds, then quit
    return 0;
}

bool realtime(repeating_timer_t *rt, void* user_data){
    // Realtime loop for modulation etc.

    return true;
}