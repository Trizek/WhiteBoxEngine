#include "Render/MaterialSerializer.h"
#include "GlobalVariables.h"
#include "ResourceManager.h"

WHITEBOX_BEGIN

IResource*	CMaterialSerializer::Load( ISerializer& serializer, const CResourceDescriptor& descriptor )
{
	CMaterial* pMaterial = new CMaterial();
	
	if ( serializer.BeginGroup( "Material" ) )
	{
		serializer.Value( "emissiveColor", pMaterial->m_emissiveColor );
		serializer.Value( "ambientColor", pMaterial->m_ambientColor );
		serializer.Value( "diffuseColor", pMaterial->m_diffuseColor );
		serializer.Value( "specularColor", pMaterial->m_specularColor );
		serializer.Value( "shininess", pMaterial->m_shininess );
		
		size_t curTextLayer = 0;
		while( serializer.BeginGroup( "TextureLayer" ) )
		{
			String textureName;
			serializer.Value( "texture", textureName );
			pMaterial->m_textureLayers[ curTextLayer ].m_pTexture = gVars->pResourceManager->GetResource< CTexture >( textureName );
			
			++curTextLayer;
		
			serializer.EndGroup();
		}
		
		serializer.EndGroup();
	}
	
	return pMaterial;
}

WHITEBOX_END
