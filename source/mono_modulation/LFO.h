#include <stdint.h>

#ifndef LFO_h
#define LFO_h

    // TODO: Consider changing FTW_coefficient and setFreq arg from floats to ints. Need to determine effect on accuracy
    // Effect on accuracy would be minimal. However, need to determine if int * float is faster than float*float
	
	typedef struct {
        uint32_t phase_accumulator;
        uint32_t FTW;
        float FTW_coefficient; // 2^n / f_clock, FTW = FTW_coefficient * f_out
        const int16_t (*wavetable)[4096];
        int16_t output;
	} LFO;
	
    // Advance phase accumulator by FTW and calculate output
	int LFO_process(LFO* ptr);

    // Returns the last output computed by LFO_process
    int LFO_getOutput(LFO* ptr);

    // precompute FTW_coefficient for a given sample rate
    void LFO_init(LFO* ptr, uint32_t f_clock);

    // FTW = round(FTW_coefficient*f_out)
    void LFO_setFrequency(LFO* ptr, float f_out);

    // Directly set the FTW. Faster than computing for a specific frequency
    void LFO_setFTW(LFO* ptr, uint32_t FTW);

    // Set the phase. Phase is between 0 and 2^32 - 1
    void LFO_setPhase(LFO* ptr, uint32_t phase);

    // Set the LFO waveform to an array of const int16_t values, length 4096. Waveform samples should be in range [-4096, 4096]
    void LFO_setWaveform(LFO* ptr, const int16_t (*wavetable)[4096]);





#endif