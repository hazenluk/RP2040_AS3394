#include "LFO.h"

int LFO_process(LFO* ptr){
    ptr->phase_accumulator += ptr->FTW;
    ptr->output = *(ptr->wavetable)[ptr->phase_accumulator/1048576];
    return ptr->output;
}

int LFO_getOutput(LFO* ptr){
    return ptr->output;
}

void LFO_init(LFO* ptr, uint32_t f_clock){
    ptr->FTW_coefficient = (float)4294967296/f_clock;
}

void LFO_setFrequency(LFO* ptr, float f_out){
    ptr->FTW = (uint32_t)(ptr->FTW_coefficient * f_out);
}

void LFO_setFTW(LFO* ptr, uint32_t FTW){
    ptr->FTW = FTW;
}

void LFO_setPhase(LFO* ptr, uint32_t phase){
    ptr->phase_accumulator = phase;
}

void LFO_setWaveform(LFO* ptr, const int16_t (*wavetable)[4096]){
    ptr->wavetable = wavetable;
}