#ifndef __RENDERER_H__
#define __RENDERER_H__

#include "BaseTypes.h"
#include "Picture.h"
#include "VertexBuffer.h"
#include "RenderWindow.h"

WHITEBOX_BEGIN

enum EShaderType : int
{
	VS,
	PS,
};

class CRenderer
{
public:
	static const size_t MAX_TEXTURES_COUNT = 8;

	CRenderer();
	~CRenderer();

	// Frame
	void	StartRenderFrame();

	// Vertex Buffer
	void*	CreateVertexBuffer( size_t size, void const * data = NULL );
	void	DestroyVertexBuffer( void* pBufferId );
	
	void	BindVertexBuffer( void* pBufferId, size_t vertexCount, const CVertexFormat& vertexFormat );
	void	UnbindVertexBuffer();
	
	void*	LockVertexBuffer( void* pBufferId, size_t vertexCount, bool bRead, bool bWrite );
	void	UnlockVertexBuffer();
	
	
	// Index Buffer
	void*	CreateIndexBuffer( class CVertexBuffer* pVertexBuffer, size_t indexCount, uint const * pIndices /*= NULL*/ );
	void	DestroyIndexBuffer( void* pBufferId );
	
	uint*	LockIndexBuffer( void* pBufferId, size_t indexCount, bool bRead, bool bWrite );
	void	UnlockIndexBuffer();
	
	void	BindIndexBuffer( void* pBufferId, size_t indexCount );
	void	UnbindIndexBuffer();
	
	// Texture
	void*	CreateTexture( uint width, uint height, uint mipMapCount, CPicture::EPictureFormat format, void* pPixelData );
	void	DestroyTexture( void* pTextureId );
	
	void	BindTexture( void* pTextureId, size_t layer );
	void	UnbindTexture( size_t layer );


	// Shader
	void*	CreateShader( char const* code, EShaderType shaderType );
	bool	CompileShader( void* pShaderId, String& errorMessage );
	void	DestroyShader( void* pShaderId );

	void*	CreateProgram();
	void	AttachShader( void* pProgramId, void* pShaderId );
	void	DetachShader( void* pProgramId, void* pShaderId );
	void	BindAttribute( void* pProgramId, const String& attribute, uint index );
	bool	LinkProgram( void* pProgramId, String& errorMessage );
	void	DestroyProgram( void* pProgramId );
	void	BindProgram( void* pProgramId );

	// Uniforms
	int		GetUniformLocation( void* pProgramId, const String& name );

	void	SetUniformInt( void* pProgramId, int location, int number );
	void	SetUniformVec3( void* pProgramId, int location, const Vec3& vec );
	void	SetUniformMatrix44( void* pProgramId, int location, const Matrix44& matrix );

	void	SetUniformInt( void* pProgramId, const String& name, int number );
	void	SetUniformVec3( void* pProgramId, const String& name, const Vec3& vec );
	void	SetUniformMatrix44( void* pProgramId, const String& name, const Matrix44& matrix );

	// return the binding index
	int		BindUniformBlockToIndex( void* pProgramId, const String& name );
	void	BindUniformBuffer( void* pBufferId, int bindingIndex );
	void*	CreateUniformBuffer( size_t size );
	void	DestroyUniformBuffer( void* pBufferId );
	void*	LockUniformBuffer( void* pBufferId, size_t offset, size_t size );
	void	UnlockUniformBuffer();

	
	// Matrix
	void	ComputeProjectionMatrix( float near, float far, float w, float h, Matrix44& projMatrix );
	void	FormatTransformMatrix( const Matrix34& inMatrix, Matrix44& outMatrix );

	// Render state
	void	CullFace( bool bBackFace );

	// Font
	void*	LoadFont( const String& fontPath, uint size );
	void	UseFont( void* pFontId, uint size );
	void	RenderText( const String& text );

	
	// Render target
	CRenderWindow*		CreateRenderWindow( size_t x, size_t y, size_t width, size_t height );
	void				BindRenderWindow( CRenderWindow* pWindow );
	
	// Draw
	void	RenderBoundTriangles( size_t indexCount );
	
	// Debug
	void	DrawLine( const Vec3& from, const Vec3& to, const Color& color, const Matrix44& projectionMatrix);

	
private:
	void*	m_pSpecificData;
	std::vector< IRenderTargetPtr >	m_pRenderTargets;
};

WHITEBOX_END

#endif
