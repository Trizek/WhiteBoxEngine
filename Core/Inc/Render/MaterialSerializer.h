#ifndef __MATERIALSERIALIZER_H__
#define __MATERIALSERIALIZER_H__

#include "BaseTypes.h"
#include "ScriptResourceSerializer.h"
#include "Material.h"

WHITEBOX_BEGIN

class CDataStream;
class CMaterialSerializer : public IScriptResourceSerializer
{
public:
	virtual IResource*	Load( ISerializer& serializer, const CResourceDescriptor& descriptor );
};

WHITEBOX_END

#endif
