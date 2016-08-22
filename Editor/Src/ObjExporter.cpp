#include "ObjExporter.h"

#include "Render/MeshHelper.h"
#include "GlobalVariables.h"
#include "FileSystem.h"

WHITEBOX_BEGIN

void CObjExporter::Export( const String& assetFolder, const String& resourceFolder, const String& filePath )
{
	String meshPath = resourceFolder + filePath.get_path_base() + filePath.get_path_name() + ".msh";

	if ( gVars->pFileSystem->DoesFileExist( meshPath ) )
	{
		return;
	}

	String completeFilePath = assetFolder + filePath;
	TFileHandle file = gVars->pFileSystem->OpenFile( completeFilePath.c_str(), true, false );
	
	CMeshHelper meshHelper;
	size_t meshPartCount = 0;
	size_t vertexCount = 0;

	char buffer[256];
	bool bConsumeWord = true;
	while( !bConsumeWord || ReadWord( file, buffer ) )
	{
		bConsumeWord = true;

		if ( strcmp( buffer, "#" ) == 0 || strcmp( buffer, "mtllib" ) == 0 )
		{
			SkipLine( file );
		}
		else if ( strcmp( buffer, "g" ) == 0 )
		{
			meshHelper.AddMeshPart();
			++meshPartCount;
		}
		else if ( strcmp( buffer, "v" ) == 0 )
		{
			Vec3 pos;
			if ( ReadWord( file, buffer ) ){ pos.x = ToFloat( buffer ); }
			if ( ReadWord( file, buffer ) ){ pos.z = ToFloat( buffer ); }
			if ( ReadWord( file, buffer ) ){ pos.y = -ToFloat( buffer ); }
			
			meshHelper.AddPosition( pos );
			
			++vertexCount;
		}
		else if ( strcmp( buffer, "vn" ) == 0 )
		{
			Vec3 normal;
			if ( ReadWord( file, buffer ) ){ normal.x = ToFloat( buffer ); }
			if ( ReadWord( file, buffer ) ){ normal.y = ToFloat( buffer ); }
			if ( ReadWord( file, buffer ) ){ normal.z = ToFloat( buffer ); }
			
			meshHelper.AddNormal( normal );
		}
		else if ( strcmp( buffer, "vt" ) == 0 )
		{
			Vec2 uv;
			if ( ReadWord( file, buffer ) ){ uv.x = ToFloat( buffer ); }
			if ( ReadWord( file, buffer ) ){ uv.y = 1.0f - ToFloat( buffer ); }
			
			meshHelper.AddUV0( uv );
		}			
		else if ( meshPartCount > 0 && strcmp( buffer, "f" ) == 0 )
		{
			std::vector< uint > indices;

			bConsumeWord = false;
 			while( ReadWord( file, buffer ) )
			{
				char* pCursor = nullptr;
				long int face = strtol( buffer, &pCursor, 10 );
				if ( pCursor == buffer )
				{
					break;
				}

				uint index;
				if (face < 0)
				{
					index = (uint)(vertexCount + face);
				}
				else
				{
					index = (uint)(face - 1);
				}

				indices.push_back( index );		
			}

			for ( uint i = 2; i < indices.size(); ++i )
			{
				meshHelper.GetMeshPart( meshPartCount - 1 )->AddIndex( indices[ 0 ] );
				meshHelper.GetMeshPart( meshPartCount - 1 )->AddIndex( indices[ i - 1 ] );
				meshHelper.GetMeshPart( meshPartCount - 1 )->AddIndex( indices[ i ] );
			}
		}
		else if ( meshPartCount > 0 && strcmp( buffer, "usemtl" ) == 0 )
		{
			if ( ReadWord( file, buffer ) )
			{
				String materialName = filePath.get_path_base() + buffer + ".mat";
				meshHelper.GetMeshPart(meshPartCount - 1)->SetMaterialName(materialName);
			}
		}		
	}
	

	gVars->pFileSystem->CreateFileDir( meshPath );
	meshHelper.SaveToFile( meshPath );	
	
	gVars->pFileSystem->CloseFile( file );

}

WHITEBOX_END
