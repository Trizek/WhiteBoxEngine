#ifndef __BEHAVIORCOMPONENT_H__
#define __BEHAVIORCOMPONENT_H__

#include "BaseTypes.h"

#include "Object.h"
#include "BehaviorManager.h"

WHITEBOX_BEGIN

class CBehaviorComponent : public IComponent
{
	DECLARE_SERIALIZABLE_FACTORY( CBehaviorComponent )
	
public:	
	static const IComponent::EComponent Type = IComponent::eC_Behavior;

	virtual void Serialize( ISerializer& serializer );
	
	virtual EComponent	GetType() const;
	
	const String&	GetBehaviorClassName() const;
	
	void	SetBehaviorClassName( const String& behaviorClassName );
	
private:
	String			m_behaviorClassName;
	
public:
	CBehaviorPtr	m_pBehavior;
private:

	virtual void	Load();
	virtual void	Unload();
};


WHITEBOX_END

#endif
