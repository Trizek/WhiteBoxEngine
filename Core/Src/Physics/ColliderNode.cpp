#include "Physics/ColliderNode.h"
#include "GlobalVariables.h"
#include "Application.h"

WHITEBOX_BEGIN

void CColliderNode::Refresh()
{
	m_colliderHandle = CreateCollider();

	Transform globalTransform;
	GetGlobalTransform( globalTransform );
	globalTransform.scale = Vec3( 1.0f );
	m_rigidBodyHandle = gVars->pPhysicsSystem->CreateRigidyBody( m_colliderHandle, globalTransform, m_mass );
	gVars->pPhysicsSystem->AddRigidyBody( m_rigidBodyHandle );

	gVars->pApplication->AddColliderProxy( this, m_rigidBodyHandle );

	CSceneNode::Refresh();
}

void	CColliderNode::SetMass( float mass )
{
	m_mass = mass;
}

float	CColliderNode::GetMass() const
{
	return m_mass;
}



void	CSphereColliderNode::SetRadius( float radius )
{
	m_radius = radius;
}

float	CSphereColliderNode::GetRadius() const
{
	return m_radius;
}


TColliderHandle		CSphereColliderNode::CreateCollider()
{
	Transform globalTransform;
	GetGlobalTransform( globalTransform );

	return gVars->pPhysicsSystem->CreateSphereCollider( globalTransform.scale.x * m_radius );
}



void	CStaticMeshColliderNode::SetMesh( CMeshPtr pMesh )
{
	m_pMesh = pMesh;
}

CMeshPtr	CStaticMeshColliderNode::GetMesh() const
{
	return m_pMesh;
}


TColliderHandle		CStaticMeshColliderNode::CreateCollider()
{
	Transform transform;
	GetGlobalTransform( transform );
	transform.position = Vec3::Zero;
	transform.rotation = Quat();

	return gVars->pPhysicsSystem->CreateStaticMeshCollider( m_pMesh->GetTriMesh().get(), transform );
}


WHITEBOX_END