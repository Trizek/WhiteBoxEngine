#ifndef __TEXTUREEXPORTER_H__
#define __TEXTUREEXPORTER_H__

#include "AssetExporter.h"

WHITEBOX_BEGIN

class CTextureExporter : public IAssetExporter
{
public:
	virtual void	Export( const String& assetFolder, const String& resourceFolder, const String& filePath );
};

WHITEBOX_END

#endif