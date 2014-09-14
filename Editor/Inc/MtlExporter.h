#ifndef __MTLEXPORTER_H__
#define __MTLEXPORTER_H__

#include "AssetExporter.h"
#include "Render/MaterialHelper.h"

WHITEBOX_BEGIN

class CMtlExporter : public IAssetExporter
{
public:
	virtual void	Export( const String& assetFolder, const String& resourceFolder, const String& filePath );
};

WHITEBOX_END

#endif