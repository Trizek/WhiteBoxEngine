#ifndef __BEHAVIORMANAGER_H__
#define __BEHAVIORMANAGER_H__

#include "BaseTypes.h"

#include "Object.h"





#include "InputManager.h"
#include "GlobalVariables.h"




WHITEBOX_BEGIN

class CBehaviorComponent;
class CBehaviorManager;
class CBehavior : public CSmartPointerData
{
public:
	CObjectPtr	GetOwner();
	
	virtual void Serialize( ISerializer& serializer ){}
	virtual void UpdateTTT( float frameTime ) = 0;

protected:
	friend class CBehaviorManager;
	friend class CBehaviorComponent;

	CBehaviorComponent*	m_pBehaviorComponent;
	Handle	m_hOwner;
};

DEFINE_SMART_PTR( CBehavior );

class CBehaviorManager
{
private:
	typedef CBehavior* (*TCreateBehaviorInstanceFunc)( Handle hOwner );
	typedef Map< String, TCreateBehaviorInstanceFunc > TCreateBehaviorInstanceFuncMap;

public:
	template< class TBehaviorClass >
	static int AddBehaviorClass()
	{
		s_createBehaviorInstanceFuncMap[ TBehaviorClass::GetClassName() ] = &CreateBehaviorInstance< TBehaviorClass >;
		return 0;
	}
	
	CBehaviorPtr	CreateBehaviorInstance( const String& className, Handle hOwner )
	{
		return s_createBehaviorInstanceFuncMap[ className ]( hOwner );
	}	
	
	template< class TBehaviorClass >
	TBehaviorClass*	CreateBehaviorInstance( Handle hOwner )
	{
		return s_createBehaviorInstanceFuncMap[ TBehaviorClass::GetClassName() ]( hOwner );
	}
	
private:
	template< class TBehaviorClass >
	static CBehavior* CreateBehaviorInstance( Handle hOwner )
	{
		CBehavior* pBehavior = new TBehaviorClass();
		//pBehavior->m_pBehaviorComponent = pBehaviorComponent;
		pBehavior->m_hOwner = hOwner;
		
		return pBehavior;
	}
	
	static TCreateBehaviorInstanceFuncMap	s_createBehaviorInstanceFuncMap;
};

#define DECLARE_BEHAVIOR( className )	static const String& GetClassName(){	\
	static const String staticClassName = #className;	\
	return staticClassName;	\
	}\
	
#define REGISTER_BEHAVIOR( className ) \
	namespace { \
		int registered = CBehaviorManager::AddBehaviorClass< className >(); \
	}\


class CMyBehavior : public CBehavior
{
public:
	DECLARE_BEHAVIOR( CMyBehavior )
	
	CMyBehavior()
	{
		m_speed = 200.0f;
	}

	virtual void Serialize( ISerializer& serializer )
	{
		serializer.Value( "speed", m_speed );
	}

	virtual void UpdateTTT( float frameTime )
	{	
		if ( gVars->pInputManager->IsButtonJustPressed( CInputManager::eB_Key_J ) )
		{
			m_speed *= -1.0f;
		}
		
		Vec3 pos = GetOwner()->GetLocalPosition();
		pos.z += m_speed * frameTime;
		
		WbLog( "Default", " Caca  \n\n");
		
		GetOwner()->SetLocalPosition(pos);
	}
	
private:
	float m_speed;
};

REGISTER_BEHAVIOR( CMyBehavior )




WHITEBOX_END

#endif
