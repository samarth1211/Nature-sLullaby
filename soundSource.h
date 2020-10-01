#pragma once

#include <al.h>
#include <alc.h>

class SoundSource
{
public:
	SoundSource();
	~SoundSource();

	void setPosition3f(ALfloat x, ALfloat y, ALfloat z);
	void setPositionfv(ALfloat position[3]);
	void setVolume(ALfloat volume);
	void setPitch(ALfloat pitch);
	void setSourcei(ALenum property, ALint value);
	void setSourceiv(ALenum property, ALint *values);
	void setSourcef(ALenum property, ALfloat value);
	void setSource3f(ALenum property, ALfloat value1, ALfloat value2, ALfloat value3);
	void setSourcefv(ALenum property, ALfloat *values);
	void getSourcei(ALenum property, ALint *value);
	void getSourceiv(ALenum property, ALint *values);
	void getSourcef(ALenum property, ALfloat *value);
	void getSource3f(ALenum property, ALfloat *value1, ALfloat *value2, ALfloat *value3);
	void getSourcefv(ALenum property, ALfloat *values);
	void play(ALuint buffer);
	void pause();
	void resume();
	void stop();
	void loop(ALboolean loop);
	unsigned int playCount();
	ALfloat getPlayPosition();

	ALboolean isPlaying();
	ALboolean isPaused();
	ALboolean isStoped();
	ALboolean isLooping();

private:
	ALuint sourceId = 0;
	ALboolean loopingEnabled = AL_FALSE;
	unsigned int count = 0;
};
