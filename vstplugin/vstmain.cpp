/*
 * Vst plugin for the Karma softsynth
 * 
 * $Id: vstmain.cpp,v 1.2 2004/01/01 16:06:20 Fredrik Ehnbom Exp $
 * Author : Fredrik Ehnbom
 * 
 * All rights reserved. Reproduction, modification, use or disclosure
 * to third parties without express authority is forbidden.
 * Copyright � Outbreak, Sweden, 2003, 2004.
 *
 */
#include <stddef.h>
#include "vstkarma.h"

static AudioEffect *effect = 0;
bool oome = false;

#if MAC
#pragma export on
#endif

//------------------------------------------------------------------------
// prototype of the export function main
#if BEOS
#define main main_plugin
extern "C" __declspec(dllexport) AEffect *main_plugin (audioMasterCallback audioMaster);

#else
extern "C" __declspec(dllexport) AEffect *main (audioMasterCallback audioMaster);
#endif

extern "C" {
AEffect *main (audioMasterCallback audioMaster)
{
	// get vst version
	if(!audioMaster (0, audioMasterVersion, 0, 0, 0, 0))
		return 0;  // old version

	effect = new VstKarma (audioMaster);
	if (!effect)
		return 0;
	if (oome)
	{
		delete effect;
		return 0;
	}
	return effect->getAeffect ();
}
}

#if MAC
#pragma export off
#endif

#if WIN32
#include <windows.h>
void* hInstance;
BOOL WINAPI DllMain (HINSTANCE hInst, DWORD dwReason, LPVOID lpvReserved)
{
	hInstance = hInst;
	return 1;
}
#endif
