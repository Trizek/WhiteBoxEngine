#include "Render/MeshHelper.h"

#include <math.h>
#include "GlobalVariables.h"
#include "FileSystem.h"
#include "LogSystem/LogSystem.h"
#include "Render/VertexBuffer.h"
#include "TriMesh.h"

WHITEBOX_BEGIN

void	CMeshPartHelper::AddIndex( uint index )
{
	m_indexArray.push_back( index );		
}

void	CMeshPartHelper::AddTriangle( uint i0, uint i1, uint i2 )
{
	AddIndex( i0 );
	AddIndex( i1 );
	AddIndex( i2 );
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
	
CMesh::CPart* CMeshPartHelper::ConvertToMeshPart( CMesh& mesh )
{
	CMesh::CPart* pMeshPart = new CMesh::CPart( mesh );
	
	pMeshPart->SetIndices( m_indexArray.size(), &m_indexArray[ 0 ] );
	return pMeshPart;
}

void CMeshPartHelper::Print() const
{
	WbLog( "Default",  "Index count : %zd\n", m_indexArray.size() );
	WbLog( "Default",  "Material name : %s\n", m_materialName.c_str() );
}


void	CMeshHelper::SaveToFile( const String& filePath ) const
{
	if (m_positionArray.size() != m_normalArray.size())
	{
		const_cast<CMeshHelper*>(this)->ComputeNormals(); 
	}


	TFileHandle file = gVars->pFileSystem->OpenFile( filePath.c_str(), false, true );
	
	// -----Shared vertices :
	// Count
	int vertexCount = (int)m_positionArray.size();
	gVars->pFileSystem->Write( file, sizeof(int), 1, &vertexCount );
	
	// Vertex Format
	CVertexFormat vertexFormat = GetVertexFormat();
	int vertexElemFlags = vertexFormat.GetSingleElementsFlags();

	gVars->pFileSystem->Write( file, sizeof(int), 1, &vertexElemFlags );
	
	for( size_t iElem = 0 ; iElem < CVertexFormat::eME_Count ; ++iElem )
	{
		uint elemCount = vertexFormat.GetMultipleElementCount( (CVertexFormat::EMultipleElement)iElem );
		gVars->pFileSystem->Write( file, 1, sizeof(uint), &elemCount );
	}
	
	// Vertices
	// ENDIANNESS !!!
	char* pVertices = BuildVertexArray( vertexFormat );
	gVars->pFileSystem->Write( file, m_positionArray.size() * vertexFormat.GetSize(), sizeof(char), pVertices );
	delete[] pVertices;
	
	// Parts
	int partCount = (int)m_meshParts.size();
	gVars->pFileSystem->Write( file, sizeof(int), 1, &partCount );
	for( size_t iPart = 0 ; iPart < m_meshParts.size() ; ++iPart )
	{
		std::vector< uint >& indexArray = m_meshParts[ iPart ]->GetIndexArray();
		int indexCount = (int)indexArray.size();
		gVars->pFileSystem->Write( file, sizeof(int), 1, &indexCount );
		gVars->pFileSystem->Write( file, sizeof(uint), indexCount, &indexArray[0] );
		gVars->pFileSystem->WriteString( file, m_meshParts[ iPart ]->GetMaterialName() );
	}
			
	
	gVars->pFileSystem->CloseFile( file );
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

void CMeshHelper::AddBlendWeight( size_t attributePosition, const SVertexBlendWeight& blendBone )
{
	m_blendBoneArray[ attributePosition ].push_back( blendBone );
}


void CMeshHelper::AddBlendWeight( size_t attributePosition, float boneIndex, float boneWeight )
{
	m_blendBoneArray[ attributePosition ].push_back( SVertexBlendWeight(boneIndex, boneWeight) );
}

void CMeshHelper::AddMeshPart()
{
	m_meshParts.push_back( new CMeshPartHelper() );
}

CMeshPartHelper*	CMeshHelper::GetMeshPart( size_t index )
{
	return m_meshParts[ index ];
}

void	CMeshHelper::ComputeNormals()
{
	m_normalArray.clear();
	m_normalArray.resize(m_positionArray.size());

	for (int iPart = 0; iPart < m_meshParts.size(); ++iPart)
	{
		CMeshPartHelper* pPart = m_meshParts[iPart];

		for (int i = 0; i < pPart->GetIndexArray().size(); i += 3)
		{
			Vec3 A = m_positionArray[ pPart->GetIndexArray()[i] ];
			Vec3 B = m_positionArray[pPart->GetIndexArray()[i + 1]];
			Vec3 C = m_positionArray[pPart->GetIndexArray()[i + 2]];

			Vec3 n = (B - A) * (C - A);
			n.Normalize();

			m_normalArray[pPart->GetIndexArray()[i]] = m_normalArray[pPart->GetIndexArray()[i]] + n;
			m_normalArray[pPart->GetIndexArray()[i+1]] = m_normalArray[pPart->GetIndexArray()[i+1]] + n;
			m_normalArray[pPart->GetIndexArray()[i+2]] = m_normalArray[pPart->GetIndexArray()[i+2]] + n;
		}
	}

	for (Vec3& n : m_normalArray)
	{
		n.Normalize();
	}
}
	
CMesh* CMeshHelper::ConvertToMesh() const
{
	CTriMeshPtr	pTriMesh = new CTriMesh();
	pTriMesh->ReserveVertices( m_positionArray.size() );
	for( const Vec3& pos : m_positionArray )
	{
		pTriMesh->AddVertex( pos );
	}

	for ( const CMeshPartHelper* pPart : m_meshParts )
	{
		const std::vector< uint >& indices = const_cast< CMeshPartHelper* >(pPart)->GetIndexArray();

		pTriMesh->ReserveIndices( indices.size() );
		for( uint index : indices )
		{
			pTriMesh->AddIndex( index );
		}
	}


	CMesh* pMesh = new CMesh( pTriMesh );

	if (m_positionArray.size() != m_normalArray.size())
	{
		const_cast<CMeshHelper*>(this)->ComputeNormals();
	}


	
	CVertexFormat vertexFormat = GetVertexFormat();
	
	WbLog( "Default",  "Creating %zd vertices of %zd bytes\n", m_positionArray.size(), vertexFormat.GetSize() );
	char* pVertices = BuildVertexArray( vertexFormat );
	pMesh->SetSharedVertices( vertexFormat, m_positionArray.size(), pVertices );	
	delete[] pVertices;
	
	for( size_t iPart = 0 ; iPart < m_meshParts.size() ; ++iPart )
	{
		pMesh->AddPart( m_meshParts[ iPart ]->ConvertToMeshPart( *pMesh ) );
	}
	
	return pMesh;
}


char*	 CMeshHelper::BuildVertexArray( const CVertexFormat& vertexFormat ) const
{
	size_t bufferSize = m_positionArray.size() * vertexFormat.GetSize();

	char* pVertices = new char[ bufferSize ];
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
			
			if ( m_normalArray.size() > 0 && m_normalArray.size() == m_positionArray.size())
			{
				*((float*)pCursor) = m_normalArray[ iVertex ].x;
				pCursor = pCursor + sizeof(float); 
				*((float*)pCursor) = m_normalArray[ iVertex ].y;
				pCursor = pCursor + sizeof(float); 
				*((float*)pCursor) = m_normalArray[ iVertex ].z;
				pCursor = pCursor + sizeof(float);
			}
			
			if ( m_colorArray.size() > 0 && m_colorArray.size() == m_positionArray.size() )
			{
				*((float*)pCursor) = m_colorArray[ iVertex ].x;
				pCursor = pCursor + sizeof(float); 
				*((float*)pCursor) = m_colorArray[ iVertex ].y;
				pCursor = pCursor + sizeof(float); 
				*((float*)pCursor) = m_colorArray[ iVertex ].z;
				pCursor = pCursor + sizeof(float);
			}		
			
			if ( m_uv0Array.size() > 0 && m_uv0Array.size() == m_positionArray.size() )
			{
				*((float*)pCursor) = m_uv0Array[ iVertex ].x;
				pCursor = pCursor + sizeof(float); 
				*((float*)pCursor) = m_uv0Array[ iVertex ].y;
				pCursor = pCursor + sizeof(float); 
			}

			if ( m_blendBoneArray[ 0 ].size() == m_positionArray.size() )
			{
				SVertexBoneWeights boneWeights( m_blendBoneArray[0][iVertex].index, m_blendBoneArray[0][iVertex].weight,
					m_blendBoneArray[1][iVertex].index, m_blendBoneArray[1][iVertex].weight,
					m_blendBoneArray[2][iVertex].index, m_blendBoneArray[2][iVertex].weight,
					m_blendBoneArray[3][iVertex].index, m_blendBoneArray[3][iVertex].weight );

				*((SVertexBoneWeights*)pCursor) = boneWeights;
				pCursor += sizeof(SVertexBoneWeights);
			}
		}
	}
	return pVertices;		
}

