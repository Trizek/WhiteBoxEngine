#include "Render/MeshHelper.h"

#include <math.h>
#include "System.h"
#include "FileSystem.h"

WHITEBOX_BEGIN

void	CMeshPartHelper::AddIndex( uint index )
{
	m_indexArray.push_back( index );		
}

std::vector< uint >& CMeshPartHelper::GetIndexArray() 
{
	return m_indexArray;
}

const String&	CMeshPartHelper::GetMaterialName() const
{
	return m_materialName;
}

void CMeshPartHelper::SetMaterialName( const String& materialName )
{
	m_materialName = materialName;
}
	
CMesh::CPart* CMeshPartHelper::ConvertToMeshPart()
{
	CMesh::CPart* pMeshPart = new CMesh::CPart();
	
	pMeshPart->SetIndices( m_indexArray.size(), &m_indexArray[ 0 ] );
	return pMeshPart;
}

void CMeshPartHelper::Print() const
{
	printf( "Index count : %zd\n", m_indexArray.size() );
	printf( "Material name : %s\n", m_materialName.c_str() );
}


void	CMeshHelper::SaveToFile( const String& filePath ) const
{
	TFileHandle file = gSystem->pFileSystem->OpenFile( filePath.c_str(), false, true );
	
	// -----Shared vertices :
	// Count
	size_t vertexCount = m_positionArray.size();
	gSystem->pFileSystem->Write( file, 1, sizeof(size_t), &vertexCount );
	
	// Vertex Format
	CVertexFormat vertexFormat = GetVertexFormat();
	int vertexElemFlags = vertexFormat.GetSingleElementsFlags();
	gSystem->pFileSystem->Write( file, 1, sizeof(int), &vertexElemFlags );
	
	for( size_t iElem = 0 ; iElem < CVertexFormat::eME_Count ; ++iElem )
	{
		uint elemCount = vertexFormat.GetMultipleElementCount( (CVertexFormat::EMultipleElement)iElem );
		gSystem->pFileSystem->Write( file, 1, sizeof(uint), &elemCount );
	}
	
	// Vertices
	// ENDIANNESS !!!
	char* pVertices = BuildVertexArray( vertexFormat );
	gSystem->pFileSystem->Write( file, m_positionArray.size() * vertexFormat.GetSize(), sizeof(char), pVertices );
	delete[] pVertices;
	
	// Parts
	size_t partCount = m_meshParts.size();
	gSystem->pFileSystem->Write( file, 1, sizeof(size_t), &partCount );
	for( size_t iPart = 0 ; iPart < m_meshParts.size() ; ++iPart )
	{
		std::vector< uint >& indexArray = m_meshParts[ iPart ]->GetIndexArray();
		size_t indexCount = indexArray.size();
		gSystem->pFileSystem->Write( file, 1, sizeof(size_t), &indexCount );
		gSystem->pFileSystem->Write( file, indexCount, sizeof(uint), &indexArray[0] );
		gSystem->pFileSystem->WriteString( file, m_meshParts[ iPart ]->GetMaterialName() );
	}
			
	
	gSystem->pFileSystem->CloseFile( file );
}
	
void CMeshHelper::AddPosition( const Vec3& position )
{
	m_positionArray.push_back( position );
}

void CMeshHelper::AddNormal( const Vec3& normal )
{
	m_normalArray.push_back( normal );
}

void CMeshHelper::AddColor( const Vec3& color )
{
	m_colorArray.push_back( color );
}			

void CMeshHelper::AddUV0( const Vec2& uv )
{
	m_uv0Array.push_back( uv );
}

void CMeshHelper::AddMeshPart()
{
	m_meshParts.push_back( new CMeshPartHelper() );
}

CMeshPartHelper*	CMeshHelper::GetMeshPart( size_t index )
{
	return m_meshParts[ index ];
}
	
CMesh* CMeshHelper::ConvertToMesh() const
{
	CMesh* pMesh = new CMesh();
	
	CVertexFormat vertexFormat = GetVertexFormat();
	
	printf( "Creating %zd vertices of %zd bytes\n", m_positionArray.size(), vertexFormat.GetSize() );
	char* pVertices = BuildVertexArray( vertexFormat );
	pMesh->SetSharedVertices( vertexFormat, m_positionArray.size(), pVertices );	
	delete[] pVertices;
	
	for( size_t iPart = 0 ; iPart < m_meshParts.size() ; ++iPart )
	{
		pMesh->AddPart( m_meshParts[ iPart ]->ConvertToMeshPart() );
	}
	
	return pMesh;
}


char*	 CMeshHelper::BuildVertexArray( const CVertexFormat& vertexFormat ) const
{
	char* pVertices = new char[ m_positionArray.size() * vertexFormat.GetSize() ];
	{
		char* pCursor = pVertices;
		for( size_t iVertex = 0 ; iVertex < m_positionArray.size() ; ++iVertex )
		{
			*((float*)pCursor) = m_positionArray[ iVertex ].x;				
			pCursor = pCursor + sizeof(float); 
			*((float*)pCursor) = m_positionArray[ iVertex ].y;
			pCursor = pCursor + sizeof(float); 
			*((float*)pCursor) = m_positionArray[ iVertex ].z;
			pCursor = pCursor + sizeof(float); 	
			
			if ( m_normalArray.size() > 0 )
			{
				*((float*)pCursor) = m_normalArray[ iVertex ].x;
				pCursor = pCursor + sizeof(float); 
				*((float*)pCursor) = m_normalArray[ iVertex ].y;
				pCursor = pCursor + sizeof(float); 
				*((float*)pCursor) = m_normalArray[ iVertex ].z;
				pCursor = pCursor + sizeof(float);
			}
			
			if ( m_colorArray.size() > 0 )
			{
				*((float*)pCursor) = m_colorArray[ iVertex ].x;
				pCursor = pCursor + sizeof(float); 
				*((float*)pCursor) = m_colorArray[ iVertex ].y;
				pCursor = pCursor + sizeof(float); 
				*((float*)pCursor) = m_colorArray[ iVertex ].z;
				pCursor = pCursor + sizeof(float);
			}		
			
			if ( m_uv0Array.size() > 0 )
			{
				*((float*)pCursor) = m_uv0Array[ iVertex ].x;
				pCursor = pCursor + sizeof(float); 
				*((float*)pCursor) = m_uv0Array[ iVertex ].y;
				pCursor = pCursor + sizeof(float); 
			}								
		}
	}
	return pVertices;		
}

