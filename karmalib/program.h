/*
 * Karma softsynth
 * 
 * $Id: program.h,v 1.3 2003/12/30 16:09:09 quarn Exp $
 * Author : Fredrik Ehnbom
 * 
 * All rights reserved. Reproduction, modification, use or disclosure
 * to third parties without express authority is forbidden.
 * Copyright � Outbreak, Sweden, 2003, 2004.
 *
 */
#ifndef __INCLUDED_KARMA_PROGRAM_H
#define __INCLUDED_KARMA_PROGRAM_H

#include "adsr.h"
#include "lfo.h"

#ifdef __cplusplus
extern "C" {
#endif

typedef struct {
	int waveform1;
	int waveform2;

	int *waveform1Table;
	int *waveform2Table;

	int wavelen1;
	int wavelen2;

	float freq1;
	float freq2;
	int waveformMix;

	karma_ADSR modEnv;
	float modEnvAmount;

	karma_LFO lfo1;
	karma_LFO lfo2;


	int filter;
	float resonance;
	float cut;
	float adsrAmount;
	karma_ADSR filterCut;

	int distortion;

	karma_ADSR amplifier;
	int gain;

	int echoDelay;
	int echoAmount;
} karma_Program;

void karma_Program_init(karma_Program *program);
void karma_Program_setParameter(karma_Program *program, long index, float value);
float karma_Program_getParameter(karma_Program *program, long index);


#ifdef __cplusplus
}
#endif

#endif

