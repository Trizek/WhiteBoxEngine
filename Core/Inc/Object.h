#ifndef __OBJECT_H__
#define __OBJECT_H__

#include "Handle.h"
#include "SerializableFactory.h"
#include "RenderAPI.h"

WHITEBOX_BEGIN

class CObject;
class IComponent : public CSerializableFactory< IComponent >::IBaseClassInterface
{
public:
	enum EComponent
	{
		eC_MeshRender = 0,
		eC_RigidBody,
		eC_Light,
		eC_Behavior,
		
		eC_Count,
	};
	
	virtual EComponent	GetType() const = 0;
	
	CObject*	GetOwner();
	
private:
	friend class CObject;
	
	virtual void	Load() = 0;
	virtual void	Unload() = 0;	
	
	Handle	m_hOwner;
};

class CObjectManager;

class CObject : public CSmartPointerData
{
public:
	~CObject(){} // Destroy components !!!

	const String& GetName() const;
	Handle GetHandle() const;
	
	const Vec3& GetLocalPosition() const;
	void	SetLocalPosition( const Vec3& localPosition );
	
	const Vec3& GetLocalRotAngles() const;
	void	SetLocalRotAngles( const Vec3& localRotAngles );	

	const Vec3& GetLocalScale() const;
	void	SetLocalScale( const Vec3& localScale );

	template< class ComponentClass >
	ComponentClass*	AddComponent()
	{
		ComponentClass* pComponent = new ComponentClass();
		pComponent->m_hOwner = m_handle;
		m_componentList[ ComponentClass::Type ].push_back( pComponent );
		return pComponent;
	}

	template< class ComponentClass >
	ComponentClass*	GetComponent( size_t index = 0 )
	{
		return static_cast< ComponentClass* >( m_componentList[ ComponentClass::Type ][ index ] );
	}
	
	void Load( bool bLoadChildren = true );
	void Unload( bool bUnloadChildren = true );
	
	void DetachFromParent();
	void AttachChild( CSmartPointer< CObject > pObject );
	void DetachChild( CSmartPointer< CObject > pObject );
	void AttachToRootSceneNode( bool bAttach );
	
	void Serialize( ISerializer& serializer, CObjectManager& objectManager );
	
	IRenderSceneNodePtr	GetRenderSceneNode();

private:
	friend class CObjectManager;

	CObject();

	Handle	m_handle;
	String	m_name;
	Vec3	m_localPosition;
	Vec3	m_localRotAngles; // in degrees
	Vec3	m_localScale;

	typedef std::vector< IComponent* > TComponentArray;
	TComponentArray	m_componentList[ IComponent::eC_Count ];
	
	typedef std::vector< Handle > TChildHandleArray;
	TChildHandleArray m_childHandles;
	
	Handle	m_hParent;
	
	IRenderSceneNodePtr m_pRenderSceneNode;
};

DEFINE_SMART_PTR( CObject );

/*

class CScriptComponent : public IComponent
{
	DECLARE_SERIALIZABLE_FACTORY( CScriptComponent )
	
public:	
	static const IComponent::EComponent Type = IComponent::eC_Script;

	virtual void Serialize( ISerializer& serializer );
	virtual EComponent	GetType() const
	{
		return Type;
	}
	
	
	
private:
	String			m_className;

	virtual void	Load()
	{
	}
	virtual void	Unload()
	{
	}
};*/



WHITEBOX_END

#endif
