#ifndef __COLLIDERNODE_H__
#define __COLLIDERNODE_H__

#include "SceneNode.h"
#include "Physics/PhysicsSystem.h"
#include "Render/Mesh.h"

WHITEBOX_BEGIN


class  CColliderNode : public CSpatialNode
{
public:	
	virtual void Refresh() override;

	void	SetMass( float mass );
	float	GetMass() const;

protected:
	virtual TColliderHandle		CreateCollider() = 0;

	TColliderHandle		m_colliderHandle;
	TRigidBodyHandle	m_rigidBodyHandle;

	float	m_mass;
};

class  CSphereColliderNode : public CColliderNode
{
public:
	void	SetRadius( float radius );
	float	GetRadius() const;

protected:
	virtual TColliderHandle		CreateCollider() override;

	float	m_radius;
};

class  CStaticMeshColliderNode : public CColliderNode
{
public:
	void		SetMesh( CMeshPtr pMesh );
	CMeshPtr	GetMesh() const;

protected:
	virtual TColliderHandle		CreateCollider() override;

	CMeshPtr	m_pMesh;
};

WHITEBOX_END

#endif

