#ifndef __SERIALIZER_H__
#define __SERIALIZER_H__

#include "BaseTypes.h"

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
};

WHITEBOX_END

#endif