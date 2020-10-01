#include "soundSource.h"


SoundSource::SoundSource()
{
	alGenSources(1, &sourceId);
	alSource3f(sourceId, AL_POSITION, 0.0f, 0.0f, 0.0f);
	alSourcef(sourceId, AL_GAIN, 1.0f);
	alSourcef(sourceId, AL_PITCH, 1.0f);
	alSourcef(sourceId, AL_ROLLOFF_FACTOR, 1.0f);
	alSourcef(sourceId, AL_REFERENCE_DISTANCE, 1.0f);
}

void SoundSource::setPositionfv(ALfloat position[3])
{
	alSourcefv(sourceId, AL_POSITION, position);
}

void SoundSource::setPosition3f(ALfloat x, ALfloat y, ALfloat z)
{
	alSource3f(sourceId, AL_POSITION, x, y, z);
}

void SoundSource::setVolume(ALfloat volume)
{
	alSourcef(sourceId, AL_GAIN, volume);
}

void SoundSource::setPitch(ALfloat pitch)
{
	alSourcef(sourceId, AL_PITCH, pitch);
}

void SoundSource::setSourcei(ALenum property, ALint value)
{
	alSourcei(sourceId, property, value);
}

void SoundSource::setSourceiv(ALenum property, ALint *values)
{
	alSourceiv(sourceId, property, values);
}

void SoundSource::setSourcef(ALenum property, ALfloat value)
{
	alSourcef(sourceId, property, value);
}

void SoundSource::setSource3f(ALenum property, ALfloat value1, ALfloat value2, ALfloat value3)
{
	alSource3f(sourceId, property, value1, value2, value3);
}

void SoundSource::setSourcefv(ALenum property, ALfloat* values)
{
	alSourcefv(sourceId, property, values);
}

void SoundSource::getSourcei(ALenum property, ALint *value)
{
	alGetSourcei(sourceId, property, value);
}

void SoundSource::getSourceiv(ALenum property, ALint *values)
{
	alGetSourceiv(sourceId, property, values);
}

void SoundSource::getSourcef(ALenum property, ALfloat *value)
{
	alGetSourcef(sourceId, property, value);
}

void SoundSource::getSource3f(ALenum property, ALfloat *value1, ALfloat *value2, ALfloat *value3)
{
	alGetSource3f(sourceId, property, value1, value2, value3);
}

void SoundSource::getSourcefv(ALenum property, ALfloat *values)
{
	alGetSourcefv(sourceId, property, values);
}

void SoundSource::play(ALuint buffer)
{
	stop();
	alSourcei(sourceId, AL_BUFFER, buffer);
	alSourcePlay(sourceId);
	++count;
}

void SoundSource::pause()
{
	if (isPlaying())
	{
		alSourcePause(sourceId);
	}
}

void SoundSource::resume()
{
	if (isPaused())
	{
		alSourcePlay(sourceId);
	}
}

void SoundSource::stop()
{
	if (!isStoped())
	{
		alSourceStop(sourceId);
	}
}

void SoundSource::loop(ALboolean loop)
{
	loopingEnabled = loop;
	alSourcei(sourceId, AL_LOOPING, loop);
}

ALfloat SoundSource::getPlayPosition()
{
	ALfloat position = 0.0f;
	alGetSourcef(sourceId, AL_SEC_OFFSET, &position);

	return position;
}

ALboolean SoundSource::isPlaying()
{
	ALint playing = 0;
	alGetSourcei(sourceId, AL_SOURCE_STATE, &playing);

	if (playing == AL_PLAYING)
	{
		return AL_TRUE;
	}

	return AL_FALSE;
}

ALboolean SoundSource::isPaused()
{
	ALint paused = 0;
	alGetSourcei(sourceId, AL_SOURCE_STATE, &paused);

	if (paused == AL_PAUSED)
	{
		return AL_TRUE;
	}

	return AL_FALSE;
}

ALboolean SoundSource::isStoped()
{
	ALint stoped = 0;
	alGetSourcei(sourceId, AL_SOURCE_STATE, &stoped);

	if (stoped == AL_STOPPED)
	{
		return AL_TRUE;
	}

	return AL_FALSE;
}

ALboolean SoundSource::isLooping()
{
	return loopingEnabled;
}

unsigned int SoundSource::playCount()
{
	return count;
}

SoundSource::~SoundSource()
{
	stop();
	alSourcei(sourceId, AL_BUFFER, 0);
	alDeleteSources(1, &sourceId);
	//logInfo("Sound source %d destroyed.\n", sourceId);
	sourceId = 0;
}
