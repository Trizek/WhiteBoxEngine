#ifndef __CAMERA_H__
#define __CAMERA_H__

#include "Render/RenderTarget.h"

WHITEBOX_BEGIN

class CCamera
{
public:
	void				ComputeProjectionMatrix();
	void				ComputeInverseTransformMatrix();

	Vec3				GetNearPlaneHalfSize() const;
	Vec3				GetWorldMousePos( const Vec2& mousePos ) const;

	Transform			transform;

	IRenderTargetPtr	pRenderTarget;
	Matrix44			projectionMatrix;
	Matrix34			inverseTransformMatrix;
};

WHITEBOX_END

#endif