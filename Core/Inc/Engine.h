#ifndef __ENGINE_H__
#define __ENGINE_H__

#include "BaseTypes.h"
#include "System/OperatingSystem.h"
#include "SceneNode.h"
#include "Physics/PhysicsSystem.h"

#include "Scene.h"
#include "Render/Camera.h"

WHITEBOX_BEGIN

struct SColliderProxy
{
	CSceneNode*			pNode;
	TRigidBodyHandle	pRigidBodyHandle;
};

class CEngine
{
public:
	CEngine();

	void	InitEngine( uint width, uint height );
	void	Resize( uint width, uint height );

	void	FrameUpdate();

	

	size_t	AddColliderProxy( CSceneNode* pNode, TRigidBodyHandle	pRigidBodyHandle );
	void	RemoveColliderProxy( size_t index );
	void	UpdateColliderProxies();

	CCameraPtr				pMainCamera;
	class CRenderPipeline*	m_pRenderPipeline;

private:
	float	m_camYaw = 0.0f;
	float	m_camPitch = 0.0f;

	Vec2	m_prevMousePos;
	bool	m_bMovingCamera = false;


	std::vector< SColliderProxy >	m_colliderProxies;

	//CScenePtr	m_curScene;
};

WHITEBOX_END

#endif
