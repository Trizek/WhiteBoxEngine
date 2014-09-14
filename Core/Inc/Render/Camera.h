#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "Render/RenderTarget.h"

WHITEBOX_BEGIN

class CCamera
{
public:
	

private:

	IRenderTargetPtr	m_pRenderTarget;
	Matrix44			m_projectionMatrix;
};

WHITEBOX_END

#endif