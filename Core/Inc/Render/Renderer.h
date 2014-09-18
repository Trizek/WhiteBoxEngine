#ifndef __RENDERER_H__
#define __RENDERER_H__

#include "BaseTypes.h"
#include "Picture.h"
#include "VertexBuffer.h"
#include "RenderWindow.h"

WHITEBOX_BEGIN

class CRenderer
{
public:
	static const size_t MAX_TEXTURES_COUNT = 8;

	CRenderer();
	~CRenderer();

	// Vertex Buffer
	void*	CreateVertexBuffer( size_t size, void const * data = NULL );
	void	DestroyVertexBuffer( void* pBufferId );
	
	void	BindVertexBuffer( void* pBufferId, size_t vertexCount, const CVertexFormat& vertexFormat );
	void	UnbindVertexBuffer();
	
	void*	LockVertexBuffer( void* pBufferId, bool bRead, bool bWrite );
	void	UnlockVertexBuffer();
	
	
	// Index Buffer
	void*	CreateIndexBuffer( size_t indexCount, uint const * pIndices /*= NULL*/ );
	void	DestroyIndexBuffer( void* pBufferId );
	
	uint*	LockIndexBuffer( void* pBufferId, bool bRead, bool bWrite );
	void	UnlockIndexBuffer();
	
	void	BindIndexBuffer( void* pBufferId, size_t indexCount );
	void	UnbindIndexBuffer();
	
	// Texture
	void*	CreateTexture( uint width, uint height, CPicture::EPictureFormat format, void* pPixelData );
	void	DestroyTexture( void* pTextureId );
	
	void	BindTexture( void* pTextureId, size_t layer );
	void	UnbindTexture( size_t layer );
	
	// Matrix
	void	ComputeProjectionMatrix( float near, float far, float w, float h, Matrix44& projMatrix );
	void	SetProjectionMatrix( const Matrix44& matrix );
	
	// Render target
	CRenderWindow*		CreateRenderWindow( size_t x, size_t y, size_t width, size_t height );
	void				BindRenderWindow( CRenderWindow* pWindow );
	
	// Draw
	void	RenderBoundTriangles( size_t indexCount );
	
	// Debug
	void	DrawLine( const Vec3& from, const Vec3& to, const Color& color );
	
private:
	void*	m_pSpecificData;
	std::vector< IRenderTargetPtr >	m_pRenderTargets;
};

WHITEBOX_END

#endif
