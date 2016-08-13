#ifdef __GEAR_VR

#include "System/GearVR/WhiteBoxGearLibrary.h"

#include "BaseTypes.h"

class MaClasse
{
public:
	int MaFonction()
	{
		return 1500;
	}
};

extern "C"
{
	int	GetVal()
	{
		MaClasse mc;
		return mc.MaFonction();
	}
}

#endif