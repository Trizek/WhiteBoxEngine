#ifndef __TRIMESH_H__
#define __TRIMESH_H__

#include "BaseTypes.h"
#include "SmartPointer.h"

WHITEBOX_BEGIN

class CTriMesh : public CSmartPointerData
{
public:
	template< class TFunctor >
	void ForEachTriangle( TFunctor  functor ) const
	{
		for ( size_t i = 0; i < m_indices.size(); i += 3 )
 		{
			const Vec3& p1 = m_vertices[ m_indices[ i ] ]; 
			const Vec3& p2 = m_vertices[ m_indices[ i + 1 ] ];
			const Vec3& p3 = m_vertices[ m_indices[ i + 2 ] ];
		
			functor( p1, p2, p3 );
		}
	}
	
	void AddVertex( const Vec3& vert )
	{
		m_vertices.push_back( vert );
	}

	void AddIndex( uint index )
	{
		m_indices.push_back( index );
	}

	void ReserveVertices( size_t vertexCount )
	{
		m_vertices.reserve( m_vertices.size() + vertexCount );
	}

	void ReserveIndices( size_t indexCount )
	{
		m_indices.reserve( m_indices.size() + indexCount );
	}


	std::vector< Vec3 >	m_vertices;
	std::vector< uint >	m_indices;
};

DEFINE_SMART_PTR(CTriMesh)

WHITEBOX_END

#endif