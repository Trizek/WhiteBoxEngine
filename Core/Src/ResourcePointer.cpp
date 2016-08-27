
#include "ResourcePointer.h"
#include "ResourceManager.h"
#include "GlobalVariables.h"

WHITEBOX_BEGIN

CResourceDescriptor*	CResourcePointer_GetDescriptor( const String& path )
{
	return gVars->pResourceManager->GetResourceDescriptor( path );
}

WHITEBOX_END
