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

void CCamera::ComputeInverseTransformMatrix()
{
	Transform inverseTransform = transform.GetInverse();
	inverseTransformMatrix.FromTransform( inverseTransform );
}

Vec3 CCamera::GetNearPlaneHalfSize() const
{
	Vec3 size;

	float width = float(pRenderTarget->GetWidth());
	float height = float(pRenderTarget->GetHeight());

	Degree fov(45.0f);
	size.z = Tan(fov * 0.5f) * 10.0f;
	size.x = size.z * (width / height);
	size.y = 10.0f;

	return size;
}

Vec3 CCamera::GetWorldMousePos( const Vec2& mousePos ) const
{
	Vec3 nearPlaneHalfSize = GetNearPlaneHalfSize();

	float width = float(pRenderTarget->GetWidth());
	float height = float(pRenderTarget->GetHeight());

	Vec2 normalizedMousePos( (mousePos.x / width) * 2.0f - 1.0f, 1.0f - (mousePos.y / height) * 2.0f );

	Vec3 localMousePos( nearPlaneHalfSize.x * normalizedMousePos.x, nearPlaneHalfSize.y, nearPlaneHalfSize.z * normalizedMousePos.y );
	return transform * localMousePos;
}

WHITEBOX_END