CVertexFormat CMeshHelper::GetVertexFormat() const
{
	CVertexFormat vertexFormat;
	vertexFormat.AddSingleElement( CVertexFormat::eSE_Position );

	if ( m_normalArray.size() > 0 && m_normalArray.size() == m_positionArray.size())
	{
		vertexFormat.AddSingleElement( CVertexFormat::eSE_Normal );
	}
	if ( m_colorArray.size() > 0 && m_colorArray.size() == m_positionArray.size())
	{
		vertexFormat.AddSingleElement( CVertexFormat::eSE_Color );
	}
	if ( m_uv0Array.size() > 0 && m_uv0Array.size() == m_positionArray.size())
	{
		vertexFormat.AddMultipleElement( CVertexFormat::eME_UV );
	}
	if ( m_blendBoneArray[0].size() == m_positionArray.size() )
	{
		vertexFormat.AddSingleElement( CVertexFormat::eSE_BoneWeights );
	}
	
	vertexFormat.Build();	
	
	return vertexFormat;
}

/*
	void LoadObj( const char* filePath )
	{
		TFileHandle file = gVars->pFileSystem->OpenFile( filePath, true, false );
		
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
					WbLog( "Default", " Found material %s for submesh %d\n", buffer, iSubmesh );
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
		
		gVars->pFileSystem->CloseFile( file );
		
		Print();
	}*/
	
void CMeshHelper::Print() const
{
	WbLog( "Default",  "Position count : %zd\n", m_positionArray.size() );
	WbLog( "Default",  "Normal count : %zd\n", m_normalArray.size() );
	WbLog( "Default",  "Color count : %zd\n", m_colorArray.size() );
	WbLog( "Default",  "UV0 count : %zd\n", m_uv0Array.size() );


	WbLog( "Default",  "Part count : %zd\n", m_meshParts.size() );
	for( size_t iPart = 0 ; iPart < m_meshParts.size() ; ++iPart )
	{
		WbLog( "Default",  "--- Part %zd ----\n", iPart );
		m_meshParts[ iPart ]->Print();
	}
}

WHITEBOX_END
