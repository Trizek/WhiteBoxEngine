#include "Render/Renderer.h"

//#include <OpenGL/gl.h> //OS x libs
//#include <OpenGL/glu.h>

#ifdef __GEAR_VR
#include <EGL/egl.h>
#include <EGL/eglext.h>
#include <GLES3/gl3.h>
#include <GLES3/gl3ext.h>
#else
#include <GL/glew.h>
#endif

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

struct SIndexBufferId
{
	GLuint	vertexArrayId;
	GLuint	indexBufferId;
};

class CSpecificData
{
public:
	CSpecificData()
		: m_boundVertexBuffer(0)
		, m_boundIndexBuffer(nullptr)
	{
		for( size_t iTex = 0 ; iTex < CRenderer::MAX_TEXTURES_COUNT ; ++iTex )
		{
			m_boundTextures[ iTex ] = 0;
		}
	}



	GLuint	m_boundVertexBuffer;
	SIndexBufferId*	m_boundIndexBuffer;
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

void	CRenderer::StartRenderFrame()
{
	glClearColor(1.0f, 1.0f, 1.0f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Enable Texture Mapping ( NEW )
#ifndef __GEAR_VR
	glShadeModel(GL_SMOOTH);									// Enable Smooth Shading
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);          // Really Nice Perspectiv	
#endif
	glClearDepthf(1.0f);											// Depth Buffer Setup
	glEnable(GL_DEPTH_TEST);									// Enables Depth Testing
	glDepthFunc(GL_LEQUAL);										// The Type Of Depth Testing To Do


	glEnable(GL_TEXTURE_2D);
	glEnable(GL_CULL_FACE);
}

void*	CRenderer::CreateVertexBuffer( const size_t size, void const * pData /*= NULL*/ )
{
	GLuint* pBufferId = new GLuint(0);

	// Create buffer
	glGenBuffers( 1, pBufferId );
	
	// if pData is NULL, only allocate memory (for future write)
	glBindBuffer( GL_ARRAY_BUFFER, *pBufferId );
	glBufferData( GL_ARRAY_BUFFER, (GLsizeiptr)size, (const GLvoid*)pData, GL_STATIC_DRAW );
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
	GLuint attribIndex = 0;
	glEnableVertexAttribArray( attribIndex );
	glVertexAttribPointer( 0, 3, GL_FLOAT, false, (GLsizei)vertexFormat.GetSize(), 0 );
	++attribIndex;

	// Normal
	if ( vertexFormat.HasSingleElement( CVertexFormat::eSE_Normal ) )
	{
		glEnableVertexAttribArray( attribIndex );
		glVertexAttribPointer( attribIndex, 3, GL_FLOAT, false, (GLsizei)vertexFormat.GetSize(), (void*)vertexFormat.GetSingleElementOffset( CVertexFormat::eSE_Normal ) );
		++attribIndex;
	}

	// Color
	if  (vertexFormat.HasSingleElement( CVertexFormat::eSE_Color ))
	{
		glEnableVertexAttribArray( attribIndex );
		glVertexAttribPointer( attribIndex, 3, GL_FLOAT, false, (GLsizei)vertexFormat.GetSize(), (void*)vertexFormat.GetSingleElementOffset( CVertexFormat::eSE_Color ) );
		++attribIndex;
	}

	// UV
	for ( size_t iInstance = 0; iInstance < vertexFormat.GetMultipleElementCount(CVertexFormat::eME_UV); ++iInstance )
	{
		glEnableVertexAttribArray( attribIndex );
		glVertexAttribPointer( attribIndex, 2, GL_FLOAT, false, (GLsizei)vertexFormat.GetSize(), (void*)vertexFormat.GetMultipleElementOffset( CVertexFormat::eME_UV, iInstance ) );
		++attribIndex;
	}

#if defined(OLD_OPENGL)
	// Position
	glEnableClientState( GL_VERTEX_ARRAY );
	glVertexPointer( 3, GL_FLOAT, (GLsizei)vertexFormat.GetSize(), (void*)vertexFormat.GetSingleElementOffset( CVertexFormat::eSE_Position ) );

	// Normal
	if ( vertexFormat.HasSingleElement( CVertexFormat::eSE_Normal ) )
	{
		glEnableClientState( GL_NORMAL_ARRAY );
		glNormalPointer( GL_FLOAT, (GLsizei)vertexFormat.GetSize(), (void*)vertexFormat.GetSingleElementOffset( CVertexFormat::eSE_Normal ) );
	}

	// Color
	if ( vertexFormat.HasSingleElement( CVertexFormat::eSE_Color ) )
	{
		glEnableClientState( GL_COLOR_ARRAY );
		glColorPointer( 3, GL_FLOAT, vertexFormat.GetSize(), (void*)vertexFormat.GetSingleElementOffset( CVertexFormat::eSE_Color ) );
	}
	
	// UV
	for( size_t iInstance = 0 ; iInstance < vertexFormat.GetMultipleElementCount( CVertexFormat::eME_UV ) ; ++iInstance )
	{
		glEnableClientState( GL_TEXTURE_COORD_ARRAY );
		glTexCoordPointer( 2, GL_FLOAT, (GLsizei)vertexFormat.GetSize(), (void*)vertexFormat.GetMultipleElementOffset( CVertexFormat::eME_UV, iInstance ) );
	}
#endif
}

void	CRenderer::UnbindVertexBuffer()
{
	if ( static_cast< CSpecificData* >( m_pSpecificData )->m_boundVertexBuffer )
	{
// 		glDisableClientState( GL_COLOR_ARRAY );
// 		glDisableClientState( GL_NORMAL_ARRAY );
// 		glDisableClientState( GL_VERTEX_ARRAY );
// 		glDisableClientState( GL_TEXTURE_COORD_ARRAY );	
	
		glBindBuffer( GL_ARRAY_BUFFER, 0 );
		static_cast< CSpecificData* >( m_pSpecificData )->m_boundVertexBuffer = 0;
	}
}

void*	CRenderer::LockVertexBuffer( void* pBufferId, size_t vertexCount, bool bRead, bool bWrite )
{
	glBindBuffer( GL_ARRAY_BUFFER, *static_cast<GLuint*>(pBufferId) );

#ifndef __GEAR_VR
	GLenum access = (bRead && bWrite)? GL_READ_WRITE : ( (bWrite)? GL_WRITE_ONLY : GL_READ_ONLY );
	return glMapBuffer( GL_ARRAY_BUFFER, access );
#else
	GLbitfield access = 0;
	if ( bRead )	access |= GL_MAP_READ_BIT;
	if ( bWrite )	access |= GL_MAP_WRITE_BIT;

	return glMapBufferRange( GL_ARRAY_BUFFER, 0, vertexCount, access );
#endif
}

void	CRenderer::UnlockVertexBuffer()
{
	glUnmapBuffer( GL_ARRAY_BUFFER );
}




void*	CRenderer::CreateIndexBuffer( CVertexBuffer* pVertexBuffer, const size_t indiceCount, uint const * pIndices /*= NULL*/ )
{
	SIndexBufferId* pBufferId = new SIndexBufferId();

	// Create Vertex Array Object (VAO)
	glGenVertexArrays( 1, &(pBufferId->vertexArrayId) );
	glBindVertexArray( pBufferId->vertexArrayId );
	
	// Bind vertex buffer (VBO) to VAO
	BindVertexBuffer( pVertexBuffer->GetBufferId(), pVertexBuffer->GetVertexCount(), pVertexBuffer->GetVertexFormat() );

	// Create Index buffer
	glGenBuffers( 1, &(pBufferId->indexBufferId) );
	
	// Bind index buffer to VAO
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, pBufferId->indexBufferId );
	glBufferData( GL_ELEMENT_ARRAY_BUFFER, (GLsizeiptr)(indiceCount * sizeof(uint)), (const GLvoid*)pIndices, GL_STATIC_DRAW );

