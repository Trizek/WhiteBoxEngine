#ifndef __OPERATINGSYSTEM_H__
#define __OPERATINGSYSTEM_H__

#include "BaseTypes.h"

WHITEBOX_BEGIN

#include <Windows.h>

class CTimer
{
public:
	void Start();
	void Stop();

	float	GetDuration() const;

private:
	LARGE_INTEGER	m_frequency;
	LARGE_INTEGER	m_startTime;
	LARGE_INTEGER	m_stopTime;
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