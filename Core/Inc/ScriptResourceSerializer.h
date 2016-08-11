#ifndef __SCRIPTRESOURCESERIALIZER_H__
#define __SCRIPTRESOURCESERIALIZER_H__

#include "ResourceSerializer.h"
#include "Serializer.h"

WHITEBOX_BEGIN

class IScriptResourceSerializer : public IResourceSerializer
{
public:
	virtual IResource*	Load( CDataStream& dataStream, const CResourceDescriptor& descriptor );
	
private:
	virtual IResource*	Load( ISerializer& serializer, const CResourceDescriptor& descriptor ) = 0;
};

WHITEBOX_END

#endif