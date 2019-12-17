#include "GlobalVariables.h"

#include "FileSystem.h"
#include "FontSystem.h"
#include "Render/Renderer.h"
#include "System/OperatingSystem.h"
//#include "ResourceLoadingThread.h"
#include "ResourceManager.h"
#include "Engine.h"
#include "LogSystem/LogSystem.h"
#include "Physics/BulletPhysicsSystem.h"
#include "BehaviorManager.h"

WHITEBOX_BEGIN

SGlobalVariables* gVars = NULL;

void SGlobalVariables::Init()
{
	gVars = new SGlobalVariables();

	gVars->pFileSystem = new CFileSystem();
	gVars->pLogSystem = new CLogSystem();
	gVars->pFontSystem = new CFontSystem();
	gVars->pRenderer = new CRenderer();
	gVars->pOperatingSystem = new COperatingSystem();
	gVars->pEngine = new CEngine();
	gVars->pResourceManager = new CResourceManager();
	//gVars->pResourceLoadingThread = new CResourceLoadingThread();
	gVars->pPhysicsSystem = new CBulletPhysicsSystem();
	gVars->pBehaviorManager = new CBehaviorManager();
};


WHITEBOX_END

