
#include "GlobalVariables.h"
#include "System/OperatingSystem.h"
#include "LogSystem/LogSystem.h"
#include "AssetManager.h"

#include "ThreadDoubleBuffer.h"

using namespace WhiteBox;

int main(int argc, char **argv)
{
	SGlobalVariables::Init();


#ifdef COOK
	WbLog( "Cook", "WhiteBox cook command utilities" );
	if ( argc < 3 )
	{
		WbLog( "Cook", "Syntax : %s <in asset path> <out resource paths>", argv[0] );
		return 0;
	}

	String assetPath( argv[1] );
	String resourcePath( argv[2] );

	WbLog( "Cook", "Exporting assets from %s to %s", assetPath.c_str(), resourcePath.c_str() );

	CAssetManager assetManager;
	assetManager.Export( assetPath, resourcePath );
#else

	WhiteBox::gVars->pOperatingSystem->Init( 1600, 800 );

#endif
	
}

