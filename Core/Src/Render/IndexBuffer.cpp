#include "Render/IndexBuffer.h"

#include "GlobalVariables.h"
#include "Render/Renderer.h"
#include "Render/VertexBuffer.h"

WHITEBOX_BEGIN

CIndexBuffer::CIndexBuffer( CVertexBuffer* pVertexBuffer, size_t indexCount, uint const * pIndices )
	: m_indexCount(indexCount)
{
#ifdef COOK
	m_pBufferId = nullptr;
#else
	m_pBufferId = gVars->pRenderer->CreateIndexBuffer( pVertexBuffer, indexCount, pIndices );
#endif
}

CIndexBuffer::~CIndexBuffer()
{
	gVars->pRenderer->DestroyIndexBuffer( m_pBufferId );
}

size_t CIndexBuffer::GetIndexCount() const
{
	return m_indexCount;
}

void* CIndexBuffer::GetBufferId() const
{
	return m_pBufferId;
}

WHITEBOX_END