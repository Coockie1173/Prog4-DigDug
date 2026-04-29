#include "SoundSystem_SDL.h"

#include <SDL3_mixer/SDL_mixer.h>
#include <thread>

sound_system_SDL::sound_system_SDL()
{
    m_mixer = MIX_CreateMixerDevice(SDL_AUDIO_DEVICE_DEFAULT_PLAYBACK, nullptr);
    m_thread = std::jthread([this](std::stop_token st)
    {
        Worker(st);
    });
}

sound_system_SDL::~sound_system_SDL()
{
    m_thread.request_stop();
    m_cv.notify_all();
}

void sound_system_SDL::PlaySound(std::string SoundName)
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
            MIX_Audio* audio = MIX_LoadAudio(m_mixer, sound.c_str(), true);
            if (audio)
            {
                MIX_PlayAudio(m_mixer, audio);
            }
        }

        lock.lock();
    }
}
