#include "Render/ShaderProgram.h"
#include "Render/Renderer.h"
#include "GlobalVariables.h"
#include "LogSystem/LogSystem.h"


size_t	s_uniformSize[] =
{
	sizeof(int),
	sizeof(WhiteBox::Vec3),
	sizeof(WhiteBox::Matrix44),
	sizeof(void*), // hold the pointer for the uniform buffer
};

WHITEBOX_BEGIN

CShaderProgram::CShaderProgram()
	: m_uniformCount(0)
	, m_uniformsSize(0)
{
	m_pProgramId = gVars->pRenderer->CreateProgram();
}

CShaderProgram::~CShaderProgram()
{
	gVars->pRenderer->DestroyProgram( m_pProgramId );
}

bool	CShaderProgram::LinkProgram()
{
	String errorMessage;
	return LinkProgram( errorMessage );
}


bool	CShaderProgram::LinkProgram( String& errorMessage )
{
	Bind( true );

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
	
	bool bSuccess = gVars->pRenderer->LinkProgram( m_pProgramId, errorMessage );
	if ( !bSuccess )
	{
		WbLog( "Default",  "Program link error : %s\n", errorMessage.c_str() );
		return bSuccess;
	}

	for ( auto it = m_uniformIndices.GetConstIterator(); it.IsValid(); it.MoveNext() )
	{
		SUniformInfo& uniformInfo = m_uniformInfos[ it.GetValue() ];
		if ( uniformInfo.type == EUniformType::Buffer )
		{
			uniformInfo.binding = gVars->pRenderer->BindUniformBlockToIndex( m_pProgramId, it.GetKey() );
			uniformInfo.location = uniformInfo.binding;
		}
		else
		{
			uniformInfo.location = gVars->pRenderer->GetUniformLocation( m_pProgramId, it.GetKey() );
			uniformInfo.binding = uniformInfo.location;
		}
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

void	CShaderProgram::AddUniformInfo( const String& name, EUniformType type, size_t size )
{
	m_uniformIndices[ name ] = m_uniformCount;
	SUniformInfo& uniformInfo = m_uniformInfos[ m_uniformCount++ ];
	uniformInfo.type = type;
	uniformInfo.offset = m_uniformsSize;
	uniformInfo.size = ( type == EUniformType::Buffer )? size : 0;
	m_uniformsSize += s_uniformSize[ (size_t)type ];
}

size_t	CShaderProgram::GetUniformCount() const
{
	return m_uniformCount;
}

const SUniformInfo&		CShaderProgram::GetUniformInfo( size_t index ) const
{
	return m_uniformInfos[ index ];
}


const SUniformInfo*		CShaderProgram::GetUniformInfo( const String& name ) const
{
	const int* findRes = m_uniformIndices.FindElement( name );
	return ( findRes == nullptr )? nullptr : &m_uniformInfos[ *findRes ];
}

int						CShaderProgram::GetUniformLocation( const String& name ) const
{
	const SUniformInfo*	pUniformInfo = GetUniformInfo( name );
	return ( pUniformInfo == nullptr )? -1 : pUniformInfo->location;
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
