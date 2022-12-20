#include "pico/stdlib.h"
#include "hardware/pwm.h"
#include "hardware/adc.h"

#define PWM_PERIOD 4096

#define FILTER_PIN 8
#define RES_PIN 10
#define GAIN_PIN 12
#define MIXER_PIN 14
#define PWM_PIN 16
#define WAVE_SEL_MSB 18
#define WAVE_SEL_LSB 17
#define MOD_PIN 19
#define VCO_PIN 20

// TODO: Consider making an enum type for pwm pins and then making an array so it can be iterated over?

int main() {
    // Init GPIOs allocated to pwm
    gpio_set_function(FILTER_PIN, GPIO_FUNC_PWM);
    gpio_set_function(RES_PIN, GPIO_FUNC_PWM);
    gpio_set_function(GAIN_PIN, GPIO_FUNC_PWM);
    gpio_set_function(MIXER_PIN, GPIO_FUNC_PWM);
    gpio_set_function(PWM_PIN, GPIO_FUNC_PWM);
    gpio_set_function(MOD_PIN, GPIO_FUNC_PWM);
    gpio_set_function(VCO_PIN, GPIO_FUNC_PWM);

    // Init wave select GPIOS
    gpio_init(WAVE_SEL_MSB);
    gpio_init(WAVE_SEL_LSB);
    gpio_set_dir(WAVE_SEL_MSB, GPIO_OUT);
    gpio_set_dir(WAVE_SEL_LSB, GPIO_OUT);

    // Init ADC GPIO
    adc_init();
    adc_gpio_init(26);
    adc_select_input(0);

    // Find out which PWM slice is connected to which GPIO
    uint filter_slice = pwm_gpio_to_slice_num(FILTER_PIN);
    uint res_slice = pwm_gpio_to_slice_num(RES_PIN);
    uint gain_slice = pwm_gpio_to_slice_num(GAIN_PIN);
    uint mixer_slice = pwm_gpio_to_slice_num(MIXER_PIN);
    uint pwm_slice = pwm_gpio_to_slice_num(PWM_PIN);
    uint mod_slice = pwm_gpio_to_slice_num(MOD_PIN);
    uint vco_slice = pwm_gpio_to_slice_num(VCO_PIN);

    // Set PWM periods
    pwm_set_wrap(filter_slice, 4096);
    pwm_set_wrap(res_slice, 4096);
    pwm_set_wrap(gain_slice, 4096);
    pwm_set_wrap(mixer_slice, 4096);
    pwm_set_wrap(pwm_slice, 4096);
    pwm_set_wrap(mod_slice, 4096);
    pwm_set_wrap(vco_slice, 4096);

    // Set pwm channels high for N cycles
    pwm_set_gpio_level(FILTER_PIN, 100);
    pwm_set_gpio_level(RES_PIN, 0);
    pwm_set_gpio_level(GAIN_PIN, 2000); // 3000
    pwm_set_gpio_level(MIXER_PIN, 400); // Main out
    //pwm_set_gpio_level(MIXER_PIN, 3700); // Sub out
    pwm_set_gpio_level(PWM_PIN, 0);
    pwm_set_gpio_level(MOD_PIN, 0);
    pwm_set_gpio_level(VCO_PIN, 2292); //2292 = A440 with 5V reg instead of 6.5

    // Set the PWM running
    pwm_set_enabled(filter_slice, true);
    pwm_set_enabled(res_slice, true);
    pwm_set_enabled(gain_slice, true);
    pwm_set_enabled(mixer_slice, true);
    pwm_set_enabled(pwm_slice, true);
    pwm_set_enabled(mod_slice, true);
    pwm_set_enabled(vco_slice, true);

    // Wave_Sel = 00 = sawtooth
    gpio_put(WAVE_SEL_MSB, 0);
    gpio_put(WAVE_SEL_LSB, 0);
}
