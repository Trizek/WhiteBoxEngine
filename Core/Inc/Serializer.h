#ifndef __SERIALIZER_H__
#define __SERIALIZER_H__

#include "BaseTypes.h"
#include "resourcepointer.h"

WHITEBOX_BEGIN

struct ISerializer
{
	virtual bool IsReading() const = 0;

	virtual bool BeginGroup( const String& name ) = 0;
	virtual void EndGroup() = 0;

	virtual bool Value( const String& name, int& val ) = 0;
	virtual bool Value( const String& name, float& val ) = 0;
	virtual bool Value( const String& name, String& val ) = 0;
	
	virtual bool Value( const String& name, Vec2& vec ) = 0;
	virtual bool Value( const String& name, Vec3& vec ) = 0;
	virtual bool Value( const String& name, Vec4& vec ) = 0;

	bool Value( const String& name, Quat& quat )
	{
		Vec4* pVec = (Vec4*)&quat;
		return Value( name, *pVec );
	}

	bool Value( const String& name, Transform& transform )
	{
		if ( BeginGroup( name ) )
		{
			Value( "position", transform.position );
			Value( "rotation", transform.rotation );
			Value( "scale", transform.scale );

			EndGroup();
		
			return true;
		}

		return false;
	}

	template< class TResourcePointer >
	bool Value( const String& name, CResourcePointer<typename TResourcePointer::TMappedType>& resource )
	{
		if ( IsReading() )
		{
			String resourceName;
			bool bVal = Value( name, resourceName );
			resource = resourceName;
			return bVal;
		}
		else
		{
			String resourceName = resource.GetDescriptor().GetPath();
			return Value( name, resourceName );
		}
	}

	virtual ~ISerializer(){}
};

WHITEBOX_END

#endif