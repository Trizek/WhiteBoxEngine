#ifndef __PHYSICSAPI_H__
#define __PHYSICSAPI_H__

#include "BaseTypes.h"

WHITEBOX_BEGIN

struct IPhysicsRigidBodyShape : public CSmartPointerData
{
	virtual ~IPhysicsRigidBodyShape(){}
};
DEFINE_SMART_PTR( IPhysicsRigidBodyShape );

struct IPhysicsWorld : public CSmartPointerData
{
	// Shapes
	virtual IPhysicsRigidBodyShapePtr	CreateBoxShape( const Vec3& size ) = 0;
	
	virtual ~IPhysicsWorld(){}
};
DEFINE_SMART_PTR( IPhysicsWorld );

struct SPhysicsAPI
{
	IPhysicsWorldPtr	CreatePhysicsWorld();
};

WHITEBOX_END

#endif
