#ifndef __COPYEXPORTER_H__
#define __COPYEXPORTER_H__

#include "AssetExporter.h"

WHITEBOX_BEGIN

class CCopyExporter : public IAssetExporter
{
public:
	virtual void	Export( const String& assetFolder, const String& resourceFolder, const String& filePath );
};

WHITEBOX_END

#endif