CVertexFormat CMeshHelper::GetVertexFormat() const
{
	CVertexFormat vertexFormat;
	vertexFormat.AddSingleElement( CVertexFormat::eSE_Position );

	if ( m_normalArray.size() > 0 )
	{
		vertexFormat.AddSingleElement( CVertexFormat::eSE_Normal );
	}
	if ( m_colorArray.size() > 0 )
	{
		vertexFormat.AddSingleElement( CVertexFormat::eSE_Color );
	}
	if ( m_uv0Array.size() > 0 )
	{
		vertexFormat.AddMultipleElement( CVertexFormat::eME_UV );
	}
	
	vertexFormat.Build();	
	
	return vertexFormat;
}

/*
	void LoadObj( const char* filePath )
	{
		TFileHandle file = gSystem->pFileSystem->OpenFile( filePath, true, false );
		
		uint subMeshVertexCount = 0;
		uint meshVertexCount = 0;
		int iSubmesh = -1;
		bool bFacesAdded = true;
		char buffer[256];
	
				subMeshVertexCount = 0;
	
				
		while( ReadWord( file, buffer ) )
		{
			if ( strcmp( buffer, "g" ) == 0 || strcmp( buffer, "s" ) == 0 )
			{
				++iSubmesh;
				m_meshParts.push_back( new CMeshPartHelper() );	
			}
			else if ( iSubmesh >= 0 )
			{ 
				if ( strcmp( buffer, "usemtl" ) == 0 )
				{
					ReadWord( file, buffer );
					m_meshParts[ iSubmesh ]->SetMaterialName( buffer );
					printf(" Found material %s for submesh %d\n", buffer, iSubmesh );
				}
				else if ( strcmp( buffer, "v" ) == 0 )
				{
					Vec3 v;
				
					ReadWord( file, buffer ); v.x = ToFloat( buffer );
					ReadWord( file, buffer ); v.y = ToFloat( buffer );
					ReadWord( file, buffer ); v.z = ToFloat( buffer );
		
					AddPosition( v );
					
					++subMeshVertexCount;
					++meshVertexCount;
				}
				else if ( strcmp( buffer, "vn" ) == 0 )
				{
					Vec3 v;
				
					ReadWord( file, buffer ); v.x = ToFloat( buffer );
					ReadWord( file, buffer ); v.y = ToFloat( buffer );
					ReadWord( file, buffer ); v.z = ToFloat( buffer );
					
					AddNormal( v );
				}
				else if ( strcmp( buffer, "vt" ) == 0 )
				{
					Vec2 v;
				
					ReadWord( file, buffer ); v.x = ToFloat( buffer );
					ReadWord( file, buffer ); v.y = ToFloat( buffer );
					
					AddUV0( v );
				}	
				else if ( iSubmesh >=0 && strcmp( buffer, "f" ) == 0 )
				{
					{
						ReadWord( file, buffer );
						long int face = ToInt( buffer );
						uint index;
						if ( face < 0 )
						{
							index = subMeshVertexCount + (uint)face;
						}
						else
						{
							uint subMeshStartFace = meshVertexCount - subMeshVertexCount;
							index = (uint)face - 1 - subMeshStartFace;
						}
						m_meshParts[ iSubmesh ]->AddIndex( index );
					}
					{
						ReadWord( file, buffer );
						long int face = ToInt( buffer );
						uint index;
						if ( face < 0 )
						{
							index = subMeshVertexCount + (uint)face;
						}
						else
						{
							uint subMeshStartFace = meshVertexCount - subMeshVertexCount;
							index = (uint)face - 1 - subMeshStartFace;
						}
						m_meshParts[ iSubmesh ]->AddIndex( index );
					}
					{
						ReadWord( file, buffer );
						long int face = ToInt( buffer );
						uint index;
						if ( face < 0 )
						{
							index = subMeshVertexCount + (uint)face;
						}
						else
						{
							uint subMeshStartFace = meshVertexCount - subMeshVertexCount;
							index = (uint)face - 1 - subMeshStartFace;
						}
						m_meshParts[ iSubmesh ]->AddIndex( index );
					}										
					
				}
				else
				{
					ConsumeLine( file );
				}										
			}
		}
		
		gSystem->pFileSystem->CloseFile( file );
		
		Print();
	}*/
	
void CMeshHelper::Print() const
{
	printf( "Position count : %zd\n", m_positionArray.size() );
	printf( "Normal count : %zd\n", m_normalArray.size() );
	printf( "Color count : %zd\n", m_colorArray.size() );
	printf( "UV0 count : %zd\n", m_uv0Array.size() );


	printf( "Part count : %zd\n", m_meshParts.size() );
	for( size_t iPart = 0 ; iPart < m_meshParts.size() ; ++iPart )
	{
		printf( "--- Part %zd ----\n", iPart );
		m_meshParts[ iPart ]->Print();
	}
}

WHITEBOX_END
