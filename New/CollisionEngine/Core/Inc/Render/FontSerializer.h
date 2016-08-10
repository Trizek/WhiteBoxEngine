#ifndef __FONTSERIALIZER_H__
#define __FONTSERIALIZER_H__

#include "BaseTypes.h"
#include "ResourceSerializer.h"
#include "Font.h"

WHITEBOX_BEGIN

class CFontSerializer : public IResourceSerializer
{
public:
	IResource*	Load( CDataStream& dataStream, const CResourceDescriptor& descriptor );
};

WHITEBOX_END

#endif
