#include "TextureExporter.h"

#include <FreeImage.h>

#if defined(_WIN32) || defined(_WIN64)
extern "C" {
	#include "image_DXT.h"
}
#else
#include "image_DXT.h"
#endif


#include "Png/lodepng.h"

WHITEBOX_BEGIN

void CTextureExporter::Export( const String& assetFolder, const String& resourceFolder, const String& filePath )
{
#ifdef EXPORT_TO_PNG
	const char* extension = ".png";
#else
	const char* extension = ".dds";
#endif

	String completeFilePath = assetFolder + filePath;
	String outFilePath = resourceFolder + filePath.get_path_base() + filePath.get_path_name() + extension;

	if ( gVars->pFileSystem->DoesFileExist( outFilePath ) )
	{
		return;
	}
	 	

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

#ifdef EXPORT_TO_PNG
	
	// Save to PNG
	unsigned char*	pngData = nullptr;
	size_t			pngSize = 0;
	lodepng_encode_memory( &pngData, &pngSize, pixels, width, height, LodePNGColorType::LCT_RGBA, 8 );

	gVars->pFileSystem->CreateFileDir( outFilePath );
	TFileHandle pngFile = gVars->pFileSystem->OpenFile( outFilePath.c_str(), false, true );
	gVars->pFileSystem->Write( pngFile, sizeof(char), pngSize, pngData );
	gVars->pFileSystem->CloseFile( pngFile );

#else
	
	// Save to DDS
 	gVars->pFileSystem->CreateFileDir( outFilePath );
	save_image_as_DDS( outFilePath.c_str(), width, height, 4, pixels );
	
#endif

	delete[] pixels;
	FreeImage_Unload( freeImage );
}

WHITEBOX_END
