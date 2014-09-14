#ifndef __SYSTEM_H__
#define __SYSTEM_H__

#include "BaseTypes.h"

WHITEBOX_BEGIN

class CFileSystem;
class CRenderer;
struct SSystem
{
	static void Init();

	CFileSystem* pFileSystem;
	CRenderer*	pRenderer;
};

extern SSystem* gSystem;

WHITEBOX_END

#endif
