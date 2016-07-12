#ifndef __RESOURCE_H__
#define __RESOURCE_H__

#include "BaseTypes.h"

#define DECLARE_RESOURCE_TYPE( name ) \
	static const String&	GetStaticName() \
	{ \
		static String staticName = #name; \
		return staticName; \
	} \
	
WHITEBOX_BEGIN

class IResource
{
public:
	virtual void OnPostLoad(){}

	virtual ~IResource(){}
};

WHITEBOX_END

#endif