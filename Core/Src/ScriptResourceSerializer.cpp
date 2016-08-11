#include "ScriptResourceSerializer.h"

#include "ScriptSerializer.h"

WHITEBOX_BEGIN

IResource*	IScriptResourceSerializer::Load( CDataStream& dataStream, const CResourceDescriptor& descriptor )
{
	CScriptStreamReader scriptStreamReader;
	SScriptNodePtr pScriptNode = scriptStreamReader.Parse( dataStream );
	
	CScriptNodeReader scriptNodeReader( pScriptNode );
	return Load( scriptNodeReader, descriptor );
}


WHITEBOX_END
