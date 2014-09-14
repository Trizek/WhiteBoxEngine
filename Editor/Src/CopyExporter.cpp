#include "CopyExporter.h"

#include "FileSystem.h"

WHITEBOX_BEGIN

void CCopyExporter::Export( const String& assetFolder, const String& resourceFolder, const String& filePath )
{
	gSystem->pFileSystem->CreateFileDir( resourceFolder + filePath );
	gSystem->pFileSystem->CopyFile( assetFolder + filePath, resourceFolder + filePath );
}

WHITEBOX_END