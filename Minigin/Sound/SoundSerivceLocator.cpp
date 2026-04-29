#include "SoundSerivceLocator.h"
std::unique_ptr<SoundSystem> SoundServiceLocator::m_service = nullptr;