#ifndef __SHADERPROGRAM_H__
#define __SHADERPROGRAM_H__

#include "ResourcePointer.h"
#include "Renderer.h"
#include "Shader.h"

WHITEBOX_BEGIN

class CShaderProgram : public IResource
{
public:
	DECLARE_RESOURCE_TYPE( CShaderProgram );
		
	CShaderProgram();
	~CShaderProgram();

	void	AddShader( CShaderPtr shader );
	void	AddAttribute( const String& attribute );
	
	void*	GetProgramId() const;

	bool	LinkProgram();
	void	Bind( bool bBind ) const;

private:
	virtual void	OnPostLoad() override;

	void*						m_pProgramId;
	std::vector< CShaderPtr >	m_shaders;
	std::vector< String >		m_attributes;
};

DECLARE_RESOURCE_POINTER( CShaderProgram );

WHITEBOX_END

#endif