	// Unbind buffers
	glBindVertexArray( 0 );
	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, 0 );
	glBindBuffer( GL_ARRAY_BUFFER, 0 );

	return pBufferId;
}

void	CRenderer::DestroyIndexBuffer( void* pBufferId )
{
	if ( pBufferId != NULL )
	{
		glDeleteBuffers( 1, &(((SIndexBufferId*)pBufferId)->indexBufferId) );
		glDeleteVertexArrays( 1, &(((SIndexBufferId*)pBufferId)->vertexArrayId) );
	
		delete (SIndexBufferId*)pBufferId;
	}
}

uint*	CRenderer::LockIndexBuffer( void* pBufferId, size_t indexCount, bool bRead, bool bWrite )
{
	glBindBuffer( GL_ARRAY_BUFFER, *static_cast<GLuint*>(pBufferId) );

#ifndef __GEAR_VR
	GLenum access = (bRead && bWrite)? GL_READ_WRITE : ( (bWrite)? GL_WRITE_ONLY : GL_READ_ONLY );
	return (uint*)glMapBuffer( GL_ELEMENT_ARRAY_BUFFER, access );
#else
	GLbitfield access = 0;
	if (bRead)	access |= GL_MAP_READ_BIT;
	if (bWrite)	access |= GL_MAP_WRITE_BIT;

	return (uint*)glMapBufferRange( GL_ARRAY_BUFFER, 0, indexCount, access );
#endif



}

