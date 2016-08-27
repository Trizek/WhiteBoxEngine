#ifndef __PHYSICSSYSTEM_H__
#define __PHYSICSSYSTEM_H__

#include "BaseTypes.h"

WHITEBOX_BEGIN

typedef void* TColliderHandle;
typedef void* TRigidBodyHandle;


class  IPhysicsSystem
{
public:
	virtual void					UpdateSimulation( float frameTime, float fixedTimeStep = 1.0f/60.0f ) = 0;

	virtual TColliderHandle			CreateSphereCollider( float radius ) = 0;
	virtual TColliderHandle			CreateBoxCollider( const Vec3& size ) = 0;
	virtual TColliderHandle			CreateStaticPlaneCollider( const Vec3& point, const Vec3& normal ) = 0;
	virtual TColliderHandle			CreateCapsuleCollider( float radius, float height ) = 0;
	virtual TColliderHandle			CreateStaticMeshCollider( class CTriMesh* pTriMesh, const Transform& transform ) = 0;

	virtual TRigidBodyHandle		CreateRigidyBody( TColliderHandle collider, const Transform& transform, float mass = 0 ) = 0;
	virtual void					AddRigidyBody( TRigidBodyHandle rigidBody ) = 0;
	virtual void					RemoveRigidyBody( TRigidBodyHandle rigidBody ) = 0;
	virtual void					GetRigidBodyTransform( TRigidBodyHandle rigidBody, Transform& transform ) = 0;
	virtual void					AddImpulse( TRigidBodyHandle rigidBody, const Vec3& impulse, const Vec3& localPoint = Vec3::Zero ) = 0;
	virtual void					AddForce( TRigidBodyHandle rigidBody, const Vec3& force, const Vec3& localPoint = Vec3::Zero ) = 0;

	virtual void					SetDebugDraw( bool m_bDebug ) = 0;

	virtual ~IPhysicsSystem() {}
};


WHITEBOX_END

#endif
