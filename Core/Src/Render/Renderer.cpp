#include "Render/Renderer.h"

#include <OpenGL/gl.h> //OS x libs
#include <OpenGL/glu.h>

WHITEBOX_BEGIN

GLenum	GetOpenGLTextureLayer( size_t layer )
{
	switch( layer )
	{
		case 0:	return GL_TEXTURE0;
		case 1:	return GL_TEXTURE1;
		case 2:	return GL_TEXTURE2;
		case 3:	return GL_TEXTURE3;
		case 4:	return GL_TEXTURE4;
		case 5:	return GL_TEXTURE5;
		case 6:	return GL_TEXTURE6;
		case 7: return GL_TEXTURE7;
		default: return GL_TEXTURE0;
	}
}

class CSpecificData
{
public:
	CSpecificData()
		: m_boundVertexBuffer(0)
		, m_boundIndexBuffer(0)
	{
		for( size_t iTex = 0 ; iTex < CRenderer::MAX_TEXTURES_COUNT ; ++iTex )
		{
			m_boundTextures[ iTex ] = 0;
		}
	}

	GLuint	m_boundVertexBuffer;
	GLuint	m_boundIndexBuffer;
	GLuint	m_boundTextures[ CRenderer::MAX_TEXTURES_COUNT ];
};

CRenderer::CRenderer()
{
	m_pSpecificData = new CSpecificData();
}

CRenderer::~CRenderer()
{
	delete static_cast< CSpecificData* >( m_pSpecificData );
}

void*	CRenderer::CreateVertexBuffer( const size_t size, void const * pData /*= NULL*/ )
{
	GLuint* pBufferId = new GLuint(0);
	
	// Create buffer
	glGenBuffers( 1, pBufferId );
	
	// if pData is NULL, only allocate memory (for future write)
	glBindBuffer( GL_ARRAY_BUFFER, *pBufferId );
	glBufferData( GL_ARRAY_BUFFER, (GLsizeiptr)size, (const GLvoid*)pData, GL_STREAM_DRAW );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );
	
	return pBufferId;
}

void	CRenderer::DestroyVertexBuffer( void* pBufferId )
{
	if ( pBufferId != NULL )
	{
		glDeleteBuffers( 1, (GLuint*)pBufferId );
	
		delete (GLuint*)pBufferId;
	}
}

void	CRenderer::BindVertexBuffer( void* pBufferId, size_t vertexCount, const CVertexFormat& vertexFormat )
{
	glBindBuffer( GL_ARRAY_BUFFER, *((GLuint*)pBufferId) );
	static_cast< CSpecificData* >( m_pSpecificData )->m_boundVertexBuffer = *((GLuint*)pBufferId);

	// Position
	glEnableClientState( GL_VERTEX_ARRAY );
	glVertexPointer( 3, GL_FLOAT, vertexFormat.GetSize(), (void*)vertexFormat.GetSingleElementOffset( CVertexFormat::eSE_Position ) );

	// Normal
	if ( vertexFormat.HasSingleElement( CVertexFormat::eSE_Normal ) )
	{
		glEnableClientState( GL_NORMAL_ARRAY );
		glNormalPointer( GL_FLOAT, vertexFormat.GetSize(), (void*)vertexFormat.GetSingleElementOffset( CVertexFormat::eSE_Normal ) );
	}

	// Color
	if ( vertexFormat.HasSingleElement( CVertexFormat::eSE_Color ) )
	{
	//	glEnableClientState( GL_COLOR_ARRAY );
	//	glColorPointer( 3, GL_FLOAT, vertexFormat.GetSize(), (void*)vertexFormat.GetSingleElementOffset( CVertexFormat::eSE_Color ) );
	}
	
	// UV
	for( size_t iInstance = 0 ; iInstance < vertexFormat.GetMultipleElementCount( CVertexFormat::eME_UV ) ; ++iInstance )
	{
		glEnableClientState( GL_TEXTURE_COORD_ARRAY );
		glTexCoordPointer( 2, GL_FLOAT, vertexFormat.GetSize(), (void*)vertexFormat.GetMultipleElementOffset( CVertexFormat::eME_UV, iInstance ) );
	}
}

void	CRenderer::UnbindVertexBuffer()
{
	if ( static_cast< CSpecificData* >( m_pSpecificData )->m_boundVertexBuffer )
	{
		glDisableClientState( GL_COLOR_ARRAY );
		glDisableClientState( GL_NORMAL_ARRAY );
		glDisableClientState( GL_VERTEX_ARRAY );
		glDisableClientState( GL_TEXTURE_COORD_ARRAY );	
	
		glBindBuffer( GL_ARRAY_BUFFER, 0 );
		static_cast< CSpecificData* >( m_pSpecificData )->m_boundVertexBuffer = 0;
	}
}

