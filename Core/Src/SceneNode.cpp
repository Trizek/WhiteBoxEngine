#include "SceneNode.h"

WHITEBOX_BEGIN

CSceneNode::CSceneNode() : m_parent(nullptr){}

CSceneNode::~CSceneNode(){}

CSceneNode*	CSceneNode::GetParentNode()
{
	return m_parent;
}

void	CSceneNode::AttachTo( CSceneNode* parent, ENodeAttachMode attachMode /*= ENodeAttachMode::ConserveLocalTransform*/ )
{
	Transform globalTransform, localTransform;
	GetGlobalTransform( globalTransform );
	GetLocalTransform( localTransform );
		
	DetachFromParent();

	parent->m_children.push_back( this );
	m_parent = parent;

	if ( m_parent != nullptr )
	{
		if ( attachMode == ENodeAttachMode::ConserveGlobalTransform )
		{
			SetGlobalTransform( globalTransform );		
		}
		else
		{
			SetLocalTransform( localTransform );
		}
	}
}

void	CSceneNode::DetachFromParent()
{
	if ( m_parent != nullptr )
	{
		for( size_t i = 0; i < m_parent->m_children.size(); ++i )
		{
			if ( m_parent->m_children[ i ].get() == this )
			{
				size_t newSize = m_parent->m_children.size() - 1;
				if ( i < newSize )
				{
					m_parent->m_children[ i ] = m_parent->m_children[ newSize ];
				}

				m_parent->m_children.resize( newSize );

				break;
			}
		}
	}
}

void	CSceneNode::PropagateTransform( const Transform& globalParentTransform )
{
	for ( NodePtr child : m_children )
	{
		child->PropagateTransform( globalParentTransform );
	}
}

void	CSceneNode::GetLocalTransform( Transform& localTransform )
{
	localTransform = Transform();
}

void	CSceneNode::SetLocalTransform( const Transform& localTransform ) {}

void	CSceneNode::GetGlobalTransform( Transform& globalTransform )
{
	if ( m_parent != nullptr )
	{
		m_parent->GetGlobalTransform( globalTransform );
	}
	else
	{
		globalTransform = Transform();
	}
}

void	CSceneNode::SetGlobalTransform( const Transform& globalTransform )
{
	Transform parentGlobalTransform;
	if ( m_parent != nullptr )
	{
		m_parent->GetGlobalTransform( parentGlobalTransform );
	}

	SetLocalTransform( parentGlobalTransform.GetInverse() * globalTransform ); 
}
	
void	CSceneNode::Refresh()
{
	for ( NodePtr child : m_children )
	{
		child->Refresh();
	}
}






void	CSpatialNode::GetLocalTransform( Transform& localTransform )
{
	localTransform = m_localTransform;
}

void	CSpatialNode::SetLocalTransform( const Transform& localTransform )
{
	m_localTransform = localTransform;
	UpdateGlobalTransform();
}

void	CSpatialNode::PropagateTransform( const Transform& globalParentTransform )
{
	m_globalTransform = globalParentTransform * m_localTransform;
	CSceneNode::PropagateTransform( m_globalTransform );
}

void	CSpatialNode::GetGlobalTransform( Transform& globalTransform )
{
	globalTransform = m_globalTransform;
}


void	CSpatialNode::UpdateGlobalTransform()
{
	CSceneNode* parentNode = GetParentNode();
	Transform parentTransform;
	if ( parentNode )
	{
		parentNode->GetGlobalTransform( parentTransform );
	}

	PropagateTransform( parentTransform );
}

WHITEBOX_END
