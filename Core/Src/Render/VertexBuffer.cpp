#include "Render/VertexBuffer.h"

#include "GlobalVariables.h"
#include "Render/Renderer.h"

WHITEBOX_BEGIN

CVertexBuffer::CVertexBuffer( const CVertexFormat& vertexFormat, size_t vertexCount, void* pVertices )
	: m_vertexFormat(vertexFormat), m_vertexCount(vertexCount)
{
	m_pBufferId = gVars->pRenderer->CreateVertexBuffer( vertexCount * m_vertexFormat.GetSize(), pVertices );
}

CVertexBuffer::~CVertexBuffer()
{
	gVars->pRenderer->DestroyVertexBuffer( m_pBufferId );
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

void*	CVertexBuffer::Lock( bool bRead, bool bWrite )
{
	return gVars->pRenderer->LockVertexBuffer( m_pBufferId, bRead, bWrite );
}

void	CVertexBuffer::Unlock()
{
	gVars->pRenderer->UnlockVertexBuffer();
}

Vec3& CVertexBuffer::GetPosition( void* pLockedData, size_t index ) const
{
	return *(Vec3*)(((char*)pLockedData) + index * m_vertexFormat.GetSize());
}

Vec3& CVertexBuffer::GetNormal(void* pLockedData, size_t index) const
{
	return *(Vec3*)(((char*)pLockedData) + m_vertexFormat.GetSingleElementOffset(CVertexFormat::eSE_Normal) + index * m_vertexFormat.GetSize());
}

Vec2& CVertexBuffer::GetUV0( void* pLockedData, size_t index ) const
{
	return *(Vec2*)(((char*)pLockedData) + m_vertexFormat.GetMultipleElementOffset(CVertexFormat::eME_UV, 0) + index * m_vertexFormat.GetSize());
}

WHITEBOX_END