void	CRenderer::UnlockIndexBuffer()
{
	glUnmapBuffer( GL_ARRAY_BUFFER );
}

void	CRenderer::BindIndexBuffer( void* pBufferId, size_t indexCount )
{
	glBindVertexArray( ((SIndexBufferId*)pBufferId)->vertexArrayId );

//	glBindBuffer( GL_ELEMENT_ARRAY_BUFFER, ((SIndexBufferId*)pBufferId)->indexBufferId );
	static_cast< CSpecificData* >( m_pSpecificData )->m_boundIndexBuffer = (SIndexBufferId*)pBufferId;
}

void	CRenderer::UnbindIndexBuffer()
{
	if ( static_cast< CSpecificData* >( m_pSpecificData )->m_boundIndexBuffer != nullptr )
	{
		glBindVertexArray( 0 );
		static_cast< CSpecificData* >( m_pSpecificData )->m_boundIndexBuffer = nullptr;
	}
}


void*	CRenderer::CreateTexture( uint width, uint height, uint mipMapCount, CPicture::EPictureFormat format, void* pPixelData )
{
	GLenum glFormat;
	GLenum type;
	bool bCompressed = false;
	
	switch ( format )
	{
		case CPicture::ePF_R8G8B8:
			glFormat = GL_RGB;
			type = GL_UNSIGNED_BYTE;
			break;
		case CPicture::ePF_R8G8B8A8:
			glFormat = GL_RGBA;
			type = GL_UNSIGNED_BYTE;
			break;
#ifndef __GEAR_VR
		case CPicture::ePF_B8G8R8:
			glFormat = GL_BGR;
			type = GL_UNSIGNED_BYTE;
			break;
		case CPicture::ePF_B8G8R8A8:
			glFormat = GL_BGRA;
			type = GL_UNSIGNED_BYTE;
			break;
		case CPicture::ePF_R8G8B8A8_DTX1:
			glFormat = GL_COMPRESSED_RGBA_S3TC_DXT1_EXT;
			type = GL_UNSIGNED_BYTE;
			bCompressed = true;
			break;
		case CPicture::ePF_R8G8B8A8_DTX3:
			glFormat = GL_COMPRESSED_RGBA_S3TC_DXT3_EXT;
			type = GL_UNSIGNED_BYTE;
			bCompressed = true;
			break;
		case CPicture::ePF_R8G8B8A8_DTX5:
			glFormat = GL_COMPRESSED_RGBA_S3TC_DXT5_EXT;
			type = GL_UNSIGNED_BYTE;
			bCompressed = true;
			break;
#endif
		default:
			return NULL;			
			break;
	}

	GLuint* pTextureId = new GLuint(0);
	
	glGenTextures(1, pTextureId );
	glBindTexture( GL_TEXTURE_2D, *pTextureId );

	if ( bCompressed )
	{ 
		mipMapCount = Max< uint >( mipMapCount, 1 );

		uint blockSize = 0;
#ifndef __GEAR_VR	
		blockSize = (glFormat == GL_COMPRESSED_RGBA_S3TC_DXT1_EXT) ? 8 : 16;
#endif
		uint offset = 0;

		for (uint level = 0; level < mipMapCount && (width || height); ++level)
		{
			uint size = ((width + 3) / 4)*((height + 3) / 4)*blockSize;
			glCompressedTexImage2D( GL_TEXTURE_2D, level, glFormat, width, height, 0, size, (char const*)pPixelData + offset );

			offset += size;
			width /= 2;
			height /= 2;
		}
	}
	else
	{
		glTexImage2D( GL_TEXTURE_2D, 0, glFormat, width, height, 0, glFormat, type, pPixelData );
		glGenerateMipmap( GL_TEXTURE_2D );
	}


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
	glActiveTexture( GetOpenGLTextureLayer( layer ) );
//	glEnable( GL_TEXTURE_2D );
	glBindTexture( GL_TEXTURE_2D, *((GLuint*)pTextureId) );
}

