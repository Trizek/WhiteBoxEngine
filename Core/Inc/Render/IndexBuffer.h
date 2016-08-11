#ifndef __INDEXBUFFER_H__
#define __INDEXBUFFER_H__

#include "../BaseTypes.h"

WHITEBOX_BEGIN

class CVertexBuffer;
class CIndexBuffer
{
public:
	CIndexBuffer( CVertexBuffer* pVertexBuffer, size_t indexCount, uint const * pIndices );
	~CIndexBuffer();
	
	size_t				GetIndexCount() const;
	void*				GetBufferId() const;

private:
	size_t				m_indexCount;
	
	void*				m_pBufferId;
};

WHITEBOX_END

#endif
