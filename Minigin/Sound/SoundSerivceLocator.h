#ifndef _SOUNDSERVICE_LOCATOR_H_
#define _SOUNDSERVICE_LOCATOR_H_

#include <memory>
#include "SoundSystem.h"

class SoundServiceLocator final
{
	static std::unique_ptr<SoundSystem> m_service;
public:
	static SoundSystem& GetSoundSystem() { return *m_service; }
	static void RegisterSoundSystem(std::unique_ptr<SoundSystem> service) { m_service = std::move(service); }
};

#endif // !