void*	CRenderer::LockVertexBuffer( void* pBufferId, bool bRead, bool bWrite )
{
	GLenum access = (bRead && bWrite)? GL_READ_WRITE : ( (bWrite)? GL_WRITE_ONLY : GL_READ_ONLY );
	return glMapBuffer( GL_ARRAY_BUFFER, access );
}

void	CRenderer::UnlockVertexBuffer()
{
	glUnmapBuffer( GL_ARRAY_BUFFER );
}




void*	CRenderer::CreateIndexBuffer( const size_t indiceCount, uint const * pIndices /*= NULL*/ )
{
	GLuint* pBufferId = new GLuint(0);
	
	// Create buffer
	glGenBuffers( 1, pBufferId );
	
	// if pData is NULL, only allocate memory (for future write)
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, *pBufferId );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)(indiceCount * sizeof(uint)), (const GLvoid*)pIndices, GL_STREAM_DRAW );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
	
	return pBufferId;
}

void	CRenderer::DestroyIndexBuffer( void* pBufferId )
{
	if ( pBufferId != NULL )
	{
		glDeleteBuffers( 1, (GLuint*)pBufferId );
	
		delete (GLuint*)pBufferId;
	}
}

uint*	CRenderer::LockIndexBuffer( void* pBufferId, bool bRead, bool bWrite )
{
	GLenum access = (bRead && bWrite)? GL_READ_WRITE : ( (bWrite)? GL_WRITE_ONLY : GL_READ_ONLY );
	return (uint*)glMapBuffer( GL_ELEMENT_ARRAY_BUFFER, access );
}

void	CRenderer::UnlockIndexBuffer()
{
	glUnmapBuffer( GL_ARRAY_BUFFER );
}

void	CRenderer::BindIndexBuffer( void* pBufferId, size_t indexCount )
{
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, *((GLuint*)pBufferId) );
	static_cast< CSpecificData* >( m_pSpecificData )->m_boundIndexBuffer = *((GLuint*)pBufferId);
}

void	CRenderer::UnbindIndexBuffer()
{
	if ( static_cast< CSpecificData* >( m_pSpecificData )->m_boundIndexBuffer )
	{
		glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
		static_cast< CSpecificData* >( m_pSpecificData )->m_boundIndexBuffer = 0;
	}
}


void*	CRenderer::CreateTexture( uint width, uint height, CPicture::EPictureFormat format, void* pPixelData )
{
	GLenum glFormat;
	GLenum type;
	
	switch ( format )
	{
		case CPicture::ePF_R8G8B8:
			glFormat = GL_RGB;
			type = GL_UNSIGNED_BYTE;
			break;
		case CPicture::ePF_B8G8R8:
			glFormat = GL_BGR;
			type = GL_UNSIGNED_BYTE;
			break;
		case CPicture::ePF_R8G8B8A8:
			glFormat = GL_RGBA;
			type = GL_UNSIGNED_BYTE;
			break;	
		case CPicture::ePF_B8G8R8A8:
			glFormat = GL_BGRA;
			type = GL_UNSIGNED_BYTE;
			break;						
		default:
			return NULL;			
			break;
	}

	GLuint* pTextureId = new GLuint(0);
	
	glGenTextures(1, pTextureId );
	glBindTexture( GL_TEXTURE_2D, *pTextureId );
	glTexImage2D( GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, glFormat, type, pPixelData );
	 glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MIN_FILTER,GL_LINEAR); // Linear Filtering
    glTexParameteri(GL_TEXTURE_2D,GL_TEXTURE_MAG_FILTER,GL_LINEAR);	
glBindTexture( GL_TEXTURE_2D, 0 );

return pTextureId;	
	
	// Create buffer
	glGenTextures( 1, pTextureId );
/*	
	glPixelStorei(GL_UNPACK_ALIGNMENT,1);
	glPixelStorei(GL_PACK_ALIGNMENT,1);


		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);


	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);*/	
	
	// Set info (width, height, format) and eventually copy data
	glBindTexture( GL_TEXTURE_2D, *pTextureId );
	glTexImage2D( GL_TEXTURE_2D, 0, glFormat /*GL_RGB*/, width, height, 0, glFormat, type, pPixelData );		
	glBindTexture( GL_TEXTURE_2D, 0 );
	
	return pTextureId;
}

