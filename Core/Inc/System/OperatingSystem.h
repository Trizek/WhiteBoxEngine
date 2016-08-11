#ifndef __OPERATINGSYSTEM_H__
#define __OPERATINGSYSTEM_H__

#include "BaseTypes.h"

WHITEBOX_BEGIN

#if defined(WIN32) || defined(WIN64)
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
#if defined(WIN32) || defined(WIN64)
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

private:
	void*	m_pSpecificData;
};

WHITEBOX_END

#endif
