#ifndef __OBJEXPORTER_H__
#define __OBJEXPORTER_H__

#include "AssetExporter.h"

WHITEBOX_BEGIN

class CObjExporter : public IAssetExporter
{
public:
	virtual void	Export( const String& assetFolder, const String& resourceFolder, const String& filePath );
};

WHITEBOX_END

#endif