void	CRenderer::DestroyTexture( void* pTextureId )
{return;
	if ( pTextureId != NULL )
	{
		glDeleteTextures( 1, (GLuint*)pTextureId );
	
		delete (GLuint*)pTextureId;
	}
}
	
void	CRenderer::BindTexture( void* pTextureId, size_t layer )
{
	static_cast< CSpecificData* >( m_pSpecificData )->m_boundTextures[ layer ] = *((GLuint*)pTextureId);
	//glActiveTexture( GetOpenGLTextureLayer( layer ) );
//	glEnable( GL_TEXTURE_2D );
	glBindTexture( GL_TEXTURE_2D, *((GLuint*)pTextureId) );
}

void	CRenderer::UnbindTexture( size_t layer )
{
	//glActiveTexture( GetOpenGLTextureLayer( layer ) );
	glBindTexture( GL_TEXTURE_2D, 0 );	
	static_cast< CSpecificData* >( m_pSpecificData )->m_boundTextures[ layer ] = 0;
}

void	CRenderer::ComputeProjectionMatrix( float near, float far, float w, float h, Matrix44& projMatrix )
{
	projMatrix.a11 = w;
	projMatrix.a12 = 0.0f;
	projMatrix.a13 = 0.0f;
	projMatrix.a14 = 0.0f;
	
	projMatrix.a21 = 0.0f;
	projMatrix.a22 = h;
	projMatrix.a23 = 0.0f;
	projMatrix.a24 = 0.0f;	
	
	projMatrix.a31 = 0.0f;
	projMatrix.a32 = 0.0f;
	projMatrix.a33 = (near + far) / (near - far);
	projMatrix.a34 = -1.0f;
	
	projMatrix.a41 = 0.0f;
	projMatrix.a42 = 0.0f;
	projMatrix.a43 = (2.0f * near * far) / (near - far);
	projMatrix.a44 = 0.0f;			
}

void	CRenderer::SetProjectionMatrix( const Matrix44& projMatrix )
{
	glMatrixMode( GL_PROJECTION );
	
	GLfloat mat[ 16 ];
	mat[ 0 ] = projMatrix.a11;
	mat[ 1 ] = projMatrix.a12;
	mat[ 2 ] = projMatrix.a13;
	mat[ 3 ] = projMatrix.a14;
	mat[ 4 ] = projMatrix.a21;
	mat[ 5 ] = projMatrix.a22;
	mat[ 6 ] = projMatrix.a23;
	mat[ 7 ] = projMatrix.a24;
	mat[ 8 ] = projMatrix.a31;
	mat[ 9 ] = projMatrix.a32;
	mat[ 10 ] = projMatrix.a33;
	mat[ 11 ] = projMatrix.a34;
	mat[ 12 ] = projMatrix.a41;
	mat[ 13 ] = projMatrix.a42;
	mat[ 14 ] = projMatrix.a43;
	mat[ 15 ] = projMatrix.a44;
			
	glLoadMatrixf( mat );
}

CRenderWindow* CRenderer::CreateRenderWindow( size_t x, size_t y, size_t width, size_t height )
{
	CRenderWindow* pRenderWindow = new CRenderWindow( x, y, width, height );
	m_pRenderTargets.push_back( pRenderWindow );
	return pRenderWindow;
}

void CRenderer::BindRenderWindow( CRenderWindow* pWindow )
{
	glViewport( pWindow->GetX(), pWindow->GetY(), pWindow->GetWidth(), pWindow->GetHeight() );
}

void	CRenderer::RenderBoundTriangles( size_t indexCount )
{
	glDrawElements( GL_TRIANGLES, indexCount, GL_UNSIGNED_INT, (void*)0 );
	//glEnd();
	//glFlush();
}

void	CRenderer::DrawLine( const Vec3& from, const Vec3& to, const Color& color )
{
	glLineWidth( 2.5f ); 
	glColor3f( color.r, color.g, color.b );
	glBegin( GL_LINES );
	glVertex3f( from.x, from.y, from.z );
	glVertex3f( to.x, to.y, to.z );
	glEnd();	
}

// Draw
/*

glColorPointer(3, GL_UNSIGNED_BYTE, sizeof(SVertex), BUFFER_OFFSET(ColorOffset));
glVertexPointer(2, GL_FLOAT, sizeof(SVertex), BUFFER_OFFSET(VertexOffset));

glEnableClientState(GL_VERTEX_ARRAY);
glEnableClientState(GL_COLOR_ARRAY);

glDrawArrays(GL_TRIANGLES, 0, VertexCount);

glDisableClientState(GL_COLOR_ARRAY);
glDisableClientState(GL_VERTEX_ARRAY);

*/

WHITEBOX_END