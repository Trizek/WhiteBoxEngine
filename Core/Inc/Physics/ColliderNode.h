#ifndef __COLLIDERNODE_H__
#define __COLLIDERNODE_H__

#include "SceneNode.h"
#include "Physics/PhysicsSystem.h"
#include "Render/Mesh.h"

WHITEBOX_BEGIN


class  CColliderNode : public CSpatialNode
{
public:	
	CColliderNode() = default;
	CColliderNode( const String& _name ) : CSpatialNode(_name) {}


	virtual void Refresh() override;

	void	SetMass( float mass );
	float	GetMass() const;

	void	AddImpulse( const Vec3& impulse, const Vec3& localPoint = Vec3::Zero );
	void	AddForce( const Vec3& force, const Vec3& localPoint = Vec3::Zero );

	virtual void	Serialize( ISerializer& serializer ) override;

protected:
	virtual TColliderHandle		CreateCollider() = 0;

public:
	TColliderHandle		m_colliderHandle;
	TRigidBodyHandle	m_rigidBodyHandle;

	float	m_mass;
};

class  CSphereColliderNode : public CColliderNode
{
DECLARE_SERIALIZABLE_CLASS(CSphereColliderNode)

public:
	CSphereColliderNode() = default;
	CSphereColliderNode( const String& _name ) : CColliderNode(_name) {}

	void	SetRadius( float radius );
	float	GetRadius() const;

	virtual void	Serialize( ISerializer& serializer ) override;

protected:
	virtual TColliderHandle		CreateCollider() override;

	float	m_radius;
};

DEFINE_SMART_PTR(CSphereColliderNode)

class  CBoxColliderNode : public CColliderNode
{
DECLARE_SERIALIZABLE_CLASS(CBoxColliderNode)

public:
	CBoxColliderNode() = default;
	CBoxColliderNode( const String& _name ) : CColliderNode(_name) {}

	void	SetSize( const Vec3& size );
	Vec3	GetSize() const;

	virtual void	Serialize( ISerializer& serializer ) override;

protected:
	virtual TColliderHandle		CreateCollider() override;

	Vec3	m_size;
};

DEFINE_SMART_PTR(CBoxColliderNode)

class  CStaticMeshColliderNode : public CColliderNode
{
DECLARE_SERIALIZABLE_CLASS(CStaticMeshColliderNode)

public:
	CStaticMeshColliderNode() = default;
	CStaticMeshColliderNode( const String& _name ) : CColliderNode(_name) {}

	void		SetMesh( CMeshPtr pMesh );
	CMeshPtr	GetMesh() const;

	virtual void	Serialize( ISerializer& serializer ) override;

protected:
	virtual TColliderHandle		CreateCollider() override;

	CMeshPtr	m_pMesh;
};

DEFINE_SMART_PTR(CStaticMeshColliderNode)

WHITEBOX_END

#endif

