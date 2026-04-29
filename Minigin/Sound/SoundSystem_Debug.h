#ifndef _SOUNDSYSTEM_DEBUG_H_
#define _SOUNDSYSTEM_DEBUG_H_

#include "SoundSystem.h"
#include <memory>
#include <Debugger.h>

class SoundSystem_Debug final : public SoundSystem
{
	std::unique_ptr<SoundSystem> m_realSoundSystem;
public:
	SoundSystem_Debug(std::unique_ptr<SoundSystem> realSoundSystem) : m_realSoundSystem(std::move(realSoundSystem)) {}
	virtual ~SoundSystem_Debug() = default;

	void PlaySound(std::string sound_name) override
	{
		Debugger::GetInstance().LogDebug("Playing sound: " + sound_name);
		m_realSoundSystem->PlaySound(std::move(sound_name));
	}
};

#endif