#include "Render/MeshSerializer.h"

#include "DataStream.h"
#include "GlobalVariables.h"
#include "ResourceManager.h"

WHITEBOX_BEGIN

IResource*	CMeshSerializer::Load( CDataStream& dataStream, const CResourceDescriptor& descriptor )
{
	printf("Loading... file size %d\n", dataStream.GetSize() );

	// Vertex count
	size_t vertexCount = 0;
	if ( !dataStream.Read( vertexCount ) )	return NULL;
	printf( "Vertex count %d\n", vertexCount );
	
	// Vertex format
	CVertexFormat vertexFormat;
	
	size_t singleElementFlags = 0;
	if ( !dataStream.Read( singleElementFlags ) ) return NULL;
	vertexFormat.SetSingleElementsFlags( singleElementFlags );
	printf( "Single elem flags %d\n", singleElementFlags );
	
	for( size_t iElem = 0 ; iElem < CVertexFormat::eME_Count ; ++iElem )
	{
		size_t elemCount = 0;
		if ( !dataStream.Read( elemCount ) ) return NULL;
		vertexFormat.SetMultipleElementCount( (CVertexFormat::EMultipleElement)iElem, elemCount );
		printf( "Multiple elem %d count %d\n", iElem, elemCount );
	}
	
	vertexFormat.Build();
	printf(" Vertex format size : %d\n", vertexFormat.GetSize() );
	
	// Vertices
	CMesh* pMesh = new CMesh();
	pMesh->SetSharedVertices( vertexFormat, vertexCount, dataStream.GetCursorData() );
	dataStream.SetCursor( dataStream.GetCursor() + vertexCount * vertexFormat.GetSize() );
	
	// Parts
	size_t partCount = 0;
	if ( !dataStream.Read( partCount ) ) return NULL;
	printf( "Part count %d\n", partCount );
	for( size_t iPart = 0 ; iPart < partCount ; ++iPart )
	{
		size_t indexCount = 0;	
		if ( !dataStream.Read( indexCount ) )
		{
			delete pMesh;
			return NULL;
		}
		printf( "Index count %d\n", indexCount );
		
		CMesh::CPart* pMeshPart = new CMesh::CPart();
		pMeshPart->SetIndices( indexCount, (uint*)dataStream.GetCursorData() );
		dataStream.SetCursor( dataStream.GetCursor() + indexCount * sizeof(uint) );
		
		// Material
		String materialName;
		dataStream.ReadString( materialName );
		if ( !materialName.empty() )
		{
			printf( "Material %s\n", materialName.c_str() );
			pMeshPart->SetMaterial( gVars->pResourceManager->GetResource< CMaterial >( materialName ) );
		}

		pMesh->AddPart( pMeshPart );		
	}	
	
	return pMesh;
}

WHITEBOX_END