void	CRenderer::UnbindTexture( size_t layer )
{
	glActiveTexture( GetOpenGLTextureLayer( layer ) );
	glBindTexture( GL_TEXTURE_2D, 0 );	
	static_cast< CSpecificData* >( m_pSpecificData )->m_boundTextures[ layer ] = 0;
}

void*	CRenderer::CreateShader( char const* code, EShaderType shaderType )
{
	char const*	sources[ 8 ];
	size_t		sourcesCount = 0;

#ifndef __GEAR_VR
	static char const* shaderHeader = "#version 400\n #define __GEAR_VR 0\n";
#else
	static char const* shaderHeader = "#version 300 es\n #define __GEAR_VR 1\n"
		"#ifndef DISABLE_MULTIVIEW\n"
		"	#define DISABLE_MULTIVIEW 0\n"
		"#endif\n"
		"#define NUM_VIEWS 2\n"
		"#if defined( GL_OVR_multiview2 ) && ! DISABLE_MULTIVIEW\n"
		"	#extension GL_OVR_multiview2 : enable\n"
		"	layout(num_views=NUM_VIEWS) in;\n"
		"	#define VIEW_ID gl_ViewID_OVR\n"
		"#else\n"
		"	uniform lowp int ViewID;\n"
		"	#define VIEW_ID ViewID\n"
		"#endif\n";
#endif
	sources[ sourcesCount++ ] = shaderHeader;
	sources[ sourcesCount++ ] = code;


	GLuint* pShaderId = new GLuint();

	GLenum glShaderType = 0;
	switch (shaderType)
	{
	case EShaderType::VS: glShaderType = GL_VERTEX_SHADER; break;
	case EShaderType::PS: glShaderType = GL_FRAGMENT_SHADER; break;
	}

	*pShaderId = glCreateShader( glShaderType );
	if ( *pShaderId == 0 )
	{
		delete pShaderId;
		return nullptr;
	}

	glShaderSource( *pShaderId, sourcesCount, (const GLchar**)sources, NULL );

	return pShaderId;
}

bool	CRenderer::CompileShader( void* pShaderId, String& errorMessage )
{
	if ( pShaderId == nullptr )
	{ 
		errorMessage = "shaderId null";
		return false;
	}

	glCompileShader( *static_cast<GLuint*>(pShaderId) );

	GLint status;
	glGetShaderiv( *static_cast<GLuint*>(pShaderId), GL_COMPILE_STATUS, &status );
	if ( status == GL_FALSE )
	{ 
		GLint logLength;
		glGetShaderiv( *static_cast<GLuint*>(pShaderId), GL_INFO_LOG_LENGTH, &logLength );
		char* logStr = new char[ logLength + 1 ];

		glGetShaderInfoLog( *static_cast<GLuint*>(pShaderId), logLength, NULL, logStr );
		errorMessage = logStr;
		delete[] logStr;

		return false;
	}
	
	return true;
}

void	CRenderer::DestroyShader( void* pShaderId )
{
	if ( pShaderId != nullptr )
	{ 
		glDeleteShader( *static_cast<GLuint*>(pShaderId) );
		delete static_cast<GLuint*>(pShaderId);
	}
}


void*	CRenderer::CreateProgram()
{
	GLuint* pProgramId = new GLuint();

	*pProgramId = glCreateProgram();

	return pProgramId;
}

void	CRenderer::AttachShader( void* pProgramId, void* pShaderId )
{
	if ( pProgramId != nullptr && pShaderId != nullptr )
	{
		glAttachShader( *static_cast<GLuint*>(pProgramId), *static_cast<GLuint*>(pShaderId) );
	}	
}

void	CRenderer::DetachShader( void* pProgramId, void* pShaderId )
{
	if ( pProgramId != nullptr && pShaderId != nullptr )
	{
		glDetachShader( *static_cast<GLuint*>(pProgramId), *static_cast<GLuint*>(pShaderId) );
	}
}

void	CRenderer::BindAttribute( void* pProgramId, const String& attribute, uint index )
{
	if ( pProgramId != nullptr )
	{
		glBindAttribLocation( *static_cast<GLuint*>(pProgramId), index, attribute.c_str() );
	}
}

