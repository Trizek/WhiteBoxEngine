#ifndef __FBXEXPORTER_H__
#define __FBXEXPORTER_H__

#include "AssetExporter.h"

WHITEBOX_BEGIN

class CFbxExporter : public IAssetExporter
{
public:
	virtual void	Export( const String& assetFolder, const String& resourceFolder, const String& filePath );
};

WHITEBOX_END

#endif