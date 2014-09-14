#ifndef __GLOBALVARIABLES_H__
#define __GLOBALVARIABLES_H__

#include "BaseTypes.h"

WHITEBOX_BEGIN

/*
class CApplication;
struct SRenderAPI;
struct SPhysicsAPI;
class CObjectManager;
class CBehaviorManager;
class CInputManager;
struct SRigidBodyShapeFactory;*/

class CResourceManager;
struct SGlobalVariables
{
	static void Init();

/*	CApplication* pApplication;
	SRenderAPI*	pRenderAPI;
	SPhysicsAPI* pPhysicsAPI;
	CObjectManager* pObjectManager;
	CBehaviorManager* pBehaviorManager;
	CInputManager* pInputManager;
	SRigidBodyShapeFactory* pRigidBodyShapeFactory;*/
	
	CResourceManager* pResourceManager;
};

extern SGlobalVariables* gVars;

WHITEBOX_END

#endif
