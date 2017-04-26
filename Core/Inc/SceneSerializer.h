#ifndef __SCENESERIALIZER_H__
#define __SCENESERIALIZER_H__

#include "BaseTypes.h"
#include "ScriptResourceSerializer.h"
#include "Scene.h"

WHITEBOX_BEGIN

class CDataStream;
class CSceneSerializer : public IScriptResourceSerializer
{
public:
	virtual IResource*	Load( ISerializer& serializer, const CResourceDescriptor& descriptor );
};

WHITEBOX_END

#endif