bool	CRenderer::LinkProgram( void* pProgramId, String& errorMessage )
{
	if ( pProgramId == nullptr )
	{ 
		errorMessage = "programId null";
		return false;
	}

	glLinkProgram( *static_cast<GLuint*>(pProgramId) );

	GLint status;
	glGetProgramiv( *static_cast<GLuint*>(pProgramId), GL_LINK_STATUS, &status );
	if ( status == GL_FALSE )
	{ 
		GLint logLength;
		glGetProgramiv( *static_cast<GLuint*>(pProgramId), GL_INFO_LOG_LENGTH, &logLength );
		char* logStr = new char[ logLength + 1 ];
		logStr[ logLength ] = '\0';

		glGetProgramInfoLog( *static_cast<GLuint*>(pProgramId), logLength, NULL, logStr );
		errorMessage = logStr;

		delete[] logStr;

		return false;
	}
	
	return true;
}

void	CRenderer::DestroyProgram( void* pProgramId )
{
	if ( pProgramId != nullptr )
	{
		glDeleteProgram( *static_cast<GLuint*>(pProgramId) );
		delete static_cast<GLuint*>(pProgramId);
	}
}

void	CRenderer::BindProgram( void* pProgramId )
{
	if ( pProgramId != nullptr )
	{
		glUseProgram( *static_cast<GLuint*>(pProgramId) );
	}
	else
	{
		glUseProgram( 0 );
	}
}

void	CRenderer::SetUniformInt( void* pProgramId, const String& name, int number )
{
	if ( pProgramId != nullptr )
	{
		uint location = glGetUniformLocation( *static_cast<GLuint*>(pProgramId), name.c_str() );
		glUniform1i( location, number );
	}
}

void	CRenderer::SetUniformVec3( void* pProgramId, const String& name, const Vec3& vec )
{
	if ( pProgramId != nullptr )
	{
		uint location = glGetUniformLocation( *static_cast<GLuint*>(pProgramId), name.c_str() );
		glUniform3fv( location, 1, &vec.x );
	}
}

void	CRenderer::SetUniformMatrix44( void* pProgramId, const String& name, const Matrix44& matrix )
{
	if ( pProgramId != nullptr )
	{
		uint location = glGetUniformLocation( *static_cast<GLuint*>(pProgramId), name.c_str() );
		glUniformMatrix4fv( location, 1, false, &matrix.a11 );
	}
}


void*	CRenderer::CreateUniformBuffer( size_t size, void* pData )
{
	return nullptr;
}

void*	CRenderer::LockUniformBuffer( void* pBufferId )
{
	return nullptr;
}

void	CRenderer::UnlockUniformBuffer( void* pBufferId )
{

}

