#ifndef __MESHHELPER_H__
#define __MESHHELPER_H__

#include "BaseTypes.h"
#include "Mesh.h"

WHITEBOX_BEGIN

class CMeshPartHelper
{
public:	
	void					AddIndex( uint index );
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
	
	void	AddMeshPart();
	CMeshPartHelper*	GetMeshPart( size_t index );
	
	CMesh*	ConvertToMesh() const;
	
private:
	char*			BuildVertexArray( const CVertexFormat& vertexFormat ) const;
	CVertexFormat	GetVertexFormat() const;
	
	void			Print() const;
	
private:
	std::vector< Vec3 >	m_positionArray;
	std::vector< Vec3 > m_normalArray;
	std::vector< Vec3 > m_colorArray;
	std::vector< Vec2 > m_uv0Array;

	std::vector< CMeshPartHelper* > m_meshParts;
};

WHITEBOX_END

#endif