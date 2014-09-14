#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include "RenderAPI.h"
#include "PhysicsAPI.h"

WHITEBOX_BEGIN

class CApplication
{
public:
	void	Run();
	void	Update( float frameTime );
	
	IRenderApplicationPtr	GetRenderApplication();
	IPhysicsWorldPtr		GetPhysicsWorld();
	
protected:
	IRenderApplicationPtr	m_pRenderApplication;
	IPhysicsWorldPtr		m_pPhysicsWorld;
};

WHITEBOX_END

#endif
