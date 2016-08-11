#ifndef __TEXTURE_H__
#define __TEXTURE_H__

#include "ResourcePointer.h"
#include "Picture.h"

WHITEBOX_BEGIN

class CTexture : public IResource
{
public:
	DECLARE_RESOURCE_TYPE( CTexture );

	enum EPictureFormat
	{
		ePF_R8G8B8 = 0,
		ePF_B8G8R8,
		ePF_R8G8B8A8,
	};
	
	CTexture( uint width, uint height, uint mipMapCount, CPicture::EPictureFormat format, void* pPixelData );
	~CTexture();
	
	uint						GetWidth() const;
	uint						GetHeight() const;
	CPicture::EPictureFormat	GetFormat() const;
	void*						GetTextureId() const;

private:
	uint						m_width;
	uint						m_height;
	uint						m_mipMapCount;
	CPicture::EPictureFormat	m_format;
	
	void*						m_pTextureId;
};

DECLARE_RESOURCE_POINTER( CTexture );

WHITEBOX_END

#endif