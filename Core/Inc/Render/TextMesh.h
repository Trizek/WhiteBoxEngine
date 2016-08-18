#ifndef __TEXTMESH_H__
#define __TEXTMESH_H__

#include "BaseTypes.h"
#include "Mesh.h"
#include "Font.h"

WHITEBOX_BEGIN

class CTextMesh
{
public:
	CTextMesh() = default;

	void			SetText( const CText& text, const CFontPtr& pFont );

	CMeshPtr	GetMesh() const;

private:
	void		CreateMesh( size_t characterCount );

	CMeshPtr	m_pMesh;
	size_t		m_characterCount;
	CFontPtr	m_pFont;
};


WHITEBOX_END

#endif