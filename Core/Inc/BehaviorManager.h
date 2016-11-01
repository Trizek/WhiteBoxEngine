#ifndef __BEHAVIORMANAGER_H__
#define __BEHAVIORMANAGER_H__

#include "Behavior.h"

WHITEBOX_BEGIN

class CBehaviorManager
{
public:
	void	AddBehavior( IBehavior* pBehavior )
	{
		m_behaviors.push_back( pBehavior );
	}

	void	RemoveBehavior( IBehavior* pBehavior )
	{
		for( size_t i = 0; i < m_behaviors.size(); ++i )
		{
			if ( m_behaviors[i] == pBehavior )
			{
				if ( i < m_behaviors.size() - 1 )
				{
					m_behaviors[i] = m_behaviors[m_behaviors.size() - 1];
				}
				m_behaviors.resize( m_behaviors.size() - 1 );
				break;
			}
		}
	}

	void	Update( float frameTime )
	{
		for( IBehavior* pBehavior : m_behaviors )
		{
			if ( pBehavior->GetParentNode() )
			{
				pBehavior->Update( frameTime );
			}
		}
	}

private:
	std::vector< IBehavior* >	m_behaviors;
};

WHITEBOX_END

#endif
