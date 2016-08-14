#include "Render/FontSerializer.h"

#include "Render/Font.h"

WHITEBOX_BEGIN

IResource*	CFontSerializer::Load( CDataStream& dataStream, const CResourceDescriptor& descriptor )
{
#ifndef __GEAR_VR
	return new CFont( dataStream );
#else
	return nullptr;
#endif
}

WHITEBOX_END
