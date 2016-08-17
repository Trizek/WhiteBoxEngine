/************************************************************************************

Filename	:	VrCubeWorld_SurfaceView.c
Content		:	This sample uses a plain Android SurfaceView and handles all
Activity and Surface life cycle events in native code. This sample
does not use the application framework and also does not use LibOVRKernel.
This sample only uses the VrApi.
Created		:	March, 2015
Authors		:	J.M.P. van Waveren

Copyright	:	Copyright 2015 Oculus VR, LLC. All Rights reserved.

*************************************************************************************/

#ifdef __GEAR_VR

#include "System/OperatingSystem.h"	
#include "LogSystem/LogSystem.h"

#include <android/asset_manager.h>

extern AAssetManager* s_assetManager;

WHITEBOX_BEGIN


void	COperatingSystem::GetDataStream( const String& path, CDataStream& dataStream, size_t size2 /*= 0*/ )
{
	AAsset* asset = AAssetManager_open(s_assetManager, path.c_str(), AASSET_MODE_UNKNOWN);
	if (NULL == asset) {
		WbLog("D", "Asset %s in APK", path.c_str());
		return;
	}
	long size = AAsset_getLength(asset);
	char* assetData = new char[size+1];
	assetData[size] = '\0';

	AAsset_read(asset, assetData, size);
	AAsset_close(asset);


	dataStream = CDataStream( assetData, size );
}


WHITEBOX_END

#endif
