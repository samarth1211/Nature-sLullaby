#pragma once

#include <windows.h>
#include <stdlib.h>
#include <stdio.h>
#include <al.h>
#include <alc.h>

#include "CWaves.h"

class AudioManager
{
public:
	AudioManager();
	~AudioManager();

	ALboolean initialize();
	ALboolean loadWaveAudio(const char* filePath, ALuint bufferId);

	void setListenerPosition(float x, float y, float z);
	void setListenerVelocity(float x, float y, float z);
	ALfloat getBufferLength(ALuint buffer);

private:
	CWaves *waveLoader = NULL;
	ALCdevice *device = NULL;
	ALCcontext *context = NULL;
	FILE *pAudioLogFile = NULL;

	void cleanUp();
};
