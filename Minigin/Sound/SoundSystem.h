#ifndef _SOUNDSYSTEM_H_
#define _SOUNDSYSTEM_H_

#include <string>

class SoundSystem
{
public:
	virtual ~SoundSystem() = default;
	virtual void PlaySound(const std::string& sound_name) = 0;
};

#endif