#ifndef __MATERIALHELPER_H__
#define __MATERIALHELPER_H__

#include "BaseTypes.h"
#include "Render/Material.h"

WHITEBOX_BEGIN

class CMaterialHelper
{
public:
	class CTextureLayerHelper
	{
	public:
		CTextureLayerHelper(){}
		CTextureLayerHelper( const String& textureName )
			: m_textureName(textureName){}
	
		String	m_textureName;
	};

	CMaterialHelper();
	
	void SaveToFile( const String& filePath );

public:
	Vec4	m_emissiveColor;
	Vec4	m_ambientColor;
	Vec4	m_diffuseColor;
	Vec4	m_specularColor;
	float	m_shininess;
	
	CTextureLayerHelper	m_textureLayers[ CMaterial::MAX_TEXTURE_LAYER ];
};

WHITEBOX_END

#endif