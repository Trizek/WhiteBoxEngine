#include "CopyExporter.h"

#include "FileSystem.h"

WHITEBOX_BEGIN

void CCopyExporter::Export( const String& assetFolder, const String& resourceFolder, const String& filePath )
{
// 	if ( gVars->pFileSystem->DoesFileExist( resourceFolder + filePath ) )
// 	{
// 		return;
// 	}
// 	

	gVars->pFileSystem->CreateFileDir( resourceFolder + filePath );
	gVars->pFileSystem->RemoveFile( resourceFolder + filePath );
	gVars->pFileSystem->CopyFileTo( assetFolder + filePath, resourceFolder + filePath );
}

WHITEBOX_END