#ifndef _SOUNDSYSTEM_SDL_H_
#define _SOUNDSYSTEM_SDL_H_

#include "SoundSystem.h"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <vector>
#include <unordered_map>

struct MIX_Mixer;
struct MIX_Audio;
class sound_system_SDL final : public SoundSystem
{
public:
	sound_system_SDL();
	~sound_system_SDL() override;
	void PlaySound(const std::string& SoundName) override;
private:
	std::jthread m_thread;
	std::mutex m_mutex;
	std::condition_variable m_cv;
	void Worker(std::stop_token st);
	std::vector<std::string> m_soundQueue;
	// cache loaded audio objects so the worker doesn't reload the same file repeatedly
	std::unordered_map<std::string, MIX_Audio*> m_audioCache;
	MIX_Mixer* m_mixer{nullptr};
};

#endif