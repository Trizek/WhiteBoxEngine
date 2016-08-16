#ifndef __LOGSYSTEM_H__
#define __LOGSYSTEM_H__


#include "BaseTypes.h"

extern "C" void WbLog( const char* channel, const char* format, ... );

WHITEBOX_BEGIN

class ILogger
{
public:
	virtual void	Log( const String& message ) = 0;

	virtual ~ILogger(){}
};

class CLogSystem
{
public:
	CLogSystem();
	~CLogSystem();


	void Log( const char* channel, const char* message );

private:
	std::vector< ILogger* >		m_loggers;
};

WHITEBOX_END

#endif


 
