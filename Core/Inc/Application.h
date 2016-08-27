#ifndef __APPLICATION_H__
#define __APPLICATION_H__

#include "BaseTypes.h"
#include "System/OperatingSystem.h"
#include "SceneNode.h"
#include "Physics/PhysicsSystem.h"

struct SBone
{
	WhiteBox::Transform t;
	std::vector<SBone> children;
};


WHITEBOX_BEGIN

struct SColliderProxy
{
	CSceneNode*			pNode;
	TRigidBodyHandle	pRigidBodyHandle;
};

class CApplication
{
public:
	CApplication();

	void	InitApplication( uint width, uint height );
	void	Resize( uint width, uint height );

	void	FrameUpdate();

	class CRenderPipeline*	m_pRenderPipeline;

	size_t	AddColliderProxy( CSceneNode* pNode, TRigidBodyHandle	pRigidBodyHandle );
	void	RemoveColliderProxy( size_t index );
	void	UpdateColliderProxies();

private:
	std::vector< SColliderProxy >	m_colliderProxies;
};

WHITEBOX_END

#endif
