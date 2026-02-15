#ifndef _TIMING_H_
#define _TIMING_H_

#include "Singleton.h"

constexpr float MAXFPS = 144.0f;
constexpr float FIXEDTIMESTEP = 1.0f / MAXFPS; //find better value later
constexpr float TARGET_FRAME_TIME = 1.0f / MAXFPS;

class Timing : public dae::Singleton<Timing>
{
public:
	void SetTimings(float DeltaTime); //leave room for later expansion
	float GetDeltaTime() { return m_deltaTime; };
private:
	float m_deltaTime{};
};

#endif