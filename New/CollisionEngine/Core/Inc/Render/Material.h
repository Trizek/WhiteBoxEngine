#ifndef __MATERIAL_H__
#define __MATERIAL_H__

#include "Resource.h"
#include "Texture.h"

WHITEBOX_BEGIN

class CMaterial : public IResource
{
public:
	DECLARE_RESOURCE_TYPE( CMaterial );

	static const size_t	MAX_TEXTURE_LAYER = 8;
	
	class CTextureLayer
	{
	public:
		CTexturePtr	m_pTexture;
	};
	
	CMaterial();

	Vec4	m_emissiveColor;
	Vec4	m_ambientColor;
	Vec4	m_diffuseColor;
	Vec4	m_specularColor;
	float	m_shininess;
	

	CTextureLayer	m_textureLayers[ MAX_TEXTURE_LAYER ];
};

DECLARE_RESOURCE_POINTER( CMaterial );

WHITEBOX_END

#endif