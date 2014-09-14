#include "GlobalVariables.h"
/*
#include "Application.h"
#include "RenderAPI.h"
#include "PhysicsAPI.h"

#include "ObjectManager.h"
#include "BehaviorManager.h"
#include "InputManager.h"
#include "RigidBodyComponent.h"

*/
#include "ResourceManager.h"

WHITEBOX_BEGIN

SGlobalVariables* gVars = NULL;

void SGlobalVariables::Init()
{
	gVars = new SGlobalVariables();
/*
	gVars->pApplication = new CApplication();
	gVars->pRenderAPI = new SRenderAPI();
	gVars->pPhysicsAPI = new SPhysicsAPI();

	gVars->pObjectManager = new CObjectManager();
	gVars->pBehaviorManager = new CBehaviorManager();
	gVars->pInputManager = new CInputManager();
	gVars->pRigidBodyShapeFactory = new SRigidBodyShapeFactory();
	
	*/
	gVars->pResourceManager = new CResourceManager();
};



WHITEBOX_END

