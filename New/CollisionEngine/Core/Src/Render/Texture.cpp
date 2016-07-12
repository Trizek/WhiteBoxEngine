#include "Render/Texture.h"

#include "GlobalVariables.h"
#include "Render/Renderer.h"

WHITEBOX_BEGIN

CTexture::CTexture( uint width, uint height, uint mipMapCount, CPicture::EPictureFormat format, void* pPixelData )
	: m_width(width)
	, m_height(height)
	, m_mipMapCount(mipMapCount)
	, m_format(format)
{
	m_pTextureId = gVars->pRenderer->CreateTexture( m_width, m_height, m_mipMapCount, m_format, pPixelData );
}

CTexture::~CTexture()
{
	gVars->pRenderer->DestroyTexture( m_pTextureId );
}

uint CTexture::GetWidth() const
{
	return m_width;
}

uint CTexture::GetHeight() const
{
	return m_height;
}

CPicture::EPictureFormat CTexture::GetFormat() const
{
	return m_format;
}

void*	CTexture::GetTextureId() const
{
	return m_pTextureId;
}

WHITEBOX_END