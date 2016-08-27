#ifndef __BULLETPHYSICSSYSTEM_H__
#define __BULLETPHYSICSSYSTEM_H__



#include "Physics/PhysicsSystem.h"


struct SBulletRigidyBody
{
	class btRigidBody*			pRigidBody;
	class btDefaultMotionState*	pMotionState;
};

using namespace WhiteBox;

class  CBulletPhysicsSystem : public IPhysicsSystem
{
public:
	CBulletPhysicsSystem();
	~CBulletPhysicsSystem();

	virtual void					UpdateSimulation( float frameTime, float fixedTimeStep = 1.0f/60.0f ) override;

	virtual TColliderHandle			CreateSphereCollider( float radius ) override;
	virtual TColliderHandle			CreateBoxCollider( const Vec3& size ) override;
	virtual TColliderHandle			CreateCapsuleCollider( float radius, float height ) override;
	virtual TColliderHandle			CreateStaticPlaneCollider( const Vec3& point, const Vec3& normal ) override;
	virtual TColliderHandle			CreateStaticMeshCollider( CTriMesh* pTriMesh, const Transform& transform ) override;
	
	virtual TRigidBodyHandle		CreateRigidyBody( TColliderHandle collider, const WhiteBox::Transform& transform, float mass = 0 ) override;
	virtual void					AddRigidyBody( TRigidBodyHandle rigidBody ) override;
	virtual void					RemoveRigidyBody( TRigidBodyHandle rigidBody ) override;
	virtual void					GetRigidBodyTransform( TRigidBodyHandle rigidBody, WhiteBox::Transform& transform ) override;
	virtual void					AddImpulse( TRigidBodyHandle rigidBody, const Vec3& impulse, const Vec3& localPoint = Vec3::Zero ) override;
	virtual void					AddForce( TRigidBodyHandle rigidBody, const Vec3& force, const Vec3& localPoint = Vec3::Zero ) override;

	virtual void					SetDebugDraw( bool m_bDebug ) override;

private:
	class btBroadphaseInterface*				m_broadphase;
	class btDefaultCollisionConfiguration*		m_collisionConfiguration;
	class btCollisionDispatcher*				m_dispatcher;
	class btSequentialImpulseConstraintSolver*	m_solver;
	class btDiscreteDynamicsWorld*				m_dynamicsWorld;

	class CBulletDebugDrawer*					m_debugDrawer;

	float										m_scale = 0.01f;
};


#endif
