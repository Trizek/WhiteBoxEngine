#ifndef __VERTEXBUFFER_H__
#define __VERTEXBUFFER_H__

#include "../BaseTypes.h"


#include <stdio.h>
WHITEBOX_BEGIN

struct SVertexBoneWeights
{
	SVertexBoneWeights( float i0, float w0, float i1, float w1, float i2, float w2, float i3, float w3 )
	{
		indices[0] = i0; weights[0] = w0;
		indices[1] = i1; weights[1] = w1;
		indices[2] = i2; weights[2] = w2;
		indices[3] = i3; weights[3] = w3;
	}

	float	indices[4];
	float	weights[4];
};


class CVertexFormat
{
public:
	
	
	enum ESingleElement
	{
		eSE_Position = 0,
		eSE_Normal,
		eSE_Color,
		eSE_BoneWeights,
		eSE_Count,
	};
	
	enum EMultipleElement
	{
		eME_UV = 0,
		eME_Count,
	};
	
	static const size_t MaxMultipleElementCount = 4;
	
	CVertexFormat()
		: m_singleElementsFlags(0)
	{
		m_multipleElementsCounts[ eME_UV ] = 0;
	}
	
	size_t	GetSize() const
	{
		return m_size;
	}

	size_t	GetSingleElementOffset( ESingleElement element ) const
	{
		return m_elementOffsets[ element ];
	}
	
	int		GetSingleElementsFlags() const
	{
		return m_singleElementsFlags;
	}	
	
	bool	HasSingleElement( ESingleElement element ) const
	{
		return ( (m_singleElementsFlags & BIT( element )) != 0 );
	}
	
	size_t	GetMultipleElementOffset( EMultipleElement element, size_t instance ) const
	{
		return m_elementOffsets[ eSE_Count + element * MaxMultipleElementCount + instance ];
	}	
	
	uint	GetMultipleElementCount( EMultipleElement element ) const
	{
		return (uint)m_multipleElementsCounts[ element ];
	}

	void	AddSingleElement( ESingleElement element )
	{
		m_singleElementsFlags |= BIT( element );
	}
	
	void	AddMultipleElement( EMultipleElement element )
	{
		++m_multipleElementsCounts[ element ];
	}
	
	void	SetSingleElementsFlags( int flags )
	{
		m_singleElementsFlags = flags;
	}
	
	void	SetMultipleElementCount( EMultipleElement element, size_t count )
	{
		m_multipleElementsCounts[ element ] = count;
	}
	
	void	Build()
	{
		m_size = 0;
		if ( m_singleElementsFlags & BIT(eSE_Position) )
		{
			m_elementOffsets[ eSE_Position ] = m_size;
			m_size += 3 * sizeof(float);
		}
		if ( m_singleElementsFlags & BIT(eSE_Normal) )
		{
			m_elementOffsets[ eSE_Normal ] = m_size;
			m_size += 3 * sizeof(float);
		}
		if ( m_singleElementsFlags & BIT(eSE_Color) )
		{
			m_elementOffsets[ eSE_Color ] = m_size;
			m_size += 3 * sizeof(float);
		}	
		if (m_singleElementsFlags & BIT(eSE_BoneWeights))
		{
			m_elementOffsets[ eSE_BoneWeights ] = m_size;
			m_size += sizeof(SVertexBoneWeights);
		}

		for( size_t iElem = 0 ; iElem < eME_Count ; ++iElem )
		{
			for( size_t iInstance = 0 ; iInstance < m_multipleElementsCounts[ iElem ] ; ++iInstance )
			{
				m_elementOffsets[ eSE_Count + iElem * MaxMultipleElementCount + iInstance ] = m_size;
				m_size += 2 * sizeof(float);
			}
		}
	}

private:
	int		m_singleElementsFlags;
	size_t	m_multipleElementsCounts[ eME_Count ];
	
	size_t	m_size;
	size_t	m_elementOffsets[ eSE_Count + eME_Count * MaxMultipleElementCount ];
};


class CVertexBuffer
{
public:
	CVertexBuffer( const CVertexFormat& vertexFormat, size_t vertexCount, void* pVertices );
	~CVertexBuffer();
	
	const CVertexFormat&	GetVertexFormat() const;
	size_t					GetVertexCount() const;
	void*					GetBufferId() const;

	void*					Lock( bool bRead, bool bWrite );
	void					Unlock();

	Vec3&					GetPosition( void* pLockedData, size_t index ) const;
	Vec3&					GetNormal( void* pLockedData, size_t index ) const;
	Vec2&					GetUV0( void* pLockedData, size_t index ) const;
	SVertexBoneWeights&		GetBoneWeights( void* pLockedData, size_t index ) const;

private:
	CVertexFormat		m_vertexFormat;
	size_t				m_vertexCount;
	
	void*				m_pBufferId;
};

WHITEBOX_END

#endif
