#ifndef __ANIMATIONSERIALIZER_H__
#define __ANIMATIONSERIALIZER_H__

#include "BaseTypes.h"
#include "ResourceSerializer.h"
#include "Animation/Animation.h"

WHITEBOX_BEGIN

class CDataStream;
class CAnimationSerializer : public IResourceSerializer
{
public:
	virtual IResource*	Load( CDataStream& dataStream, const CResourceDescriptor& descriptor );
};

WHITEBOX_END

#endif
