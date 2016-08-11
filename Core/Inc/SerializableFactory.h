#ifndef __SERIALIZABLEFACTORY_H__
#define __SERIALIZABLEFACTORY_H__

#include "Serializer.h"

WHITEBOX_BEGIN

template< class BaseClass >
class CSerializableFactory
{
public:
	struct IBaseClassInterface
	{
		virtual const String& GetClassName() const = 0;
		virtual void Serialize( ISerializer& serializer ) = 0;
	};

	template< class DerivedClass >
	void RegisterClass()
	{
		m_instantiateDerivedClassFuncMap[ DerivedClass::GetStaticClassName() ] = &DerivedClassInstantiator< DerivedClass >::Instantiate;	
	}
	
	void Serialize( BaseClass*& pDerivedClass, ISerializer& serializer )
	{
		String derivedClassName;
	
		if ( serializer.IsReading() )
		{
			// delete pBaseClass ?
			if ( pDerivedClass )
			{
				SAFE_DELETE( pDerivedClass );
			}
			serializer.Value( "DerivedClassName", derivedClassName );
			if ( m_instantiateDerivedClassFuncMap.find( derivedClassName ) != m_instantiateDerivedClassFuncMap.end() )
			{
				pDerivedClass = m_instantiateDerivedClassFuncMap[ derivedClassName ]();
			}
			else
			{
				pDerivedClass = NULL;
			}
		}
		else
		{
			derivedClassName = pDerivedClass->GetClassName();
			serializer.Value( "DerivedClassName", derivedClassName );
		}
		
		if ( serializer.BeginGroup( derivedClassName ) )
		{
			pDerivedClass->Serialize( serializer );
			serializer.EndGroup();
		}
	}

private:
	template< class DerivedClass >
	struct DerivedClassInstantiator
	{
		static BaseClass*	Instantiate()
		{
			return new DerivedClass();
		}
	};

	typedef BaseClass* (*TInstantiateDerivedClassFunc)();
	
	Map< String, TInstantiateDerivedClassFunc > m_instantiateDerivedClassFuncMap;
};

#define DECLARE_SERIALIZABLE_FACTORY(className) \
	public: \
	static const String& GetStaticClassName() \
	{ \
		static String name(#className); \
		return name; \
	} \
	virtual const String& GetClassName() const \
	{ \
		return GetStaticClassName(); \
	} \

WHITEBOX_END

#endif