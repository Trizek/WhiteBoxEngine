#include "Render/MaterialHelper.h"

#include "BaseTypes.h"
#include "Serializer.h"
#include "ScriptSerializer.h"

WHITEBOX_BEGIN

CMaterialHelper::CMaterialHelper()
	: m_shininess(0.0f)
{}

void CMaterialHelper::SaveToFile( const String& filePath )
{
	CScriptNodeWriter scriptNodeWriter;
	
	scriptNodeWriter.BeginGroup( "Material" );
	{
		//scriptNodeWriter.Value( "emissiveColor", m_emissiveColor );
		//scriptNodeWriter.Value( "ambientColor", m_ambientColor );
		//scriptNodeWriter.Value( "diffuseColor", m_diffuseColor );
		//scriptNodeWriter.Value( "specularColor", m_specularColor );
		//scriptNodeWriter.Value( "shininess", m_shininess );
		
		for( size_t iLayer = 0 ; iLayer < CMaterial::MAX_TEXTURE_LAYER ; ++iLayer )
		{
			if ( m_textureLayers[ iLayer ].m_textureName.empty() )
			{
				continue;
			}
		
			scriptNodeWriter.BeginGroup( "TextureLayer" );
			{
				scriptNodeWriter.Value( "texture", m_textureLayers[ iLayer ].m_textureName );
			}
			scriptNodeWriter.EndGroup();
		}
	}
	scriptNodeWriter.EndGroup();
	

	CScriptFileWriter scriptFileWriter( filePath.c_str() );
	scriptFileWriter.WriteNode( scriptNodeWriter.GetRootNode() );
}

WHITEBOX_END