void	CRenderer::UseUniformBuffer( void* pBufferId )
{

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

// void	CRenderer::SetProjectionMatrix( const Matrix44& projMatrix )
// {
// 	glMatrixMode( GL_PROJECTION );
// 	
// 	GLfloat mat[ 16 ];
// 	mat[ 0 ] = projMatrix.a11;
// 	mat[ 1 ] = projMatrix.a12;
// 	mat[ 2 ] = projMatrix.a13;
// 	mat[ 3 ] = projMatrix.a14;
// 	mat[ 4 ] = projMatrix.a21;
// 	mat[ 5 ] = projMatrix.a22;
// 	mat[ 6 ] = projMatrix.a23;
// 	mat[ 7 ] = projMatrix.a24;
// 	mat[ 8 ] = projMatrix.a31;
// 	mat[ 9 ] = projMatrix.a32;
// 	mat[ 10 ] = projMatrix.a33;
// 	mat[ 11 ] = projMatrix.a34;
// 	mat[ 12 ] = projMatrix.a41;
// 	mat[ 13 ] = projMatrix.a42;
// 	mat[ 14 ] = projMatrix.a43;
// 	mat[ 15 ] = projMatrix.a44;
// 			
// 	glLoadMatrixf( mat );
// }

void	CRenderer::FormatTransformMatrix( const Matrix34& inMatrix, Matrix44& outMatrix )
{
	Matrix34 m1;
	m1.SetAxisX(Vec3(1.0f, 0.0f, 0.0f));
	m1.SetAxisY(Vec3(0.0f, 0.0f, -1.0f));
	m1.SetAxisZ(Vec3(0.0f, 1.0f, 0.0f));

	Matrix34 matrix = m1 * inMatrix;

	// Right
	outMatrix.a11 = matrix.a11;
	outMatrix.a12 = matrix.a21;
	outMatrix.a13 = matrix.a31;
	outMatrix.a14 = 0.0f;

	// Up
	outMatrix.a21 = matrix.a12;
	outMatrix.a22 = matrix.a22;
	outMatrix.a23 = matrix.a32;
	outMatrix.a24 = 0.0f;

	// Backward
	outMatrix.a31 = matrix.a13;
	outMatrix.a32 = matrix.a23;
	outMatrix.a33 = matrix.a33;
	outMatrix.a34 = 0.0f;

	// Position
	outMatrix.a41 = matrix.a14;
	outMatrix.a42 = matrix.a24;
	outMatrix.a43 = matrix.a34;
	outMatrix.a44 = 1.0f;
}

void	CRenderer::CullFace( bool bBackFace )
{
	glFrontFace( (bBackFace)? GL_CCW : GL_CW );
}


void*	CRenderer::LoadFont( const String& fontPath, uint size )
{
	return nullptr;/*dtx_open_font(fontPath.c_str(), (int)size);*/
}

void	CRenderer::UseFont( void* pFontId, uint size )
{
/*	dtx_use_font( (dtx_font*)pFontId, (int)size );*/
}

void	CRenderer::RenderText( const String& text )
{
/*	dtx_string( text.c_str() );*/
}

// void	CRenderer::SetTransformMatrix( const Matrix34& m )
// {
// 	glMatrixMode( GL_MODELVIEW );
// 
// 	GLfloat mat[16];
// 	//mat[0] = matrix.a11;
// 	//mat[1] = matrix.a12;
// 	//mat[2] = matrix.a13;
// 	//mat[3] = matrix.a14;
// 	//mat[4] = matrix.a21;
// 	//mat[5] = matrix.a22;
// 	//mat[6] = matrix.a23;
// 	//mat[7] = matrix.a24;
// 	//mat[8] = matrix.a31;
// 	//mat[9] = matrix.a32;
// 	//mat[10] = matrix.a33;
// 	//mat[11] = matrix.a34;
// 	//mat[12] = matrix.a41;
// 	//mat[13] = matrix.a42;
// 	//mat[14] = -matrix.a43;
// 	//mat[15] = matrix.a44;
// 
// 	Matrix34 m1;
// 	m1.SetAxisX(Vec3(1.0f, 0.0f, 0.0f));
// 	m1.SetAxisY(Vec3(0.0f, 0.0f, -1.0f));
// 	m1.SetAxisZ(Vec3(0.0f, 1.0f, 0.0f));
// 
// 	Matrix34 m2 = m1.GetInvertedOrtho();
// 
// 	Matrix34 test = m1 * m2;
// 
// 	Matrix34 matrix = m1 * m * m2;
// 
// 	// Right
// 	mat[0] = matrix.a11;
// 	mat[1] = matrix.a21;
// 	mat[2] = matrix.a31;
// 	mat[3] = 0.0f;
// 	
// 	// Up
// 	mat[4] = matrix.a13;
// 	mat[5] = matrix.a23;
// 	mat[6] = matrix.a33;
// 	mat[7] = 0.0f;
// 	
// 	// Backward
// 	mat[8] = -matrix.a12;
// 	mat[9] = -matrix.a22;
// 	mat[10] = -matrix.a32;
// 	mat[11] = 0.0f;
// 	
// 	// Position
// 	mat[12] = matrix.a14;
// 	mat[13] = matrix.a24;
// 	mat[14] = matrix.a34;
// 	mat[15] = 1.0f;
// 
// 
// 	glLoadMatrixf( mat );
// }

CRenderWindow* CRenderer::CreateRenderWindow( size_t x, size_t y, size_t width, size_t height )
{
	CRenderWindow* pRenderWindow = new CRenderWindow( (uint)x, (uint)y, (uint)width, (uint)height );
	m_pRenderTargets.push_back( pRenderWindow );
	return pRenderWindow;
}

void CRenderer::BindRenderWindow( CRenderWindow* pWindow )
{
	glViewport( pWindow->GetX(), pWindow->GetY(), pWindow->GetWidth(), pWindow->GetHeight() );
}

void	CRenderer::RenderBoundTriangles( size_t indexCount )
{
	glDrawElements( GL_TRIANGLES, (GLsizei)indexCount, GL_UNSIGNED_INT, (void*)0 );
	//glEnd();
	//glFlush();
}

void	CRenderer::DrawLine( const Vec3& from, const Vec3& to, const Color& color )
{
	glLineWidth( 2.5f ); 
// 	glColor3f( color.r, color.g, color.b );
// 	glBegin( GL_LINES );
// 	glVertex3f( from.x, from.y, from.z );
// 	glVertex3f( to.x, to.y, to.z );
// 	glEnd();	
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
