#ifndef __PICTURE_H__
#define __PICTURE_H__

#include "DataStream.h"

WHITEBOX_BEGIN

class CPicture
{
public:
	enum EPictureFormat : unsigned int
	{
		ePF_R8G8B8 = 0,
		ePF_B8G8R8,
		ePF_R8G8B8A8,
		ePF_B8G8R8A8,
		ePF_R8G8B8A8_DTX1,
		ePF_R8G8B8A8_DTX3,
		ePF_R8G8B8A8_DTX5,
	};
	
	CPicture();
	CPicture( const char* extension, CDataStream& datastream );
	~CPicture();
	
	uint			GetWidth() const;
	uint			GetHeight() const;
	uint			GetMipMapCount() const;
	EPictureFormat	GetFormat() const;
	void*			GetPixelData();

private:
	void			Load( const char* extension, CDataStream& datastream );
	void			Destroy();

	void			LoadDDS( CDataStream& dataStream );
	void			LoadPNG( CDataStream& dataStream );


	uint			m_width;
	uint			m_height;
	uint			m_mipMapCount;
	EPictureFormat	m_format;

	void*			m_pPixelData;
	
};

WHITEBOX_END

#endif