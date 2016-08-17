#include "Render/Picture.h"

#define FOURCC_DXT1 0x31545844 // Equivalent to "DXT1" in ASCII
#define FOURCC_DXT3 0x33545844 // Equivalent to "DXT3" in ASCII
#define FOURCC_DXT5 0x35545844 // Equivalent to "DXT5" in ASCII

WHITEBOX_BEGIN

CPicture::CPicture()
	: m_width(0)
	, m_height(0)
	, m_mipMapCount(0)
	, m_format(ePF_R8G8B8)
	, m_pPixelData(NULL)
	{}
	
CPicture::CPicture( const char* extension, CDataStream& dataStream )
	: m_width(0)
	, m_height(0)
	, m_mipMapCount(0)
	, m_format(ePF_R8G8B8)
	, m_pPixelData(NULL)
{
	Load( extension, dataStream );
}

CPicture::~CPicture()
{
	Destroy();
}
	
uint CPicture::GetWidth() const
{
	return m_width;
}

uint CPicture::GetHeight() const
{
	return m_height;
}

uint CPicture::GetMipMapCount() const
{
	return m_mipMapCount;
}

CPicture::EPictureFormat	CPicture::GetFormat() const
{
	return m_format;
}

void* CPicture::GetPixelData()
{
	return m_pPixelData;
}

void CPicture::Load( const char* extension, CDataStream& dataStream )
{	
	if ( strcmp( extension, "dds" ) != 0 )
	{
		WbLog( "Default",  "Error loading texture, only DDS is supported\n" );
	}

	char const* header = (char const*)dataStream.GetCursorData() + 4;

	m_height = *(unsigned int*)&(header[8]);
	m_width = *(unsigned int*)&(header[12]);
	unsigned int linearSize = *(unsigned int*)&(header[16]);
	m_mipMapCount = *(unsigned int*)&(header[24]);
	unsigned int fourCC = *(unsigned int*)&(header[80]);

	m_pPixelData = const_cast<char*>(header) + 124;

	switch (fourCC)
	{
	case FOURCC_DXT1:	m_format = ePF_R8G8B8A8_DTX1; break;
	case FOURCC_DXT3:	m_format = ePF_R8G8B8A8_DTX3; break;
	case FOURCC_DXT5:	m_format = ePF_R8G8B8A8_DTX5; break;
	default:
		break;
		// free
	}





// 
//  	if ( strcmp( extension, "jpg" ) == 0 || strcmp( extension, "jpeg" ) == 0 )
//  	{
//  		m_format = ePF_B8G8R8;
//  		fif = FIF_JPEG;
//  	}
//  	else if ( strcmp( extension, "tga" ) == 0 )
//  	{
//  		m_format = ePF_R8G8B8A8;
//  		fif = FIF_TARGA;
//  	}
//  	else if ( strcmp( extension, "dds" ) == 0 )
//  	{
//  		m_format = ePF_B8G8R8A8;
//  		fif = FIF_DDS;
//  	}	
//  	else
//  	{
//  		WbLog( "Default",  "Picture format %s not supported\n", extension );
//  		return;
//  	}
 	
	//	
}

void CPicture::Destroy()
{
	m_pPixelData = nullptr;
}
	
WHITEBOX_END
