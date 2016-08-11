#include "Render/FontSerializer.h"

#include "Render/Font.h"

WHITEBOX_BEGIN

IResource*	CFontSerializer::Load( CDataStream& dataStream, const CResourceDescriptor& descriptor )
{
	return new CFont( dataStream );
}

WHITEBOX_END
