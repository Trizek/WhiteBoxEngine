#ifndef __OPERATINGSYSTEM_H__
#define __OPERATINGSYSTEM_H__

#include "BaseTypes.h"

#include "DataStream.h"

WHITEBOX_BEGIN

#if defined(_WIN32) || defined(_WIN64)
#include <Windows.h>
#else
#include <time.h>
#endif

enum class EShaderPlatform
{
	eDesktop = 0,
	eGearVR,
};


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

	bool	PollTouchEvent( STouchEvent& event );


	// return frame time since last tick
	float	Tick();

	void	GetDataStream( const String& path, CDataStream& dataStream, size_t size = 0 );

	EShaderPlatform	GetShaderPlatform() const;

private:
#ifndef __GEAR_VR
	void*	m_pSpecificData;
	CTimer	m_frameTimer;
#else
public:
	Matrix44*	GetInverseEyeMatrices();
	const char*	GetPackagePath() const;

private:
	Matrix44	m_inverseEyeTransforms[ 2 ];
#endif
};

WHITEBOX_END

#endif
