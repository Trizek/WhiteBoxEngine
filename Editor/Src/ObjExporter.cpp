#include "ObjExporter.h"

#include "Render/MeshHelper.h"
#include "System.h"
#include "FileSystem.h"

WHITEBOX_BEGIN

void CObjExporter::Export( const String& assetFolder, const String& resourceFolder, const String& filePath )
{
	String completeFilePath = assetFolder + filePath;
	TFileHandle file = gSystem->pFileSystem->OpenFile( completeFilePath.c_str(), true, false );
	
	CMeshHelper meshHelper;
	size_t meshPartCount = 0;
	size_t vertexCount = 0;

	char buffer[256];
	while( ReadWord( file, buffer ) )
	{
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
			if ( ReadWord( file, buffer ) ){ pos.y = ToFloat( buffer ); }
			if ( ReadWord( file, buffer ) ){ pos.z = ToFloat( buffer ); }
			
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
			if ( ReadWord( file, buffer ) ){ uv.y = ToFloat( buffer ); }
			
			meshHelper.AddUV0( uv );
		}			
		else if ( meshPartCount > 0 && strcmp( buffer, "f" ) == 0 )
		{
			for( int index = 0 ; index < 3 ; ++index )
			{
				if ( ReadWord( file, buffer ) )
				{
					long int face = ToInt( buffer );
					uint index;
					if ( face < 0 )
					{
						index = (uint)(vertexCount + face);
					}
					else
					{
						index = (uint)(face - 1);
					}				
					meshHelper.GetMeshPart( meshPartCount - 1 )->AddIndex( index );
				}
			}
		}
		else if ( meshPartCount > 0 && strcmp( buffer, "usemtl" ) == 0 )
		{
			if ( ReadWord( file, buffer ) )
			{
				String materialName = resourceFolder + filePath.get_path_base() + buffer + ".mat";
				meshHelper.GetMeshPart( meshPartCount - 1 )->SetMaterialName( materialName );
			}
		}		
	}
	
	String meshPath = resourceFolder + filePath.get_path_base() + filePath.get_path_name() + ".msh";
	gSystem->pFileSystem->CreateFileDir( meshPath );
	meshHelper.SaveToFile( meshPath );	
	
	gSystem->pFileSystem->CloseFile( file );

}

WHITEBOX_END
