#ifndef __SKELETONSERIALIZER_H__
#define __SKELETONSERIALIZER_H__

#include "BaseTypes.h"
#include "ResourceSerializer.h"
#include "Animation/Skeleton.h"

WHITEBOX_BEGIN

class CDataStream;
class CSkeletonSerializer : public IResourceSerializer
{
public:
	virtual IResource*	Load( CDataStream& dataStream, const CResourceDescriptor& descriptor );
};

WHITEBOX_END

#endif
