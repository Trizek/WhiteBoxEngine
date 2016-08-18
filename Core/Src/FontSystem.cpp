#include "FontSystem.h"

#include <stdio.h>


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
		WbLog( "Font", "Freetype init : %d", (int)m_bInit );
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

