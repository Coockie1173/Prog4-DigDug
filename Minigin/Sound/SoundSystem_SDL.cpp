#include "SoundSystem_SDL.h"

#include <SDL3_mixer/SDL_mixer.h>
#include <stdexcept>
#include <thread>

void sound_system_SDL::MixerDeleter::operator()(MIX_Mixer* mixer) const noexcept
{
    if (mixer)
    {
        MIX_DestroyMixer(mixer);
    }
}

void sound_system_SDL::AudioDeleter::operator()(MIX_Audio* audio) const noexcept
{
    if (audio)
    {
        MIX_DestroyAudio(audio);
    }
}

sound_system_SDL::sound_system_SDL()
{
    m_mixer.reset(MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr));
    if (!m_mixer)
    {
        throw std::runtime_error(std::string{"Failed to create SDL mixer device: "} + SDL_GetError());
    }

    m_thread = std::jthread([this](std::stop_token st)
    {
        Worker(st);
    });
}

sound_system_SDL::~sound_system_SDL()
{
    m_thread.request_stop();
    m_cv.notify_all();
    m_thread = std::jthread{};
}

void sound_system_SDL::PlaySound(const std::string& SoundName)
{
    {
        std::lock_guard lock(m_mutex);
        m_soundQueue.emplace_back(SoundName);
    }
    m_cv.notify_one();
}

void sound_system_SDL::Worker(std::stop_token st)
{
    std::unique_lock lock(m_mutex);

    while (true)
    {
        m_cv.wait(lock, [&] 
            {
                return !m_soundQueue.empty() || st.stop_requested();
            });

        if (st.stop_requested() && m_soundQueue.empty())
            break;

        std::vector<std::string> soundsToPlay;
        soundsToPlay.swap(m_soundQueue);

        lock.unlock();
        for (auto& sound : soundsToPlay)
        {
            auto audioIt = m_audioCache.find(sound);
            if (audioIt == m_audioCache.end())
            {
                auto audio = std::unique_ptr<MIX_Audio, AudioDeleter>{MIX_LoadAudio(m_mixer.get(), sound.c_str(), false)};
                if (audio)
                {
                    audioIt = m_audioCache.emplace(sound, std::move(audio)).first;
                }
                else
                {
                    SDL_Log("Failed to load sound '%s': %s", sound.c_str(), SDL_GetError());
                }
            }

            if (audioIt != m_audioCache.end() && audioIt->second)
            {
                MIX_PlayAudio(m_mixer.get(), audioIt->second.get());
            }
        }

        lock.lock();
    }
}
