#include "Render/ShaderProgram.h"
#include "Render/Renderer.h"
#include "GlobalVariables.h"


WHITEBOX_BEGIN

CShaderProgram::CShaderProgram()
{
	m_pProgramId = gVars->pRenderer->CreateProgram();
}

CShaderProgram::~CShaderProgram()
{
	gVars->pRenderer->DestroyProgram( m_pProgramId );
}

bool	CShaderProgram::LinkProgram()
{
	for( CShaderPtr& shader : m_shaders )
	{
		if ( shader )
		{
			gVars->pRenderer->AttachShader( m_pProgramId, shader->GetShaderId() );
		}
	}

	for ( size_t i = 0; i < m_attributes.size(); ++i )
	{
		gVars->pRenderer->BindAttribute( m_pProgramId, m_attributes[ i ], i );
	}

	String errorMessage;
	bool bSuccess = gVars->pRenderer->LinkProgram( m_pProgramId, errorMessage );
	if ( !bSuccess )
	{
		printf( "Program link error : %s\n", errorMessage.c_str() );
	}

	return bSuccess;
}

void	CShaderProgram::Bind( bool bBind ) const
{
	gVars->pRenderer->BindProgram( (bBind)? m_pProgramId : nullptr );
}

void	CShaderProgram::AddShader( CShaderPtr shader )
{
	m_shaders.push_back( shader );
}

void	CShaderProgram::AddAttribute( const String& attribute )
{
	m_attributes.push_back( attribute );
}

void*	CShaderProgram::GetProgramId() const
{
	return m_pProgramId;
}

void	CShaderProgram::OnPostLoad()
{
	LinkProgram();
}

WHITEBOX_END