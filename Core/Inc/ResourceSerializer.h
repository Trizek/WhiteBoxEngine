#ifndef __RESOURCESERIALIZER_H__
#define __RESOURCESERIALIZER_H__

#include "Resource.h"
#include "ResourceDescriptor.h"
#include "DataStream.h"

WHITEBOX_BEGIN

class IResourceSerializer
{
public:
	virtual IResource*	Load( CDataStream& DataStream, const CResourceDescriptor& descriptor ) = 0;
};

WHITEBOX_END

#endif