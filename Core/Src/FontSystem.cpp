#include "FontSystem.h"

#include <stdio.h>

#ifndef __GEAR_VR

WHITEBOX_BEGIN

CFontSystem::CFontSystem()
	: m_bInit(false)
{
	Init();
}

void	CFontSystem::Init()
{
	if ( !m_bInit )
	{
		m_bInit = FT_Init_FreeType( &m_library ) == 0;
	}
}

bool	CFontSystem::IsInit() const
{
	return m_bInit;
}

FT_Library&	CFontSystem::GetLibrary()
{
	return m_library;
}

WHITEBOX_END


#endif