#ifndef __GLOBALVARIABLES_H__
#define __GLOBALVARIABLES_H__

#include "BaseTypes.h"



WHITEBOX_BEGIN


class CFileSystem;
class CFontSystem;
class CRenderer;
class COperatingSystem;
class CEngine;
class CResourceManager;
class CLogSystem;
class IPhysicsSystem;
class CBehaviorManager;
class CResourceLoadingThread;
struct SGlobalVariables
{
	static void Init();

	CFileSystem*		pFileSystem;
	CLogSystem*			pLogSystem;
	CFontSystem*		pFontSystem;
	CRenderer*			pRenderer;
	COperatingSystem*	pOperatingSystem;
	CEngine*			pEngine;
	CResourceManager*	pResourceManager;
	CResourceLoadingThread*	pResourceLoadingThread;
	IPhysicsSystem*		pPhysicsSystem;
	CBehaviorManager*	pBehaviorManager;
};

extern SGlobalVariables* gVars;

WHITEBOX_END

#endif
