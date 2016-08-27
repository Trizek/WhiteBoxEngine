#include "Physics/BulletPhysicsSystem.h"


#include <btBulletDynamicsCommon.h>
#include <BulletCollision/CollisionShapes/btBvhTriangleMeshShape.h>
#include "TriMesh.h"
#include "Application.h"
#include "GlobalVariables.h"
#include "Render/RenderPipeline.h"

using namespace WhiteBox;


class CBulletDebugDrawer : public btIDebugDraw
{
public:

	CBulletDebugDrawer( float scale ) : m_scale(scale), m_debugMode(0) {}
	virtual ~CBulletDebugDrawer() {}

	virtual void   drawLine(const btVector3& from, const btVector3& to, const btVector3& fromColor, const btVector3& toColor)
	{
		drawLine(from, to, fromColor);
	}

	virtual void   drawLine(const btVector3& from, const btVector3& to, const btVector3& color)
	{
		float scale = 1.0f / m_scale;
		Vec3 p0( scale * from.x(), -scale * from.z(), scale * from.y() );
		Vec3 p1( scale * to.x(), -scale * to.z(), scale * to.y() );

		gVars->pApplication->m_pRenderPipeline->DrawLine( p0, p1, Color(color.x(), color.y(), color.z()));
	}

	virtual void   drawSphere(const btVector3& p, btScalar radius, const btVector3& color)
	{
	}

	virtual void   drawTriangle(const btVector3& a, const btVector3& b, const btVector3& c, const btVector3& color, btScalar alpha)
	{
	}

	virtual void   drawContactPoint(const btVector3& PointOnB, const btVector3& normalOnB, btScalar distance, int lifeTime, const btVector3& color)
	{
	}

	virtual void   reportErrorWarning(const char* warningString)
	{
	}

	virtual void   draw3dText(const btVector3& location, const char* textString) {}

	virtual void   setDebugMode( int debugMode )
	{
		m_debugMode = debugMode;
	}

	virtual int      getDebugMode() const
	{
		return m_debugMode;
	}

private:
	float	m_scale;
	int		m_debugMode;

};

CBulletPhysicsSystem::CBulletPhysicsSystem()
{
	m_broadphase = new btDbvtBroadphase();

	m_collisionConfiguration = new btDefaultCollisionConfiguration();
	m_dispatcher = new btCollisionDispatcher( m_collisionConfiguration );

	m_solver = new btSequentialImpulseConstraintSolver;

	m_dynamicsWorld = new btDiscreteDynamicsWorld( m_dispatcher, m_broadphase, m_solver, m_collisionConfiguration );

	m_dynamicsWorld->setGravity( btVector3(0, -9.8f, 0) );

	m_scale = 0.1f;

	m_debugDrawer = new CBulletDebugDrawer( m_scale );
	m_dynamicsWorld->setDebugDrawer( m_debugDrawer );
}

CBulletPhysicsSystem::~CBulletPhysicsSystem()
{
	delete m_dynamicsWorld;
	delete m_solver;
	delete m_collisionConfiguration;
	delete m_dispatcher;
	delete m_broadphase;

	delete m_debugDrawer;
}


void	CBulletPhysicsSystem::UpdateSimulation( float frameTime, float fixedTimeStep /*= 1.0f / 60.0f*/ )
{
	m_dynamicsWorld->stepSimulation( frameTime, 10, fixedTimeStep );
	m_dynamicsWorld->debugDrawWorld();
}

TColliderHandle	CBulletPhysicsSystem::CreateSphereCollider( float radius )
{
	return new btSphereShape( radius * m_scale);
}

TColliderHandle	CBulletPhysicsSystem::CreateBoxCollider( const Vec3& size )
{
	return new btBoxShape( btVector3(m_scale * size.x * 0.5f, m_scale * size.z * 0.5f, m_scale * size.y * 0.5f) );
}

TColliderHandle	CBulletPhysicsSystem::CreateCapsuleCollider( float radius, float height )
{
	return new btCapsuleShape( m_scale * radius, m_scale * height );
}

TColliderHandle	CBulletPhysicsSystem::CreateStaticPlaneCollider( const Vec3& point, const Vec3& normal )
{
	float constant = -m_scale * (point | normal);
	return new btStaticPlaneShape( btVector3(normal.x, normal.z, -normal.y), constant );
}

