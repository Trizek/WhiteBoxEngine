#include "Render/TextMesh.h"

#include "FontSystem.h"
#include "GlobalVariables.h"
#include "Render/Font.h"
#include "Render/Renderer.h"
#include "Render/MeshHelper.h"
#include "Render/RenderPipeline.h"
#include "Render/Material.h"

WHITEBOX_BEGIN


void	CTextMesh::SetText( const CText& text, const CFontPtr& pFont )
{
	bool bSetMat = false;
	if ( !m_pMesh || text.length() > m_characterCount )
	{
		CreateMesh( text.length() );
		bSetMat = true;
	}

	const Vec2& uvScale = pFont->GetUVScale();

	CVertexBuffer* pVertexBuffer = m_pMesh->GetVertexBuffer();
	void* pVertices = pVertexBuffer->Lock( false, true );

	Vec2 advance;
	for ( size_t i = 0; i < text.length(); ++i )
	{
		if ( text[ i ] == '\n' )
		{
			advance.x = 0.0f;
			advance.y -= 64.0f;
			continue;
		}
		const CFont::SCharacter& chara = pFont->GetCharater( text[ i ] );
		 
		Vec2 start( advance.x + chara.corner.x, advance.y + chara.corner.y );
		advance.x += chara.advance;
		Vec2 end( start.x + chara.size.x, start.y - chara.size.y );
		 
		Vec2 startCoord = Vec2( chara.pos.x * uvScale.x, chara.pos.y * uvScale.y );
		Vec2 endCoord = Vec2( startCoord.x + chara.size.x * uvScale.x, startCoord.y + chara.size.y * uvScale.y );

		pVertexBuffer->GetPosition( pVertices, 4 * i ) = Vec3( start.x, 0.0f, start.y );
		pVertexBuffer->GetPosition(pVertices, 4 * i + 1) = Vec3(end.x, 0.0f, start.y);
		pVertexBuffer->GetPosition(pVertices, 4 * i + 2) = Vec3(end.x, 0.0f, end.y) ;
		pVertexBuffer->GetPosition(pVertices, 4 * i + 3) = Vec3(start.x, 0.0f, end.y) ;
		 
		pVertexBuffer->GetUV0(pVertices, 4 * i) = Vec2(startCoord.x, startCoord.y);
		pVertexBuffer->GetUV0(pVertices, 4 * i + 1 ) = Vec2(endCoord.x, startCoord.y);
		pVertexBuffer->GetUV0(pVertices, 4 * i + 2) = Vec2(endCoord.x, endCoord.y);
		pVertexBuffer->GetUV0(pVertices, 4 * i + 3) = Vec2(startCoord.x, endCoord.y);
		 
		pVertexBuffer->GetNormal(pVertices, 4 * i) = Vec3(0, 1, 0);
		pVertexBuffer->GetNormal(pVertices, 4 * i + 1) = Vec3(0, 1, 0);
		pVertexBuffer->GetNormal(pVertices, 4 * i + 2) = Vec3(0, 1, 0);
		pVertexBuffer->GetNormal(pVertices, 4 * i + 3) = Vec3(0, 1, 0);
	}

	pVertexBuffer->Unlock();

	if ( m_pFont != pFont || bSetMat )
	{
		m_pMesh->GetPart(0)->SetMaterial( pFont->GetMaterial() );
		m_pFont = pFont;
	}
}


void	CTextMesh::CreateMesh( size_t characterCount )
{
	m_characterCount = characterCount;
	m_pMesh = CMeshPtr( new CMesh(nullptr) );

	CVertexFormat vertexFormat;
	vertexFormat.AddSingleElement(CVertexFormat::eSE_Position);
	vertexFormat.AddSingleElement(CVertexFormat::eSE_Normal);
	vertexFormat.AddMultipleElement(CVertexFormat::eME_UV);
	vertexFormat.Build();
	m_pMesh->SetSharedVertices(vertexFormat, m_characterCount * 4, nullptr);

	CMesh::CPart* pPart = new CMesh::CPart(*m_pMesh.get());
	uint* indices = new uint[6 * m_characterCount];
	for (size_t i = 0; i < m_characterCount; ++i)
	{
		indices[6 * i] = 0 + i * 4;
		indices[6 * i + 1] = 2 + i * 4;
		indices[6 * i + 2] = 1 + i * 4;

		indices[6 * i + 3] = 3 + i * 4;
		indices[6 * i + 4] = 2 + i * 4;
		indices[6 * i + 5] = 0 + i * 4;
	}
	pPart->SetIndices(6 * m_characterCount, indices);
	m_pMesh->AddPart(pPart);
	delete[] indices;
}

CMeshPtr	CTextMesh::GetMesh() const
{
	return m_pMesh;
}

WHITEBOX_END
