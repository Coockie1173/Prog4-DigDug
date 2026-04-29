#ifndef _SOUNDSYSTEM_SDL_H_
#define _SOUNDSYSTEM_SDL_H_

#include "SoundSystem.h"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>

struct MIX_Mixer;
class sound_system_SDL final : public SoundSystem
{
public:
	sound_system_SDL();
	~sound_system_SDL() override;
	void PlaySound(std::string SoundName) override;
private:
	std::jthread m_thread;
	std::mutex m_mutex;
	std::condition_variable m_cv;
	void Worker(std::stop_token st);
	std::vector<std::string> m_soundQueue;
	MIX_Mixer* m_mixer;
};

#endif