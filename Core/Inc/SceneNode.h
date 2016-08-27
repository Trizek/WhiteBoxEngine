#ifndef __SCENENODE_H__
#define __SCENENODE_H__

#include "BaseTypes.h"

WHITEBOX_BEGIN

enum class ENodeAttachMode
{
	ConserveLocalTransform = 0,
	ConserveGlobalTransform,
};

class CSceneNode : public CSmartPointerData
{
public:

	typedef CSmartPointer<CSceneNode> NodePtr;

	CSceneNode();
	virtual ~CSceneNode();

	CSceneNode*	GetParentNode();

	void		AttachTo( CSceneNode* parent, ENodeAttachMode attachMode = ENodeAttachMode::ConserveLocalTransform );
	void		DetachFromParent();

	template< class TFunctor >
	bool ForEachChild( TFunctor functor )
	{
		for( NodePtr child : m_children )
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


private:
	CSceneNode*				m_parent;
	std::vector< NodePtr >	m_children;
};

DEFINE_SMART_PTR(CSceneNode)


class CSpatialNode : public CSceneNode
{
public:

	virtual void	GetLocalTransform( Transform& localTransform ) override;
	virtual void	SetLocalTransform( const Transform& localTransform ) override;

	virtual void	PropagateTransform( const Transform& globalParentTransform ) override;
	virtual void	GetGlobalTransform( Transform& globalTransform ) override;

private:
	void	UpdateGlobalTransform();

	Transform	m_localTransform;
	Transform	m_globalTransform;
};

WHITEBOX_END

#endif
