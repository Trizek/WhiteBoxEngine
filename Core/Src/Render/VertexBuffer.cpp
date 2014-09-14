#include "Render/VertexBuffer.h"

#include "System.h"
#include "Render/Renderer.h"

WHITEBOX_BEGIN

CVertexBuffer::CVertexBuffer( const CVertexFormat& vertexFormat, size_t vertexCount, void* pVertices )
	: m_vertexFormat(vertexFormat), m_vertexCount(vertexCount)
{
	m_pBufferId = gSystem->pRenderer->CreateVertexBuffer( vertexCount * m_vertexFormat.GetSize(), pVertices );
}

CVertexBuffer::~CVertexBuffer()
{
	gSystem->pRenderer->DestroyVertexBuffer( m_pBufferId );
}

const CVertexFormat& CVertexBuffer::GetVertexFormat() const
{
	return m_vertexFormat;
}

size_t CVertexBuffer::GetVertexCount() const
{
	return m_vertexCount;
}

void* CVertexBuffer::GetBufferId() const
{
	return m_pBufferId;
}

WHITEBOX_END