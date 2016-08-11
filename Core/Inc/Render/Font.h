#ifndef __FONT_H__
#define __FONT_H__

#include "ResourcePointer.h"
#include "Picture.h"
#include "Material.h"
#include "Texture.h"

WHITEBOX_BEGIN

class CFont : public IResource
{
public:
	struct SCharacter
	{
		SCharacter() = default;
		SCharacter(const Vec2& _pos, const Vec2& _corner, const Vec2& _size, float _advance)
			: pos(_pos)
			, corner(_corner)
			, size(_size)
			, advance(_advance) {}

		Vec2	pos;
		Vec2	corner;
		Vec2	size;
		float	advance;
	};

	DECLARE_RESOURCE_TYPE(CFont);

	CFont( CDataStream& dataStream, uint size = 16, uint dpi = 300 );

	const SCharacter&			GetCharater( wchar unicode ) const;
	const Vec2&					GetUVScale() const;
	CMaterialPtr				GetMaterial() const;
	

private:
	std::vector< SCharacter >	m_characters;
	Vec2						m_uvScale;
	CMaterialPtr				m_pMaterial;
};

DECLARE_RESOURCE_POINTER(CFont);

WHITEBOX_END

#endif