#include "Render/ShaderProgramSerializer.h"
#include "Render/ShaderProgram.h"
#include "GlobalVariables.h"
#include "ResourceManager.h"
#include "System/OperatingSystem.h"


const char*	s_shaderPlatforms[] = {
	"Desktop",
	"GearVR",
};

WHITEBOX_BEGIN

IResource*	CShaderProgramSerializer::Load( ISerializer& serializer, const CResourceDescriptor& descriptor )
{
	const char* curShaderPlatform = s_shaderPlatforms[ (int)gVars->pOperatingSystem->GetShaderPlatform() ];

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

		// Uniform
		while ( serializer.BeginGroup("Uniform") )
		{
			int	size = 0;

			String shaderPlatform;
			if ( serializer.Value( "platform", shaderPlatform ) && shaderPlatform != curShaderPlatform )
			{
				serializer.EndGroup();
				continue;
			}


			String uniformName;
			serializer.Value( "name", uniformName );

			String uniformTypeStr;
			serializer.Value( "type", uniformTypeStr );

			EUniformType uniformType = EUniformType::Int;
			if ( uniformTypeStr == "Vector3" )
			{
				uniformType = EUniformType::Vector3;
			}
			else if ( uniformTypeStr == "Matrix4x4" )
			{ 
				uniformType = EUniformType::Matrix4x4;
			}
			else if ( uniformTypeStr == "Buffer" )
			{
				uniformType = EUniformType::Buffer;
				
				serializer.Value( "size", size );
			}

			pShaderProgram->AddUniformInfo( uniformName, uniformType, size );

			serializer.EndGroup();
		}

		serializer.EndGroup();
	}

	return pShaderProgram;
}

WHITEBOX_END
