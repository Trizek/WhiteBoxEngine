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

	void	AddImpulse( const Vec3& impulse, const Vec3& localPoint = Vec3::Zero );
	void	AddForce( const Vec3& force, const Vec3& localPoint = Vec3::Zero );

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

DEFINE_SMART_PTR(CSphereColliderNode)

class  CBoxColliderNode : public CColliderNode
{
public:
	void	SetSize( const Vec3& size );
	Vec3	GetSize() const;

protected:
	virtual TColliderHandle		CreateCollider() override;

	Vec3	m_size;
};

DEFINE_SMART_PTR(CBoxColliderNode)

class  CStaticMeshColliderNode : public CColliderNode
{
public:
	void		SetMesh( CMeshPtr pMesh );
	CMeshPtr	GetMesh() const;

protected:
	virtual TColliderHandle		CreateCollider() override;

	CMeshPtr	m_pMesh;
};

DEFINE_SMART_PTR(CStaticMeshColliderNode)

WHITEBOX_END

#endif

