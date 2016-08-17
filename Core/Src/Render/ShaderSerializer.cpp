#include "Render/ShaderSerializer.h"
#include "Render/Shader.h"
#include "GlobalVariables.h"
#include "LogSystem/LogSystem.h"

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


#ifdef COOK
	return new CShader( shaderType, nullptr );
#else
	void* pShaderId = gVars->pRenderer->CreateShader( (char const*)dataStream.GetCursorData(), shaderType );
	
	String errorMessage;
	if ( !gVars->pRenderer->CompileShader( pShaderId, errorMessage ) )
	{ 
		WbLog( "Default", "Shader compilation %s failed :\n%s\n", descriptor.GetPath().c_str(), errorMessage.c_str());
		return nullptr;
	}

	return new CShader( shaderType, pShaderId );
#endif
}

WHITEBOX_END
