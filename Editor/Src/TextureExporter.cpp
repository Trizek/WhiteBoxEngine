#include "TextureExporter.h"

#include <FreeImage.h>

#if defined(WIN32) || defined(WIN64)
extern "C" {
	#include "image_DXT.h"
}
#else
#include "image_DXT.h"
#endif

WHITEBOX_BEGIN

void CTextureExporter::Export( const String& assetFolder, const String& resourceFolder, const String& filePath )
{
	String completeFilePath = assetFolder + filePath;

	// Load image with FreeImage
	FREE_IMAGE_FORMAT format = FreeImage_GetFileType( completeFilePath.c_str(), 0 );
	FIBITMAP* freeImage = FreeImage_Load( format, completeFilePath.c_str() );

	FIBITMAP* freeImage32bits = freeImage;
 	freeImage = FreeImage_ConvertTo32Bits( freeImage );
 	FreeImage_Unload( freeImage32bits );

	int width = FreeImage_GetWidth( freeImage );
	int height = FreeImage_GetHeight( freeImage );

	BYTE* freeImagePixels = FreeImage_GetBits( freeImage );

	// From BGRA to RGBA
	unsigned char* pixels = new unsigned char[ width * height * 4 ];

	for (int i = 0; i < width; ++i)
	{
		for (int j = 0; j < height; ++j)
		{
			pixels[ (i + j*width) * 4 + 0 ] = freeImagePixels[ (i + (height - 1 - j)*width) * 4 + 2 ];
			pixels[ (i + j*width) * 4 + 1 ] = freeImagePixels[ (i + (height - 1 - j)*width) * 4 + 1 ];
			pixels[ (i + j*width) * 4 + 2 ] = freeImagePixels[ (i + (height - 1 - j)*width) * 4 + 0 ];
			pixels[ (i + j*width) * 4 + 3 ] = freeImagePixels[ (i + (height - 1 - j)*width) * 4 + 3 ];
		}
	}

	// Save to DDS
	String ddsOutFilePath = resourceFolder + filePath.get_path_base() + filePath.get_path_name() + ".dds";
	gVars->pFileSystem->CreateFileDir( ddsOutFilePath );
	save_image_as_DDS( ddsOutFilePath.c_str(), width, height, 4, pixels );
	
	delete[] pixels;
	FreeImage_Unload( freeImage );
}

WHITEBOX_END
