#ifndef __TEXTURESERIALIZER_H__
#define __TEXTURESERIALIZER_H__

#include "BaseTypes.h"
#include "ResourceSerializer.h"
#include "Texture.h"

WHITEBOX_BEGIN

class CDataStream;
class CTextureSerializer : public IResourceSerializer
{
public:
	IResource*	Load( CDataStream& dataStream, const CResourceDescriptor& descriptor );
};

WHITEBOX_END

#endif
