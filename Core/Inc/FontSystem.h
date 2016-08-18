#ifndef __FONTSYSTEM_H__
#define __FONTSYSTEM_H__

#include "BaseTypes.h"


#include <ft2build.h>
#include FT_FREETYPE_H


WHITEBOX_BEGIN

class CFontSystem
{
public:
	CFontSystem();

	void		Init();
	bool		IsInit() const;
	FT_Library&	GetLibrary();

private:
	FT_Library	m_library;
	bool		m_bInit;
};



WHITEBOX_END

#endif

