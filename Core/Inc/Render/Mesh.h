#ifndef __MESH_H__
#define __MESH_H__

#include "IndexBuffer.h"
#include "VertexBuffer.h"
#include "Material.h"

#include "Resource.h"

WHITEBOX_BEGIN

class CMesh : public IResource
{
public:
	DECLARE_RESOURCE_TYPE( CMesh );

	class CPart
	{
	public:
		CPart();
		~CPart();
	
		CIndexBuffer*	GetIndexBuffer();
		void			SetIndices( size_t indexCount, uint const * pIndices );
		
		CMaterialPtr	GetMaterial();
		void			SetMaterial( CMaterialPtr pMaterial );
		
	private:
		CIndexBuffer*	m_pIndexBuffer;
		CMaterialPtr	m_pMaterial;
	};

	CMesh();
	~CMesh();
	
	void		SetSharedVertices( const CVertexFormat& vertexFormat, size_t vertexCount, void* pVertices );
	
	size_t		GetPartCount() const;
	CPart*		GetPart( size_t iPart );
	void		AddPart( CPart* pPart );
	
	void		Render() const;
	
private:
	CVertexBuffer*	m_pSharedVertexBuffer;

	std::vector< CPart* >	m_parts;
};

WHITEBOX_END

#endif