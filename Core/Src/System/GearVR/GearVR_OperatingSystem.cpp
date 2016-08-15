#ifdef __GEAR_VR

#include "System/OperatingSystem.h"



#include "GlobalVariables.h"
#include "Render/Renderer.h"
#include "Application.h"

WHITEBOX_BEGIN


COperatingSystem::COperatingSystem() {}
COperatingSystem::~COperatingSystem() {}

void	COperatingSystem::Init(uint width, uint height) {}
void	COperatingSystem::Reshape(uint width, uint height) {}

Vec2	COperatingSystem::GetMousePos() { return Vec2(); }
bool	COperatingSystem::GetMouseButton(int button) { return false; }
bool	COperatingSystem::IsPressingKey(Key key) { return false; }
bool	COperatingSystem::JustPressedKey(Key key) { return false; }

float	COperatingSystem::Tick()
{
	return 1.0f / 60.0f;
}

WHITEBOX_END

#endif