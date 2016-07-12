#include "Render/ShaderProgramSerializer.h"
#include "Render/ShaderProgram.h"
#include "GlobalVariables.h"
#include "ResourceManager.h"

WHITEBOX_BEGIN

IResource*	CShaderProgramSerializer::Load( ISerializer& serializer, const CResourceDescriptor& descriptor )
{
	CShaderProgram* pShaderProgram = new CShaderProgram();

	if ( serializer.BeginGroup("Program") )
	{
		// Shaders
		while( serializer.BeginGroup("Shader") )
		{ 
			String shaderName;
			serializer.Value( "name", shaderName );

			pShaderProgram->AddShader( gVars->pResourceManager->GetResource< CShader >(shaderName) );

			serializer.EndGroup();
		}

		// Attributes
		while( serializer.BeginGroup("Attribute") )
		{
			String attributeName;
			serializer.Value( "name", attributeName );

			pShaderProgram->AddAttribute( attributeName );

			serializer.EndGroup();
		}

		serializer.EndGroup();
	}

	return pShaderProgram;
}

WHITEBOX_END
