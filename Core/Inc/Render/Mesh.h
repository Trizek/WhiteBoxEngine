#ifndef __MESH_H__
#define __MESH_H__

#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "Material.h"

#include "Resource.h"
#include "TriMesh.h"

WHITEBOX_BEGIN

struct SVertexBlendWeight
{
	SVertexBlendWeight() = default;
	SVertexBlendWeight( float _index, float _weight ) : index(_index), weight(_weight) {}

	float	index, weight;
};

class CMesh : public IResource
{
public:
	DECLARE_RESOURCE_TYPE( CMesh );

	class CPart
	{
	public:
		CPart( CMesh& mesh );
		~CPart();
	
		CIndexBuffer*	GetIndexBuffer();
		void			SetIndices( size_t indexCount, uint const * pIndices );
		
		CMaterialPtr	GetMaterial();
		void			SetMaterial( CMaterialPtr pMaterial );
		
	private:
		CMesh&			m_mesh;
		CIndexBuffer*	m_pIndexBuffer;
		CMaterialPtr	m_pMaterial;
	};

	CMesh( CTriMeshPtr pTriMesh );
	~CMesh();
	
	CVertexBuffer*	GetVertexBuffer();
	void		SetSharedVertices( const CVertexFormat& vertexFormat, size_t vertexCount, void* pVertices );
	
	size_t		GetPartCount() const;
	CPart*		GetPart( size_t iPart );
	void		AddPart( CPart* pPart );

	CTriMeshPtr	GetTriMesh() const
	{
		return m_pTriMesh;
	}
	
	void		Render() const;
	
private:
	CVertexBuffer*	m_pSharedVertexBuffer;

	std::vector< CPart* >	m_parts;

	CTriMeshPtr		m_pTriMesh;
};

DECLARE_RESOURCE_POINTER( CMesh );

WHITEBOX_END

#endif