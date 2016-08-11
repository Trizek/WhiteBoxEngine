#ifndef __RIGIDBODYCOMPONENT_H__
#define __RIGIDBODYCOMPONENT_H__

#include "Object.h"
#include "PhysicsAPI.h"

WHITEBOX_BEGIN

class IRigidBodyShape : public CSerializableFactory< IRigidBodyShape >::IBaseClassInterface, public CSmartPointerData
{
public:
	virtual	void CreateShape() = 0;
	void DestroyShape();
	
	virtual ~IRigidBodyShape(){}

protected:
	IPhysicsRigidBodyShapePtr	m_pShape;
};
DEFINE_SMART_PTR( IRigidBodyShape );

struct SRigidBodyShapeFactory : public CSerializableFactory< IRigidBodyShape >
{
};

class CRigidBodyBoxShape : public IRigidBodyShape
{
	DECLARE_SERIALIZABLE_FACTORY( CRigidBodyBoxShape );
	
public:
	CRigidBodyBoxShape();

	virtual	void CreateShape();
	virtual void Serialize( ISerializer& serializer );
	
	const Vec3& GetSize() const;
	void  SetSize( const Vec3& size );
	
private:
	Vec3	m_size;
};

class CRigidBodyComponent : public IComponent
{
	DECLARE_SERIALIZABLE_FACTORY( CRigidBodyComponent )
	
public:	
	static const IComponent::EComponent Type = IComponent::eC_RigidBody;

	virtual void Serialize( ISerializer& serializer );
	virtual EComponent	GetType() const;
	
	template< class TRigidBodyShape >
	TRigidBodyShape*	SetRigidBodyShape()
	{
		TRigidBodyShape* pRigidBodyShape = new TRigidBodyShape();
		m_pRigidBodyShape = pRigidBodyShape;
		return pRigidBodyShape;
	}
	void ResetRigidBodyShape();
	
private:
	virtual void	Load();
	virtual void	Unload();
	
	float	m_mass;
	float	m_friction;
	float	m_restitution;	
	IRigidBodyShapePtr m_pRigidBodyShape;
};

WHITEBOX_END

#endif
