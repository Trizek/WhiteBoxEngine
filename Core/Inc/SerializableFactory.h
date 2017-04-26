#ifndef __SERIALIZABLEFACTORY_H__
#define __SERIALIZABLEFACTORY_H__

#include "Serializer.h"

WHITEBOX_BEGIN

class CSerializableFactory
{
public:
	struct ISerializable
	{
		virtual const String& GetClassName() const = 0;
		virtual void Serialize( ISerializer& serializer ) = 0;
		virtual ~ISerializable() {};
	};

	static CSerializableFactory&	Get()
	{
		static CSerializableFactory s_instance;
		return s_instance;
	}

	template< class T >
	bool RegisterNewClass()
	{
		m_loadFuncMap[ T::GetStaticClassName() ] = &LoadStaticFunc< T >;
		return true;
	}

	template< class T >
	void Serialize( T*& pObject, ISerializer& serializer )
	{
		String className;
	
		if ( serializer.IsReading() )
		{
			if ( serializer.BeginGroup( "Class" ) )
			{
				serializer.Value( "ClassName", className );

				TLoadFunc* pLoadFunc = m_loadFuncMap.FindElement( className );
				if ( pLoadFunc )
				{
					if ( pObject )
					{
						SAFE_DELETE( pObject );
					}

					pObject = static_cast<T*>( (*pLoadFunc)( serializer ) );
				}

				serializer.EndGroup();
			}
		}
		else
		{
			className = pObject->GetClassName();
			serializer.BeginGroup( "Class" );
			serializer.Value( "ClassName", className );
			pObject->Serialize( serializer );
			serializer.EndGroup();
		}
	}

private:
	template< class T >
	static ISerializable* LoadStaticFunc( ISerializer& serializer )
	{
		T* pObject = new T;
		pObject->Serialize( serializer );
		return pObject;
	}

	typedef ISerializable* (*TLoadFunc)( ISerializer& serializer );

	Map< String, TLoadFunc > m_loadFuncMap;
};



WHITEBOX_END

#define DECLARE_SERIALIZABLE_CLASS(className) \
	public: \
	static const String& GetStaticClassName() \
	{ \
		static String name(#className); \
		return name; \
	} \
	virtual const String& GetClassName() const override\
	{ \
		return GetStaticClassName(); \
	} \

#define DEFINE_SERIALIZABLE_CLASS(className) \
	namespace className##SerializeNamespace \
	{ \
		bool bDummy = CSerializableFactory::Get().RegisterNewClass< className >(); \
	};

#endif