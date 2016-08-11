#include "GlobalVariables.h"

#include "FileSystem.h"
#include "FontSystem.h"
#include "Render/Renderer.h"
#include "System/OperatingSystem.h"
#include "ResourceManager.h"
#include "Application.h"

WHITEBOX_BEGIN

SGlobalVariables* gVars = NULL;

void SGlobalVariables::Init()
{
	gVars = new SGlobalVariables();

	gVars->pFileSystem = new CFileSystem();
	gVars->pFontSystem = new CFontSystem();
	gVars->pRenderer = new CRenderer();
	gVars->pOperatingSystem = new COperatingSystem();
	gVars->pApplication = new CApplication();
	gVars->pResourceManager = new CResourceManager();
};


template< class T >
CString ToString(const T& val)
{
	return CString("{not supported type}");
}

template<>
CString ToString< int >(const int& val)
{
	char buffer[256];
	sprintf_s(buffer, 256, "%d", val);
	return CString(buffer);
}

template<>
CString ToString< float >(const float& val)
{
	char buffer[256];
	sprintf_s(buffer, 256, "%.2f", val);
	return CString(buffer);
}

WHITEBOX_END

