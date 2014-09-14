#include "Render/Picture.h"

#include <FreeImage.h>

namespace FreeImageWrapper
{
	inline unsigned FreeImage_ReadProc(void *buffer, unsigned size, unsigned count, fi_handle handle)
	{
		WhiteBox::CDataStream* pStream = (WhiteBox::CDataStream*)handle;
		size_t total_size = size * count;
		size_t max_size = pStream->GetSize() - pStream->GetCursor();
		if ( total_size > max_size )
		{
			total_size = max_size;
		}
		
		pStream->Read( size, count, buffer );
		return total_size;
	}
	
	inline unsigned FreeImage_WriteProc(void *buffer, unsigned size, unsigned count, fi_handle handle)
	{
		WhiteBox::CDataStream* pStream = (WhiteBox::CDataStream*)handle;
		size_t total_size = size * count;
		size_t max_size = pStream->GetSize() - pStream->GetCursor();
		if ( total_size > max_size )
		{
			total_size = max_size;
		}
		
		pStream->Write( size, count, buffer );
		return total_size;	
	}

	inline int  FreeImage_SeekProc(fi_handle handle, long offset, int origin)
	{
		WhiteBox::CDataStream* pStream = (WhiteBox::CDataStream*)handle;
		size_t cursor = 0;
		if ( origin == SEEK_CUR )
		{
			cursor = pStream->GetCursor();
		}
		else if ( origin == SEEK_END )
		{
			cursor = pStream->GetSize();
		}
		cursor += offset;
		
		if ( cursor > pStream->GetSize() )
		{
			return -1;
		}
		
		pStream->SetCursor( cursor );
		return 0;
	}

	inline long FreeImage_TellProc(fi_handle handle)
	{
		WhiteBox::CDataStream* pStream = (WhiteBox::CDataStream*)handle;
		return pStream->GetCursor();
	}	
};

WHITEBOX_BEGIN

CPicture::CPicture()
	: m_width(0)
	, m_height(0)
	, m_format(ePF_R8G8B8)
	, m_pPixelDataInfo(NULL)
	{}
	
CPicture::CPicture( const char* extension, CDataStream& dataStream )
	: m_width(0)
	, m_height(0)
	, m_format(ePF_R8G8B8)
	, m_pPixelDataInfo(NULL)
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

CPicture::EPictureFormat	CPicture::GetFormat() const
{
	return m_format;
}

void* CPicture::GetPixelData()
{
	return FreeImage_GetBits( (FIBITMAP*)m_pPixelDataInfo );
}

void CPicture::Load( const char* extension, CDataStream& dataStream )
{	
	FREE_IMAGE_FORMAT fif = FIF_UNKNOWN;

	if ( strcmp( extension, "jpg" ) == 0 || strcmp( extension, "jpeg" ) == 0 )
	{
		m_format = ePF_B8G8R8;
		fif = FIF_JPEG;
	}
	else if ( strcmp( extension, "tga" ) == 0 )
	{
		m_format = ePF_R8G8B8A8;
		fif = FIF_TARGA;
	}
	else if ( strcmp( extension, "dds" ) == 0 )
	{
		m_format = ePF_B8G8R8A8;
		fif = FIF_DDS;
	}	
	else
	{
		printf( "Picture format %s not supported\n", extension );
		return;
	}
	
	FreeImageIO io;

	io.read_proc  = FreeImageWrapper::FreeImage_ReadProc;
	io.write_proc = FreeImageWrapper::FreeImage_WriteProc;
	io.tell_proc  = FreeImageWrapper::FreeImage_TellProc;
	io.seek_proc  = FreeImageWrapper::FreeImage_SeekProc;
		
	
	m_pPixelDataInfo = FreeImage_LoadFromHandle( fif, &io, (fi_handle)&dataStream );
	m_width = FreeImage_GetWidth( (FIBITMAP*)m_pPixelDataInfo );
	m_height = FreeImage_GetHeight( (FIBITMAP*)m_pPixelDataInfo );
	printf( "Loaded picture %d x %d, data %p\n", m_width, m_height, m_pPixelDataInfo );
}

void CPicture::Destroy()
{
	if ( m_pPixelDataInfo != NULL )
	{
		FreeImage_Unload( (FIBITMAP*)m_pPixelDataInfo );
		m_pPixelDataInfo = NULL;
	}
}
	
WHITEBOX_END
