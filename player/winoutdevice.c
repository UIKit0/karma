#include <windows.h>

#include "device.h"

#define REPLAY_NBSOUNDBUFFER  3

int m_currentBuffer = 0;
WAVEHDR m_waveHeader[REPLAY_NBSOUNDBUFFER];
HWAVEOUT m_hWaveOut;
bool closing = false;
short soundBuffer[BUFFERSIZE * 2 * REPLAY_NBSOUNDBUFFER];

static __declspec(align(32)) int bufferL[BUFFERSIZE];
static __declspec(align(32)) int bufferR[BUFFERSIZE];


// Internal WaveOut API callback function. We just call our sound handler ("playNextBuffer")
void	fillNextBuffer() {
	if (closing) return;
	// check if the buffer is already prepared (should not !)
	if (m_waveHeader[m_currentBuffer].dwFlags&WHDR_PREPARED)
		waveOutUnprepareHeader(m_hWaveOut,&m_waveHeader[m_currentBuffer],sizeof(WAVEHDR));

	memset(&bufferL, 0, BUFFERSIZE * sizeof(int));
	memset(&bufferR, 0, BUFFERSIZE * sizeof(int));

	karma_process(ksong, bufferL, bufferR, BUFFERSIZE);
	int pos = m_currentBuffer * BUFFERSIZE * 2;
	for (int i = 0; i < BUFFERSIZE; i++) {
		int samplel = bufferL[i];
		int sampler = bufferR[i];

		samplel = samplel < -32767 ? -32767 : samplel > 32767 ? 32767 : samplel;
		sampler = sampler < -32767 ? -32767 : sampler > 32767 ? 32767 : sampler;

		soundBuffer[pos++] = samplel;
		soundBuffer[pos++] = sampler;
	}

	// Prepare the buffer to be sent to the WaveOut API
	m_waveHeader[m_currentBuffer].lpData = (LPSTR) (&soundBuffer[2 * BUFFERSIZE * m_currentBuffer]);
	m_waveHeader[m_currentBuffer].dwBufferLength = BUFFERSIZE*2*2;
	waveOutPrepareHeader(m_hWaveOut,&m_waveHeader[m_currentBuffer],sizeof(WAVEHDR));

	// Send the buffer the the WaveOut queue
	waveOutWrite(m_hWaveOut,&m_waveHeader[m_currentBuffer],sizeof(WAVEHDR));

	m_currentBuffer++;
	if (m_currentBuffer >= REPLAY_NBSOUNDBUFFER) m_currentBuffer = 0;
}

static void CALLBACK waveOutProc(HWAVEOUT hwo,UINT uMsg,DWORD dwInstance,DWORD dwParam1,DWORD dwParam2)
{
		if (uMsg == WOM_DONE)
		{
			fillNextBuffer();
		}
}

int deviceOpen() {
	WAVEFORMATEX pcmWaveFormat;

	short chn = 2;
	int rate = 44100;
	short bitpersample = 16;

	pcmWaveFormat.wFormatTag = WAVE_FORMAT_PCM; 
	pcmWaveFormat.nChannels = chn; 
	pcmWaveFormat.nSamplesPerSec = rate;
	pcmWaveFormat.nAvgBytesPerSec = chn * rate * (bitpersample / 8);
	pcmWaveFormat.nBlockAlign = 4; 
	pcmWaveFormat.wBitsPerSample = bitpersample; 
	pcmWaveFormat.cbSize = sizeof(WAVEFORMATEX);


	int s = waveOutOpen(
		&m_hWaveOut,
		-1,
		&pcmWaveFormat,
		(DWORD_PTR)waveOutProc,
		0,
		CALLBACK_FUNCTION);

	m_currentBuffer = 0;
	for (int i = 0; i < REPLAY_NBSOUNDBUFFER; i++) {
		fillNextBuffer();
	}

	return 0;
}
void deviceClose() {
	closing = true;
	waveOutReset(m_hWaveOut);
	for (int i=0;i<REPLAY_NBSOUNDBUFFER;i++) {
		if ((m_waveHeader[i].dwFlags&WHDR_PREPARED) != 0)
			waveOutUnprepareHeader(m_hWaveOut, &m_waveHeader[i], sizeof(WAVEHDR));
	}

	waveOutClose(m_hWaveOut);
}

void deviceMainLoop() {
	// winout handles itself...
	getch();
}

