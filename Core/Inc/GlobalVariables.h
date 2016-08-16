#ifndef __GLOBALVARIABLES_H__
#define __GLOBALVARIABLES_H__

#include "BaseTypes.h"



WHITEBOX_BEGIN


class CFileSystem;
class CFontSystem;
class CRenderer;
class COperatingSystem;
class CApplication;
class CResourceManager;
class CLogSystem;
struct SGlobalVariables
{
	static void Init();

	CFileSystem*		pFileSystem;
	CLogSystem*			pLogSystem;
	CFontSystem*		pFontSystem;
	CRenderer*			pRenderer;
	COperatingSystem*	pOperatingSystem;
	CApplication*		pApplication;
	CResourceManager*	pResourceManager;
};

extern SGlobalVariables* gVars;

WHITEBOX_END

#endif
