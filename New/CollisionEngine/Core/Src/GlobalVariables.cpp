#include "GlobalVariables.h"

#include "FileSystem.h"
#include "Render/Renderer.h"
#include "System/OperatingSystem.h"
#include "ResourceManager.h"
#include "Application.h"

WHITEBOX_BEGIN

SGlobalVariables* gVars = NULL;

void SGlobalVariables::Init()
{
	gVars = new SGlobalVariables();

	gVars->pFileSystem = new CFileSystem();
	gVars->pRenderer = new CRenderer();
	gVars->pOperatingSystem = new COperatingSystem();
	gVars->pApplication = new CApplication();
	gVars->pResourceManager = new CResourceManager();
};



WHITEBOX_END

