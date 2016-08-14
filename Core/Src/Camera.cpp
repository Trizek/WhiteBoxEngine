#include "Render/Camera.h"
#include "Render/Renderer.h"
#include "GlobalVariables.h"

WHITEBOX_BEGIN

void CCamera::ComputeProjectionMatrix()
{
	Degree fov(45.0f);
	float w = 1.0f / Tan(fov * 0.5f);
	float h = (w * float(pRenderTarget->GetHeight())) / float(pRenderTarget->GetWidth());

	gVars->pRenderer->ComputeProjectionMatrix( 10.0f, 10000.0f, h, w, projectionMatrix );
}

void CCamera::ComputeTransformMatrix()
{
	Transform inverseTransform = transform.GetInverse();
	inverseTransformMatrix.FromTransform( inverseTransform );
}

WHITEBOX_END
