#include "Render/MeshSerializer.h"

#include "DataStream.h"
#include "GlobalVariables.h"
#include "ResourceManager.h"
#include "LogSystem/LogSystem.h"

WHITEBOX_BEGIN

IResource*	CMeshSerializer::Load( CDataStream& dataStream, const CResourceDescriptor& descriptor )
{
	WbLog( "Default", "Loading... file size %zd", dataStream.GetSize() );


	// Vertex count
	int vertexCount = 0;
	if ( !dataStream.Read( vertexCount ) )	return NULL;
	WbLog( "Default",  "Vertex count %zd", vertexCount );

	// Vertex format
	CVertexFormat vertexFormat;
	
	int singleElementFlags = 0;
	if ( !dataStream.Read( singleElementFlags ) ) return NULL;
	vertexFormat.SetSingleElementsFlags( (int)singleElementFlags );
	WbLog( "Default",  "Single elem flags %zd\n", singleElementFlags );
	
	for( size_t iElem = 0 ; iElem < CVertexFormat::eME_Count ; ++iElem )
	{
		int elemCount = 0;
		if ( !dataStream.Read( elemCount ) ) return NULL;
		vertexFormat.SetMultipleElementCount( (CVertexFormat::EMultipleElement)iElem, (int)elemCount );
		WbLog( "Default",  "Multiple elem %zd count %d\n", iElem, elemCount );
	}
	
	vertexFormat.Build();
	WbLog( "Default", " Vertex format size : %zd\n", vertexFormat.GetSize() );
	
	// Vertices
	CMesh* pMesh = new CMesh();
	pMesh->SetSharedVertices( vertexFormat, vertexCount, dataStream.GetCursorData() );
	dataStream.SetCursor( dataStream.GetCursor() + vertexCount * vertexFormat.GetSize() );
	
	// Parts
	int partCount = 0;
	if ( !dataStream.Read( partCount ) ) return NULL;
	WbLog( "Default",  "Part count %zd\n", partCount );
	for( size_t iPart = 0 ; iPart < partCount ; ++iPart )
	{
		int indexCount = 0;	
		if ( !dataStream.Read( indexCount ) )
		{
			delete pMesh;
			return NULL;
		}
		WbLog( "Default",  "Index count %d\n", indexCount );
		
		CMesh::CPart* pMeshPart = new CMesh::CPart( *pMesh );
		pMeshPart->SetIndices( indexCount, (uint*)dataStream.GetCursorData() );
		dataStream.SetCursor( dataStream.GetCursor() + indexCount * sizeof(uint) );
		
		// Material
		String materialName;
		dataStream.ReadString( materialName );
		if ( !materialName.empty() )
		{
			WbLog( "Default",  "Material %s\n", materialName.c_str() );
			pMeshPart->SetMaterial( gVars->pResourceManager->GetResource< CMaterial >( materialName ) );
		}

		pMesh->AddPart( pMeshPart );		
	}	
	
	return pMesh;
}

WHITEBOX_END
