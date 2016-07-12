#ifndef __MESHSERIALIZER_H__
#define __MESHSERIALIZER_H__

#include "BaseTypes.h"
#include "ResourceSerializer.h"
#include "Render/Mesh.h"

WHITEBOX_BEGIN

class CDataStream;
class CMeshSerializer : public IResourceSerializer
{
public:
	virtual IResource*	Load( CDataStream& dataStream, const CResourceDescriptor& descriptor );
};

WHITEBOX_END

#endif
