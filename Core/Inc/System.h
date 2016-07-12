#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include "BaseTypes.h"

WHITEBOX_BEGIN

class CFileSystem;
class CRenderer;
class COperatingSystem;
struct SSystem
{
	static void Init();

	CFileSystem* pFileSystem;
	CRenderer*	pRenderer;
	COperatingSystem*	pOperatingSystem;
};

extern SSystem* gSystem;

WHITEBOX_END

#endif
