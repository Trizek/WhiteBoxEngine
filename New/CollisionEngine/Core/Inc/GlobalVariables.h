#ifndef __GLOBALVARIABLES_H__
#define __GLOBALVARIABLES_H__

#include "BaseTypes.h"



WHITEBOX_BEGIN


class CFileSystem;
<<<<<<< HEAD
class CFontSystem;
=======
>>>>>>> master
class CRenderer;
class COperatingSystem;
class CApplication;
class CResourceManager;
struct SGlobalVariables
{
	static void Init();

	CFileSystem* pFileSystem;
<<<<<<< HEAD
	CFontSystem*	pFontSystem;
=======
>>>>>>> master
	CRenderer*	pRenderer;
	COperatingSystem*	pOperatingSystem;
	CApplication*	pApplication;
	CResourceManager* pResourceManager;
};

extern SGlobalVariables* gVars;

WHITEBOX_END

#endif
