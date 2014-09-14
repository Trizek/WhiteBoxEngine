#include "Render/IndexBuffer.h"

#include "System.h"
#include "Render/Renderer.h"

WHITEBOX_BEGIN

CIndexBuffer::CIndexBuffer( size_t indexCount, uint const * pIndices )
	: m_indexCount(indexCount)
{
	m_pBufferId = gSystem->pRenderer->CreateIndexBuffer( indexCount, pIndices );
}

CIndexBuffer::~CIndexBuffer()
{
	gSystem->pRenderer->DestroyIndexBuffer( m_pBufferId );
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