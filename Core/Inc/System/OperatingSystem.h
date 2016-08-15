#ifndef __OPERATINGSYSTEM_H__
#define __OPERATINGSYSTEM_H__

#include "BaseTypes.h"

WHITEBOX_BEGIN

#if defined(_WIN32) || defined(_WIN64)
#include <Windows.h>
#else
#include <time.h>
#endif


class CTimer
{
public:
	void Start();
	void Stop();

	float	GetDuration() const;

private:
#if defined(_WIN32) || defined(_WIN64)
	LARGE_INTEGER	m_frequency;
	LARGE_INTEGER	m_startTime;
	LARGE_INTEGER	m_stopTime;
#else
	struct timespec	m_startTime;
	struct timespec m_stopTime;
#endif
};

class COperatingSystem
{
public:
	COperatingSystem();
	~COperatingSystem();

	void	Init(uint width, uint height);
	void	Reshape(uint width, uint height);
	
	Vec2	GetMousePos();
	bool	GetMouseButton(int button);
	bool	IsPressingKey(Key key);
	bool	JustPressedKey(Key key);

	// return frame time since last tick
	float	Tick();

private:
#ifndef __GEAR_VR
	void*	m_pSpecificData;
	CTimer	m_frameTimer;
#endif
};

WHITEBOX_END

#endif
