#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include "BaseTypes.h"
#include "System/OperatingSystem.h"

WHITEBOX_BEGIN

class CApplication
{
public:
	CApplication();

	void	InitApplication( uint width, uint height );
	void	Resize( uint width, uint height );

	void	FrameUpdate();

	class CRenderPipeline*	m_pRenderPipeline;
};

WHITEBOX_END

#endif
