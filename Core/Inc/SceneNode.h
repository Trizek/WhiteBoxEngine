#ifndef __SCENENODE_H__
#define __SCENENODE_H__

#include "BaseTypes.h"
#include "SerializableFactory.h"

WHITEBOX_BEGIN

enum class ENodeAttachMode
{
	ConserveLocalTransform = 0,
	ConserveGlobalTransform,
};

class CSceneNode : public CSmartPointerData, public CSerializableFactory::ISerializable
{
public:
	DECLARE_SERIALIZABLE_CLASS(CSceneNode);

	typedef CSmartPointer<CSceneNode> SceneNodePtr;

	CSceneNode( const String& _name );
	CSceneNode();
	virtual ~CSceneNode();

	CSceneNode*	GetParentNode();

	void		AttachTo( CSceneNode* parent, ENodeAttachMode attachMode = ENodeAttachMode::ConserveLocalTransform );
	void		DetachFromParent();

	template< class TFunctor >
	bool ForEachChild( TFunctor functor )
	{
		for( SceneNodePtr child : m_children )
		{
			if ( !functor( child ) || !child->ForEachChild< TFunctor >( functor ) )
			{
				return false;
			}
		}

		return true;
	}

	virtual void	PropagateTransform( const Transform& globalParentTransform );

	virtual void	GetLocalTransform( Transform& localTransform );
	virtual void	SetLocalTransform( const Transform& localTransform );
	virtual void	GetGlobalTransform( Transform& globalTransform );
	void			SetGlobalTransform( const Transform& globalTransform );
	
	virtual void	Refresh();

	virtual void	Serialize( ISerializer& serialize ) override;

public:
	String					name;


private:
	CSceneNode*				m_parent;
	std::vector< SceneNodePtr >	m_children;
};

DEFINE_SMART_PTR(CSceneNode)


class CSpatialNode : public CSceneNode
{
public:
	DECLARE_SERIALIZABLE_CLASS(CSpatialNode);

	CSpatialNode() = default;
	CSpatialNode( const String& _name ) : CSceneNode(_name) {}

	virtual void	GetLocalTransform( Transform& localTransform ) override;
	virtual void	SetLocalTransform( const Transform& localTransform ) override;

	virtual void	PropagateTransform( const Transform& globalParentTransform ) override;
	virtual void	GetGlobalTransform( Transform& globalTransform ) override;

	void			Rotate( Degree Yaw, Degree Pitch, Degree Roll );
	
	virtual void	Serialize( ISerializer& serialize ) override;

private:
	void	UpdateGlobalTransform();

	Transform	m_localTransform;
	Transform	m_globalTransform;
};

WHITEBOX_END

#endif
