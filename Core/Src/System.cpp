#include "System.h"

#include "FileSystem.h"
#include "Render/Renderer.h"

WHITEBOX_BEGIN

SSystem* gSystem = NULL;

void SSystem::Init()
{
	gSystem = new SSystem();
	
	gSystem->pFileSystem = new CFileSystem();
	gSystem->pRenderer = new CRenderer();
}

WHITEBOX_END
