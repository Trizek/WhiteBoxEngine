#ifndef __SHADERPROGRAM_H__
#define __SHADERPROGRAM_H__

#include "ResourcePointer.h"
#include "Renderer.h"
#include "Shader.h"

WHITEBOX_BEGIN

enum class EUniformType
{
	Int = 0,
	Vector3,
	Matrix4x4,
	Buffer,
};

struct SUniformInfo
{
	static const size_t	MAX_UNIFORM_COUNT = 8;
	static const size_t	MAX_UNIFORMS_SIZE = 512;

	EUniformType	type;
	int				location;
	int				binding;
	size_t			offset;
	size_t			size; // buffer only
};


class CShaderProgram : public IResource
{
public:
	DECLARE_RESOURCE_TYPE( CShaderProgram );
		
	CShaderProgram();
	~CShaderProgram();

	void	AddShader( CShaderPtr shader );
	void	AddAttribute( const String& attribute );
	
	void					AddUniformInfo( const String& name, EUniformType type, size_t size );
	size_t					GetUniformCount() const;
	const SUniformInfo&		GetUniformInfo( size_t index ) const;
	const SUniformInfo*		GetUniformInfo( const String& name ) const;
	int						GetUniformLocation( const String& name ) const;

	void*	GetProgramId() const;

	bool	LinkProgram();
	bool	LinkProgram( String& errorMessage );
	void	Bind( bool bBind ) const;

private:
	virtual void	OnPostLoad() override;

	void*						m_pProgramId;
	std::vector< CShaderPtr >	m_shaders;
	std::vector< String >		m_attributes;

	size_t						m_uniformCount;
	SUniformInfo				m_uniformInfos[ SUniformInfo::MAX_UNIFORM_COUNT ];
	Map< String, int >			m_uniformIndices;
	size_t						m_uniformsSize;
};

DECLARE_RESOURCE_POINTER( CShaderProgram );

WHITEBOX_END

#endif