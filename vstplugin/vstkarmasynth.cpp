/*
 * Vst plugin for the Karma softsynth
 * 
 * $Id: vstkarmasynth.cpp,v 1.2 2004/01/01 16:06:20 Fredrik Ehnbom Exp $
 * Author : Fredrik Ehnbom
 * 
 * All rights reserved. Reproduction, modification, use or disclosure
 * to third parties without express authority is forbidden.
 * Copyright � Outbreak, Sweden, 2003, 2004.
 *
 */
#include <math.h>
#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include "vstkarma.h"
#include "../karmalib/param.h"

//const double midiScaler = (1. / 127.);
//static float fScaler = kWaveSize / 44100.0f;

//-----------------------------------------------------------------------------------------
//-----------------------------------------------------------------------------------------
void VstKarma::setSampleRate (float sampleRate)
{
	// TODO: check
	AudioEffectX::setSampleRate(sampleRate);
//	fScaler = (float)((double)kWaveSize / (double)sampleRate);
}

//-----------------------------------------------------------------------------------------
void VstKarma::setBlockSize (long blockSize)
{
	AudioEffectX::setBlockSize (blockSize);
	// you may need to have to do something here...
}

//-----------------------------------------------------------------------------------------
void VstKarma::resume ()
{
	wantEvents ();
}

//-----------------------------------------------------------------------------------------
void VstKarma::initProcess ()
{
//	fScaler = (float)((double)kWaveSize / 44100.);	// we don't know the sample rate yet
}

#ifdef __GNUC__
static int bufferL[BUFFERSIZE] __attribute__((aligned(32)));
static int bufferR[BUFFERSIZE] __attribute__((aligned(32)));
#else
static __declspec(align(32)) int bufferL[BUFFERSIZE];
static __declspec(align(32)) int bufferR[BUFFERSIZE];
#endif

//-----------------------------------------------------------------------------------------
void VstKarma::process (float **inputs, float **outputs, long sampleFrames)
{
	if (sampleFrames <= BUFFERSIZE) {
		memset(&bufferL, 0, BUFFERSIZE * sizeof(int));
		memset(&bufferR, 0, BUFFERSIZE * sizeof(int));
		for (int i = 0; i < 16; i++) {
			karma_Channel_process(&channel[i], bufferL, bufferR, sampleFrames);
		}
		for (int i = 0; i < sampleFrames; i++) {
			float samplel = bufferL[i] / 32767.0f;
			float sampler = bufferR[i] / 32767.0f;
			samplel = samplel > 1.0f ? 1.0f : samplel < -1.0f ? -1.0f : samplel;
			sampler = sampler > 1.0f ? 1.0f : sampler < -1.0f ? -1.0f : sampler;
			outputs[0][i] += samplel;
			outputs[1][i] += sampler;
		}
	}
}


//-----------------------------------------------------------------------------------------
void VstKarma::processReplacing (float **inputs, float **outputs, long sampleFrames)
{

	if (sampleFrames <= BUFFERSIZE) {
		memset(&bufferL, 0, BUFFERSIZE * sizeof(int));
		memset(&bufferR, 0, BUFFERSIZE * sizeof(int));
		for (int i = 0; i < 16; i++) {
			karma_Channel_process(&channel[i], bufferL, bufferR, sampleFrames);
		}

		for (int i = 0; i < sampleFrames; i++) {
			float samplel = bufferL[i] / 32767.0f;
			float sampler = bufferR[i] / 32767.0f;
			samplel = samplel > 1.0f ? 1.0f : samplel < -1.0f ? -1.0f : samplel;
			sampler = sampler > 1.0f ? 1.0f : sampler < -1.0f ? -1.0f : sampler;
			outputs[0][i] = samplel;
			outputs[1][i] = sampler;
		}
	}
}

//-----------------------------------------------------------------------------------------
long VstKarma::processEvents (VstEvents* ev)
{
	for (long i = 0; i < ev->numEvents; i++)
	{
		if ((ev->events[i])->type != kVstMidiType)
			continue;
		VstMidiEvent* event = (VstMidiEvent*)ev->events[i];
		char* midiData = event->midiData;
		long cmd = midiData[0] & 0xf0;		// extract command
		long chn = midiData[0] & 0x0f;		// extract channel

		if (cmd == 0xb0 && (midiData[1] == 120 || midiData[1] >= 123)) {
			for (int i = 0; i < 16; i++)
				karma_Channel_allNotesOff(&channel[i]);
		} else {
			karma_MidiEvent *kevent = (karma_MidiEvent*) malloc(sizeof(karma_MidiEvent));
			memset(kevent, 0, sizeof(karma_MidiEvent));
			kevent->data[0] = midiData[0];
			kevent->data[1] = midiData[1];
			kevent->data[2] = midiData[2];
			kevent->time = event->deltaFrames;

			karma_Channel_addEvent(&channel[chn], kevent);
		}

		event++;
	}
	return 1;	// want more
}
