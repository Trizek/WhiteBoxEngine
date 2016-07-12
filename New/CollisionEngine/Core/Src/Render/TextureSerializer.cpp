#include "Render/TextureSerializer.h"

#include "Render/Picture.h"
#include "Render/Texture.h"

WHITEBOX_BEGIN

IResource*	CTextureSerializer::Load( CDataStream& dataStream, const CResourceDescriptor& descriptor )
{
	CPicture picture( descriptor.GetExtension().c_str() , dataStream );
	void* pPixelData = picture.GetPixelData();
	if ( pPixelData == NULL )
	{
		return NULL;
	}

	CTexture* pTexture = new CTexture( picture.GetWidth(), picture.GetHeight(), picture.GetMipMapCount(), picture.GetFormat(), pPixelData );
	return pTexture;
}

WHITEBOX_END
