#ifndef __ASSETMANAGER_H__
#define __ASSETMANAGER_H__

#include "BaseTypes.h"


WHITEBOX_BEGIN

class IAssetExporter;
class CAssetManager
{
public:
	CAssetManager();

	void	AddExporter(const String& extension, IAssetExporter* pExporter);
	IAssetExporter*	GetExporter(const String& extension);

	void	GenerateResourceListFile(const String& resourceFolder);
	void	Export(const String& assetFolder, const String& resourceFolder);

	private:
	typedef Map< String, IAssetExporter* >	TAssetExporterMap;
	TAssetExporterMap m_assetExporters;
};

WHITEBOX_END

#endif