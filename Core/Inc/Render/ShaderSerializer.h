#ifndef __SHADERSERIALIZER_H__
#define __SHADERSERIALIZER_H__

#include "BaseTypes.h"
#include "ResourceSerializer.h"
#include "Shader.h"

WHITEBOX_BEGIN

class CDataStream;
class CShaderSerializer : public IResourceSerializer
{
public:
	IResource*	Load( CDataStream& dataStream, const CResourceDescriptor& descriptor );
};

WHITEBOX_END

#endif
