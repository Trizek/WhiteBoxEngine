#include "Render/Shader.h"

#include "GlobalVariables.h"
#include "Render/Renderer.h"

WHITEBOX_BEGIN


CShader::CShader( EShaderType shaderType, void* pShaderId )
	: m_shaderType(shaderType)
	, m_pShaderId(pShaderId)
{}

CShader::~CShader()
{
	if ( m_pShaderId != nullptr )
	{ 
		gVars->pRenderer->DestroyShader( m_pShaderId );
	}
}

void*		CShader::GetShaderId() const
{
	return m_pShaderId;
}


EShaderType	CShader::GetShaderType() const
{
	return m_shaderType;
}

WHITEBOX_END