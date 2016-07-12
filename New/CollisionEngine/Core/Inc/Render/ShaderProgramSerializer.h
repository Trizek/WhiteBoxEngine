#ifndef __SHADERPROGRAMSERIALIZER_H__
#define __SHADERPROGRAMSERIALIZER_H__

#include "BaseTypes.h"
#include "ScriptResourceSerializer.h"
#include "ShaderProgram.h"

WHITEBOX_BEGIN

class CDataStream;
class CShaderProgramSerializer : public IScriptResourceSerializer
{
public:
	virtual IResource*	Load( ISerializer& serializer, const CResourceDescriptor& descriptor );
};

WHITEBOX_END

#endif
