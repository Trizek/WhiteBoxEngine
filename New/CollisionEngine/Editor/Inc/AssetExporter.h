#ifndef __ASSETEXPORTER_H__
#define __ASSETEXPORTER_H__

#include "BaseTypes.h"
#include "GlobalVariables.h"
#include "FileSystem.h"

WHITEBOX_BEGIN

class IAssetExporter
{
public:
	static bool		IsDigit( char c );
	static float	ToFloat( const char* str );
	static long int ToInt( const char* str );
	static bool		ReadWord( TFileHandle file, char* pDest );
	static void		SkipLine( TFileHandle file );
	
	virtual void	Export( const String& assetFolder, const String& resourceFolder, const String& filePath ) = 0;
};

WHITEBOX_END

#endif