#include "GlobalVariables.h"

#include "FileSystem.h"
#include "FontSystem.h"
#include "Render/Renderer.h"
#include "System/OperatingSystem.h"
#include "ResourceManager.h"
#include "Application.h"
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
	gVars->pApplication = new CApplication();
	gVars->pResourceManager = new CResourceManager();
	gVars->pPhysicsSystem = new CBulletPhysicsSystem();
	gVars->pBehaviorManager = new CBehaviorManager();
};


WHITEBOX_END

