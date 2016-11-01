#ifndef __MESHHELPER_H__
#define __MESHHELPER_H__

#include "BaseTypes.h"
#include "Mesh.h"

WHITEBOX_BEGIN

class CMeshPartHelper
{
public:	
	void					AddIndex( uint index );
	void					AddTriangle( uint i0, uint i1, uint i2 );
	std::vector< uint >&	GetIndexArray();
	
	const String&			GetMaterialName() const;
	void					SetMaterialName( const String& materialName );
	
	CMesh::CPart*			ConvertToMeshPart( CMesh& mesh );
	
	void					Print() const;	
		
private:
	std::vector< uint > m_indexArray;
	String				m_materialName;
};

class CMeshHelper
{
public:
	void	SaveToFile( const String& filePath ) const;
	
	void	AddPosition( const Vec3& position );
	void	AddNormal( const Vec3& normal );
	void	AddColor( const Vec3& color );		
	void	AddUV0( const Vec2& uv );
	void	AddBlendWeight( size_t attributePosition, const SVertexBlendWeight& blendBone );
	void	AddBlendWeight( size_t attributePosition, float boneIndex, float boneWeight );

	
	CMeshPartHelper*	AddMeshPart();
	CMeshPartHelper*	GetMeshPart( size_t index );

	void	ComputeNormals();
	
	CMesh*	ConvertToMesh() const;
	
public:
	char*			BuildVertexArray( const CVertexFormat& vertexFormat ) const;
	CVertexFormat	GetVertexFormat() const;
	
	void			Print() const;

	
	std::vector< Vec3 >	m_positionArray;
	std::vector< Vec3 > m_normalArray;
	std::vector< Vec3 > m_colorArray;
	std::vector< Vec2 > m_uv0Array;

	std::vector< SVertexBlendWeight > m_blendBoneArray[ 4 ];

	std::vector< CMeshPartHelper* > m_meshParts;
};

WHITEBOX_END

#endif