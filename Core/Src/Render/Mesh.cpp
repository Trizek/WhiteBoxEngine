#include "Render/Mesh.h"

#include "GlobalVariables.h"
#include "Render/Renderer.h"

WHITEBOX_BEGIN


CMesh::CPart::CPart( CMesh& mesh )
	: m_mesh(mesh)
	, m_pIndexBuffer(NULL)
	{}

CMesh::CPart::~CPart()
{
	if ( m_pIndexBuffer != NULL )
	{
		delete m_pIndexBuffer;
	}
}

CIndexBuffer*	CMesh::CPart::GetIndexBuffer()
{
	return m_pIndexBuffer;
}


void	CMesh::CPart::SetIndices( size_t indexCount, uint const * pIndices )
{
	if ( m_pIndexBuffer != NULL )
	{
		delete m_pIndexBuffer;
	}
	
	m_pIndexBuffer = new CIndexBuffer( m_mesh.GetVertexBuffer(), indexCount, pIndices );
}

CMaterialPtr	CMesh::CPart::GetMaterial()
{
	return m_pMaterial;
}

void	CMesh::CPart::SetMaterial( CMaterialPtr pMaterial )
{
	m_pMaterial = pMaterial;
}
		

CMesh::CMesh()
	: m_pSharedVertexBuffer(NULL){}

CMesh::~CMesh()
{
	for( size_t iPart = 0 ; iPart < m_parts.size() ; ++iPart )
	{
		delete m_parts[ iPart ];
	}
}


CVertexBuffer*	CMesh::GetVertexBuffer()
{
	return m_pSharedVertexBuffer;
}

void	CMesh::SetSharedVertices( const CVertexFormat& vertexFormat, size_t vertexCount, void* pVertices )
{
	if ( m_pSharedVertexBuffer != NULL )
	{
		delete m_pSharedVertexBuffer;
	}
	m_pSharedVertexBuffer = new CVertexBuffer( vertexFormat, vertexCount, pVertices );
}

size_t		CMesh::GetPartCount() const
{
	return m_parts.size();
}

CMesh::CPart*	CMesh::GetPart( size_t iPart )
{
	return m_parts[ iPart ];
}

void		CMesh::AddPart( CPart* pPart )
{
	m_parts.push_back( pPart );
}

void		CMesh::Render() const
{
	if ( m_pSharedVertexBuffer == NULL )
	{
		return;
	}

	for( size_t iPart = 0 ; iPart < m_parts.size() ; ++iPart )
	{
		CIndexBuffer* pIndexBuffer = m_parts[ iPart ]->GetIndexBuffer();
		if ( pIndexBuffer == NULL )
		{
			continue;
		}
		CMaterialPtr pMaterial = m_parts[ iPart ]->GetMaterial();
	
	
		gVars->pRenderer->BindIndexBuffer( pIndexBuffer->GetBufferId(), pIndexBuffer->GetIndexCount() );
		if ( pMaterial )
		{
			for( size_t iTexLayer = 0 ; iTexLayer < CMaterial::MAX_TEXTURE_LAYER ; ++iTexLayer )
			{
				if ( pMaterial->m_textureLayers[ iTexLayer ].m_pTexture )
				{
					gVars->pRenderer->BindTexture( pMaterial->m_textureLayers[ iTexLayer ].m_pTexture->GetTextureId(), iTexLayer );
				}
			}
		}
		
		gVars->pRenderer->RenderBoundTriangles( pIndexBuffer->GetIndexCount() );

		for( size_t iTexLayer = 0 ; iTexLayer < CMaterial::MAX_TEXTURE_LAYER ; ++iTexLayer )
		{
			gVars->pRenderer->UnbindTexture( iTexLayer );
		}	
	
		
		gVars->pRenderer->UnbindIndexBuffer();
	}
}

WHITEBOX_END
