#ifndef __PICTURE_H__
#define __PICTURE_H__

#include "DataStream.h"

WHITEBOX_BEGIN

class CPicture
{
public:
	enum EPictureFormat
	{
		ePF_R8G8B8 = 0,
		ePF_B8G8R8,
		ePF_R8G8B8A8,
		ePF_B8G8R8A8,
	};
	
	CPicture();
	CPicture( const char* extension, CDataStream& datastream );
	~CPicture();
	
	uint			GetWidth() const;
	uint			GetHeight() const;
	EPictureFormat	GetFormat() const;
	void*			GetPixelData();

private:
	void			Load( const char* extension, CDataStream& datastream );
	void			Destroy();

	uint			m_width;
	uint			m_height;
	EPictureFormat	m_format;
	void*			m_pPixelDataInfo;
	
};

WHITEBOX_END

#endif