#ifndef __SHADER_H__
#define __SHADER_H__

#include "ResourcePointer.h"
#include "Renderer.h"

WHITEBOX_BEGIN

class CShader: public IResource
{
public:
	DECLARE_RESOURCE_TYPE( CShader );

	CShader( EShaderType shaderType, void* pShaderId );
	~CShader();

	void*		GetShaderId() const;

	EShaderType	GetShaderType() const;

private:
	EShaderType	m_shaderType;
	void*		m_pShaderId;
};

DECLARE_RESOURCE_POINTER( CShader );

WHITEBOX_END

#endif