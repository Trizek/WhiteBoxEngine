#ifdef __GEAR_VR

#include "System/GearVR/WhiteBoxGearLibrary.h"

#include "BaseTypes.h"
#include "GlobalVariables.h"

#define C_FUNCS_BEGIN extern "C"{
#define C_FUNCS_END }

class MaClasse
{
public:
	int MaFonction()
	{
		return 250;
	}
};

using namespace WhiteBox;

C_FUNCS_BEGIN

int	GetVal()
{
	MaClasse mc;
	return mc.MaFonction();
}

void WBGL_Init()
{
	SGlobalVariables::Init();
}

C_FUNCS_END

#endif