#include "LogSystem/LogSystem.h"
#include "Render/Renderer.h"
#include "GlobalVariables.h"

#ifdef __GEAR_VR
#include <android/log.h>
#endif
#include <stdarg.h>

using namespace WhiteBox;

extern "C"
{
	void WbLog( const char* channel, const char* format, ... )
	{
		char message[1024];

		va_list arglist;
		va_start(arglist, format);
#if defined(_WIN32) || defined(_WIN64)
		vsprintf_s(message, 1024, format, arglist);
#else
		vsprintf(message, format, arglist);
#endif
		va_end(arglist);

		gVars->pLogSystem->Log( channel, message  );
	}
}

WHITEBOX_BEGIN

class CConsoleLogger : public ILogger
{
	virtual void	Log( const String& message ) override
	{
#ifdef __GEAR_VR
		__android_log_print( ANDROID_LOG_ERROR, "WhiteBox", message.c_str(), 0 );
#else
		fwrite( message.c_str(), sizeof(char), message.size(), stdout );
#endif
	}
};


CLogSystem::CLogSystem()
{
	m_loggers.push_back( new CConsoleLogger ); 
}

CLogSystem::~CLogSystem()
{
	for( ILogger* pLogger : m_loggers )
	{
		delete pLogger;
	}
}

void	CLogSystem::Log( const char* channel, const char* message )
{
	String formatMessage = String(channel) + " : " + String(message);
	for( ILogger* pLogger : m_loggers )
	{
		pLogger->Log( formatMessage );
	}
}

WHITEBOX_END
