// Based on code from https://github.com/polykit/pico-dco

#include <stdio.h>
#include <stdlib.h>
#include "pico/stdlib.h"
#include "bsp/board.h"
#include "tusb.h"
#include "hardware/adc.h"
#include "hardware/pwm.h"
#include "sine.h"

void usb_midi_task();
void note_on(uint8_t note, uint8_t velocity);
void note_off(uint8_t note);

#define MIDI_CHANNEL 1

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

int midiNoteFreq[] = {8, 9, 9, 10, 10, 11, 12, 12, 13, 14, 15, 15, 16, 17, 18, 19, 21, 22, 23, 24, 26, 28, 29, 31, 33, 35, 37, 39, 41, 44, 46, 49, 52, 55, 58, 62, 65, 69, 73, 78, 82, 87, 92, 98, 104, 110, 117, 123, 131, 139, 147, 156, 165, 175, 185, 196, 208, 220, 233, 247, 262, 277, 294, 311, 330, 349, 370, 392, 415, 440, 466, 494, 523, 554, 587, 622, 659, 698, 740, 784, 831, 880, 932, 988, 1047, 1109, 1175, 1245, 1319, 1397, 1480, 1568, 1661, 1760, 1865, 1976, 2093, 2217, 2349, 2489, 2637, 2794, 2960, 3136, 3322, 3520, 3729, 3951, 4186, 4435, 4699, 4978, 5274, 5588, 5920, 6272, 6645, 7040, 7459, 7902, 8372, 8870, 9397, 9956, 10548, 11175, 11840, 12544};

// Synth Parameters
uint16_t master_gain = 3000;

int main() {
    board_init();
    tusb_init();
    adc_init();

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

    // Initial PWM values
    pwm_set_gpio_level(FILTER_PIN, 100);
    pwm_set_gpio_level(RES_PIN, 0);
    pwm_set_gpio_level(GAIN_PIN, 2000); // 3000
    pwm_set_gpio_level(MIXER_PIN, 400); // Main out
    //pwm_set_gpio_level(MIXER_PIN, 3700); // Sub out
    pwm_set_gpio_level(PWM_PIN, 0);
    pwm_set_gpio_level(MOD_PIN, 0);
    pwm_set_gpio_level(VCO_PIN, 2048);

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

    while (true) {
        tud_task();
        usb_midi_task();

        absolute_time_t time = get_absolute_time();

    }

    return 0;
}

void usb_midi_task() {
    if (tud_midi_available() < 4) return;

    uint8_t buff[4];
    uint8_t mono_last_note = 0;

    if (tud_midi_packet_read(buff)) {
        if (buff[1] == (0x90 | (MIDI_CHANNEL-1))) {
            if (buff[3] > 0) {
                mono_last_note = buff[2];
                note_on(buff[2], buff[3]);
            } else {
                note_off(buff[2]); //NOTE: techinically could maybe lead to issues in mono mode but should be a rare bug. Could fix by making mono_last_note global.
            }
        }

        if (buff[1] == (0x80 | (MIDI_CHANNEL-1))) {
            if(buff[2] == mono_last_note)
                note_off(buff[2]);
        }

        /*if (buff[1] == (0xE0 | (MIDI_CHANNEL-1))) {
            midi_pitch_bend = buff[2] | (buff[3]<<7);
        }

        if (midi_serial_status == (0xB0 | (MIDI_CHANNEL-1))) {
            if (buff[2] == 5) { // portamento time
                portamento_time = buff[3];
            }
            if (buff[2] == 65) { // portamento on/off
                portamento = buff[3] > 63;
            }
        }
        */
    }
}

void note_on(uint8_t note, uint8_t velocity) {
    //float freq = midiNoteFreq[note];
    pwm_set_gpio_level(GAIN_PIN, master_gain*velocity);
    uint16_t cycles = (69 - note)*33.52077 + 2292;
    pwm_set_gpio_level(VCO_PIN, cycles);
}

void note_off(uint8_t note) {
    pwm_set_gpio_level(GAIN_PIN, 400);
}