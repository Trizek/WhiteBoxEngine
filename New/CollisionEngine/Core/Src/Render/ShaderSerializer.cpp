#include "Render/ShaderSerializer.h"
#include "Render/Shader.h"
#include "GlobalVariables.h"

WHITEBOX_BEGIN

IResource*	CShaderSerializer::Load( CDataStream& dataStream, const CResourceDescriptor& descriptor )
{
	EShaderType shaderType = EShaderType::VS;
	if ( strcmp( descriptor.GetExtension().c_str(), "vs" ) == 0 )
	{ 
		shaderType = EShaderType::VS;
	}
	else if ( strcmp( descriptor.GetExtension().c_str(), "ps" ) == 0 )
	{ 
		shaderType = EShaderType::PS;
	}

	void* pShaderId = gVars->pRenderer->CreateShader( (char const*)dataStream.GetCursorData(), shaderType );
	
	String errorMessage;
	if ( !gVars->pRenderer->CompileShader( pShaderId, errorMessage ) )
	{ 
		printf("Shader compilation %s failed :\n%s\n", descriptor.GetPath().c_str(), errorMessage.c_str());
		return nullptr;
	}

	return new CShader( shaderType, pShaderId );
}

WHITEBOX_END