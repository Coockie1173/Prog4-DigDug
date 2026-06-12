#ifndef _SOUNDSYSTEM_SDL_H_
#define _SOUNDSYSTEM_SDL_H_

#include "SoundSystem.h"
#include <thread>
#include <mutex>
#include <condition_variable>
#include <memory>
#include <vector>
#include <unordered_map>
#include <atomic>

struct MIX_Mixer;
struct MIX_Audio;
class sound_system_SDL final : public SoundSystem
{
public:
	sound_system_SDL();
	~sound_system_SDL() override;
	void PlaySound(const std::string& SoundName) override;
	void ToggleMuted() override { m_muted = !m_muted; }
	bool IsMuted() const override { return m_muted; }
private:
	struct MixerDeleter
	{
		void operator()(MIX_Mixer* mixer) const noexcept;
	};

	struct AudioDeleter
	{
		void operator()(MIX_Audio* audio) const noexcept;
	};

	std::atomic<bool> m_muted{ false };
	std::mutex m_mutex;
	std::condition_variable m_cv;
	void Worker(std::stop_token st);
	std::vector<std::string> m_soundQueue;
	std::unordered_map<std::string, std::unique_ptr<MIX_Audio, AudioDeleter>> m_audioCache;
	std::unique_ptr<MIX_Mixer, MixerDeleter> m_mixer;
	std::jthread m_thread;
};

#endif