TColliderHandle	CBulletPhysicsSystem::CreateStaticMeshCollider( CTriMesh* pTriMesh, const Transform& transform )
{
	btTriangleMesh* mesh = new btTriangleMesh;

	pTriMesh->ForEachTriangle([&]( const Vec3& v0, const Vec3& v1, const Vec3& v2 )
	{
		Vec3 p0 = m_scale * (transform * v0);
		Vec3 p1 = m_scale * (transform * v1);
		Vec3 p2 = m_scale * (transform * v2);

		

		mesh->addTriangle( btVector3(p0.x, p0.z, -p0.y), btVector3(p1.x, p1.z, -p1.y), btVector3(p2.x, p2.z, -p2.y));
	});

	//btTriangleIndexVertexArray* indexVertexArray = new btTriangleIndexVertexArray(pTriMesh->m_indices.size() / 3, (int*)&(pTriMesh->m_indices[0]), 3 * sizeof(uint), pTriMesh->m_vertices.size(), (float*)&(pTriMesh->m_vertices[0]), 3 * sizeof(float));
	return new btBvhTriangleMeshShape( mesh, false );
}
	
TRigidBodyHandle	CBulletPhysicsSystem::CreateRigidyBody( TColliderHandle collider, const Transform& transform, float mass /*= 0*/ )
{
	SBulletRigidyBody* pBulletRigidBody = new SBulletRigidyBody();
	
	pBulletRigidBody->pMotionState = new btDefaultMotionState( btTransform(btQuaternion(transform.rotation.x, transform.rotation.y, transform.rotation.z, transform.rotation.w)
		, btVector3(m_scale* transform.position.x, m_scale* transform.position.z, -m_scale* transform.position.y)) );

	btVector3 inertia(0, 0, 0);
	if ( mass > 0.0f )
	{
		((btCollisionShape*)collider)->calculateLocalInertia( mass, inertia );
	}

	//((btCollisionShape*)collider)->setMargin(1.0f);

	btRigidBody::btRigidBodyConstructionInfo constructInfo( mass, pBulletRigidBody->pMotionState, (btCollisionShape*)collider, inertia );
	constructInfo.m_restitution = 0.8f;

	pBulletRigidBody->pRigidBody = new btRigidBody( constructInfo );

	if ( mass == 0.0f )
	{
		//pBulletRigidBody->pRigidBody->setCollisionFlags(btCollisionObject::CF_STATIC_OBJECT);
	}

	return pBulletRigidBody;
}

void	CBulletPhysicsSystem::AddRigidyBody( TRigidBodyHandle rigidBody )
{
	if (((SBulletRigidyBody*)rigidBody)->pRigidBody->getInvMass() == 0.0f)
	{
		m_dynamicsWorld->addCollisionObject(((SBulletRigidyBody*)rigidBody)->pRigidBody);
	}

	m_dynamicsWorld->addRigidBody( ((SBulletRigidyBody*)rigidBody)->pRigidBody );
}

void	CBulletPhysicsSystem::RemoveRigidyBody( TRigidBodyHandle rigidBody )
{
	m_dynamicsWorld->removeRigidBody( ((SBulletRigidyBody*)rigidBody)->pRigidBody );
}


void	CBulletPhysicsSystem::GetRigidBodyTransform( TRigidBodyHandle rigidBody, Transform& transform )
{
	btTransform bt;
	((SBulletRigidyBody*)rigidBody)->pMotionState->getWorldTransform(bt);
	
	Vec3 pos = *((Vec3*)bt.getOrigin().m_floats);
	transform.position = (1.0f / m_scale) * Vec3(pos.x, -pos.z, pos.y);;
	transform.rotation = Quat( bt.getRotation().w(), bt.getRotation().x(), -bt.getRotation().z(), bt.getRotation().y() );
}

void	CBulletPhysicsSystem::AddImpulse( TRigidBodyHandle rigidBody, const Vec3& impulse, const Vec3& localPoint /*= Vec3::Zero*/ )
{
	((SBulletRigidyBody*)rigidBody)->pRigidBody->applyImpulse( btVector3(impulse.x, impulse.z, -impulse.y), btVector3(localPoint.x, localPoint.z, -localPoint.y) );
}

void	CBulletPhysicsSystem::AddForce( TRigidBodyHandle rigidBody, const Vec3& force, const Vec3& localPoint /*= Vec3::Zero*/ )
{
	((SBulletRigidyBody*)rigidBody)->pRigidBody->applyImpulse( btVector3(force.x, force.z, -force.y), btVector3(localPoint.x, localPoint.z, -localPoint.y) );
}

void	CBulletPhysicsSystem::SetDebugDraw( bool m_bDebug )
{
	m_debugDrawer->setDebugMode( (int)m_bDebug );
}