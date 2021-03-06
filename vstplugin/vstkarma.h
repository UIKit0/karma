/*
 * Vst plugin for the Karma softsynth
 * 
 * $Id: vstkarma.h,v 1.2 2004/01/01 16:06:19 Fredrik Ehnbom Exp $
 * Author : Fredrik Ehnbom
 * 
 * All rights reserved. Reproduction, modification, use or disclosure
 * to third parties without express authority is forbidden.
 * Copyright � Outbreak, Sweden, 2003, 2004.
 *
 */
#ifndef __INCLUDED_KARMA_VSTKARMA_H
#define __INCLUDED_KARMA_VSTKARMA_H

#include <string.h>
#include "D:\CODE\vst\vstsdk2.3\vstsdk2.3\source\common\audioeffectx.h"

#include "../karmalib/Program.h"
#include "../karmalib/Channel.h"
#include "../karmalib/param.h"
#include "../karmalib/waveform.h"

enum
{
	kNumPrograms = 16,
	kNumOutputs = 2,

	kTest = kParamEnd,

	kNumParams
};



//------------------------------------------------------------------------------------------
class VstKarma : public AudioEffectX
{
public:
	VstKarma(audioMasterCallback audioMaster);
	~VstKarma();

	static void Debug(char *blah);

	virtual void process(float **inputs, float **outputs, long sampleframes);
	virtual void processReplacing(float **inputs, float **outputs, long sampleframes);
	virtual long processEvents(VstEvents* events);

	virtual void setProgram(long program);
	virtual long getProgram();
	virtual void setProgramName(char *name);
	virtual void getProgramName(char *name);
	virtual void setParameter(long index, float value);
	virtual float getParameter(long index);
	virtual void getParameterLabel(long index, char *label);
	virtual void getParameterDisplay(long index, char *text);
	virtual void getParameterName(long index, char *text);
	virtual void setSampleRate(float sampleRate);
	virtual void setBlockSize(long blockSize);
	virtual void resume();

	virtual bool getOutputProperties (long index, VstPinProperties* properties);
	virtual bool getProgramNameIndexed (long category, long index, char* text);
	virtual bool copyProgram (long destination);
	virtual bool getEffectName (char* name);
	virtual bool getVendorString (char* text);
	virtual bool getProductString (char* text);
	virtual long getVendorVersion () {return 1;}
	virtual long canDo (char* text);

private:
	void initProcess();

	char names[16][30];
	karma_Channel channel[16];
//	Program* programs;
	karma_Program* currentProgram;
	long	currentProgramIndex;
};


#endif
