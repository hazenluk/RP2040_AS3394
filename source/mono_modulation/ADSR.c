//  ADSR.c
//
//  Originally Created by Nigel Redmon on 12/18/12.
//  EarLevel Engineering: earlevel.com
//  Copyright 2012 Nigel Redmon
//  C Port 2013 Francesco D'Este

//
//  For a complete explanation of the ADSR envelope generator and code,
//  read the series of articles by the author, starting here:
//  http://www.earlevel.com/main/2013/06/01/envelope-generators/
//
//  License:
//
//  This source code is provided as is, without warranty.
//  You may copy and distribute verbatim copies of this document.
//  You may modify and use this source code to create binary code for your own purposes, free or commercial.
//

#include "ADSR.h"
#include <math.h>

 

void ADSR_init(ADSR* ptr){
    ADSR_reset(ptr);
  
    ADSR_setAttackRate(ptr,0);
    ADSR_setDecayRate(ptr,0);
    ADSR_setReleaseRate(ptr,0);
    ADSR_setSustainLevel(ptr,1.0);
    ADSR_setTargetRatioA(ptr,0.3);
    ADSR_setTargetRatioDR(ptr,0.0001);
	
}

void ADSR_setAttackRate(ADSR* ptr, float rate) {
    ptr->attackRate = rate;
    ptr->attackCoef = ADSR_calcCoef(rate, ptr->targetRatioA);
    ptr->attackBase = (1.0 + ptr->targetRatioA) * (1.0 - ptr->attackCoef);
}

void ADSR_setDecayRate(ADSR* ptr,float rate) {
    ptr->decayRate = rate;
    ptr->decayCoef = ADSR_calcCoef(rate, ptr->targetRatioDR);
    ptr->decayBase = (ptr->sustainLevel - ptr->targetRatioDR) * (1.0 - ptr->decayCoef);
}

void ADSR_setReleaseRate(ADSR* ptr, float rate) {
    ptr->releaseRate = rate;
    ptr->releaseCoef = ADSR_calcCoef(rate, ptr->targetRatioDR);
    ptr->releaseBase = -ptr->targetRatioDR * (1.0 - ptr->releaseCoef);
}

float ADSR_calcCoef(float rate, float targetRatio) {
    return exp(-log((1.0 + targetRatio) / targetRatio) / rate);
}

void ADSR_setSustainLevel(ADSR* ptr, float level) {
    ptr->sustainLevel = level;
    ptr->decayBase = (ptr->sustainLevel - ptr->targetRatioDR) * (1.0 - ptr->decayCoef);
}

void ADSR_setTargetRatioA(ADSR* ptr, float targetRatio) {
    if (targetRatio < 0.000000001)
        targetRatio = 0.000000001;  // -180 dB
    ptr->targetRatioA = targetRatio;
    ptr->attackBase = (1.0 + ptr->targetRatioA) * (1.0 - ptr->attackCoef);
}

void ADSR_setTargetRatioDR(ADSR* ptr, float targetRatio) {
    if (targetRatio < 0.000000001)
        targetRatio = 0.000000001;  // -180 dB
    ptr->targetRatioDR = targetRatio;
    ptr->decayBase = (ptr->sustainLevel - ptr->targetRatioDR) * (1.0 - ptr->decayCoef);
    ptr->releaseBase = -ptr->targetRatioDR * (1.0 - ptr->releaseCoef);
}

float ADSR_process(ADSR* ptr) {
	switch (ptr->state) {
        case env_idle:
            break;
        case env_attack:
            ptr->output = ptr->attackBase + ptr->output * ptr->attackCoef;
            if (ptr->output >= 1.0) {
                ptr->output = 1.0;
                ptr->state = env_decay;
            }
            break;
        case env_decay:
            ptr->output = ptr->decayBase + ptr->output * ptr->decayCoef;
            if (ptr->output <= ptr->sustainLevel) {
                ptr->output = ptr->sustainLevel;
                ptr->state = env_sustain;
            }
            break;
        case env_sustain:
            break;
        case env_release:
            ptr->output = ptr->releaseBase + ptr->output * ptr->releaseCoef;
            if (ptr->output <= 0.0) {
                ptr->output = 0.0;
                ptr->state = env_idle;
            }
	}
	return ptr->output;
}

void ADSR_gate(ADSR* ptr) {
	if (ptr->gate == 0){
		ptr->state = env_attack;
		ptr->gate = 1;
	}
	else if (ptr->state != env_idle){
        ptr->state = env_release;
		ptr->gate = 0;
		}
}

int ADSR_getState(ADSR* ptr) {
    return ptr->state;
}

void ADSR_reset(ADSR* ptr) {
	ptr->gate = 0;
    ptr->state = env_idle;
    ptr->output = 0.0;
}

float ADSR_getOutput(ADSR* ptr) {
	return ptr->output;
}
