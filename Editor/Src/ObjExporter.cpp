#include "ObjExporter.h"

#include "Render/MeshHelper.h"
#include "GlobalVariables.h"
#include "FileSystem.h"
#include "System/OperatingSystem.h"
#include "Tokenizer.h"

enum class EObjKeyword
{
	v = 0,
	vt,
	vn,
	f,
	usemtl,
	mtllib,
	g,
};



int ConvertIndex( int index, int vertexCount )
{
	return (index < 0) ? vertexCount + index : index - 1;
}



WHITEBOX_BEGIN

struct SObjVert
{
	int	pos, tex, normal;
};

struct SObjFace
{
	std::vector< SObjVert > verts;
};

struct SObjPart
{
	String matName;
	std::vector< SObjFace > faces;
};

int		AddVertex( const SObjVert& vert, const std::vector<Vec3>& positions, const std::vector<Vec3>& normals, const std::vector<Vec2>& uvs, CMeshHelper& mshHelper )
{
	int index = (int)mshHelper.m_positionArray.size();
	
	mshHelper.AddPosition( positions[vert.pos] );
	
	if ( vert.normal >= 0 )
	{
		mshHelper.AddNormal( normals[vert.normal] );
	}

	if ( vert.tex >= 0 )
	{
		mshHelper.AddUV0( uvs[vert.tex] );
	}

	return index;
}

bool	ReadFloat( CTokenizer& tokenizer, float& val )
{
	tokenizer.ReadToken();
	if ( tokenizer.GetTokenType() == (int)ETokenType::eInt )
	{
		val = (float)tokenizer.GetInt();
		return true;
	}
	else if ( tokenizer.GetTokenType() == (int)ETokenType::eFloat )
	{
		val = tokenizer.GetFloat();
		return true;
	}

	return false;
}

bool	ReadInt( CTokenizer& tokenizer, int& val )
{
	tokenizer.ReadToken();
	if ( tokenizer.GetTokenType() == (int)ETokenType::eInt )
	{
		val = tokenizer.GetInt();
		return true;
	}

	return false;
}



void CObjExporter::Export( const String& assetFolder, const String& resourceFolder, const String& filePath )
{
	String meshPath = resourceFolder + filePath.get_path_base() + filePath.get_path_name() + ".msh";

	if ( gVars->pFileSystem->DoesFileExist( meshPath ) )
	{
		return;
	}

	String completeFilePath = assetFolder + filePath;
// 	TFileHandle file = gVars->pFileSystem->OpenFile( completeFilePath.c_str(), true, false );
// 	
	CDataStream dataStream;
	gVars->pOperatingSystem->GetDataStream( completeFilePath.c_str(), dataStream );

	CTokenizer tokenizer;
	tokenizer.SetDataStream( dataStream );
	tokenizer.AddKeyword( "v", (int)EObjKeyword::v );
	tokenizer.AddKeyword( "vt", (int)EObjKeyword::vt );
	tokenizer.AddKeyword( "vn", (int)EObjKeyword::vn );
	tokenizer.AddKeyword( "f", (int)EObjKeyword::f );
	tokenizer.AddKeyword( "mtllib", (int)EObjKeyword::mtllib );
	tokenizer.AddKeyword( "usemtl", (int)EObjKeyword::usemtl );
	tokenizer.AddKeyword( "g", (int)EObjKeyword::g );

	CMeshHelper meshHelper;
	size_t meshPartCount = 0;
	size_t vertexCount = 0;

	std::vector< Vec3 >	positions;
	std::vector< Vec3 > normals;
	std::vector< Vec2 > uvs; 
	std::vector< SObjPart > parts;

	
	tokenizer.ReadToken(); 
	while( tokenizer.GetTokenType() > 0 )
	{
		if ( tokenizer.GetTokenType() == '#' )
		{
			tokenizer.SkipLine();

			tokenizer.ReadToken();
			continue;
		}

		if ( tokenizer.GetTokenType() == (int)ETokenType::eIdentifier )
		{
			if ( tokenizer.GetKeyword() == (int)EObjKeyword::v )
			{
				Vec3 position;
				if ( ReadFloat(tokenizer, position.x) && ReadFloat(tokenizer, position.z) && ReadFloat(tokenizer, position.y) )
				{
					position.y = -position.y;
					positions.push_back( position );
				}

				tokenizer.SkipLine();
				tokenizer.ReadToken();
				continue;
			}

			if ( tokenizer.GetKeyword() == (int)EObjKeyword::vt )
			{
				Vec2 uv;
				if ( ReadFloat(tokenizer, uv.x) && ReadFloat(tokenizer, uv.y) )
				{
					uv.y = 1.0f - uv.y;
					uvs.push_back( uv );
				}

				tokenizer.SkipLine();
				tokenizer.ReadToken();
				continue;
			}

			if ( tokenizer.GetKeyword() == (int)EObjKeyword::vn )
			{
				Vec3 normal;
				if ( ReadFloat(tokenizer, normal.x) && ReadFloat(tokenizer, normal.z) && ReadFloat(tokenizer, normal.y) )
				{
					normal.y = -normal.y;
					normals.push_back( normal );
				}

				tokenizer.SkipLine();
				tokenizer.ReadToken();
				continue;
			}

			if ( tokenizer.GetKeyword() == (int)EObjKeyword::f )
			{
				if ( !tokenizer.ReadToken() )
				{
					break;
				}

				SObjFace face;

				while( true )
				{
					SObjVert vert = { -1, -1, -1 };

					if ( tokenizer.GetTokenType() != (int)ETokenType::eInt )
					{
						break;
					}

					vert.pos = ConvertIndex( tokenizer.GetInt(), positions.size() );
				
					tokenizer.ReadToken();
					if ( tokenizer.GetTokenType() == '/' )
					{
						tokenizer.ReadToken();
						if ( tokenizer.GetTokenType() == (int)ETokenType::eInt )
						{
							vert.tex = ConvertIndex( tokenizer.GetInt(), positions.size() );
							tokenizer.ReadToken();
						}
						
						if ( tokenizer.GetTokenType() == '/' )
						{
							tokenizer.ReadToken();
							if ( tokenizer.GetTokenType() == (int)ETokenType::eInt )
							{
								vert.normal = ConvertIndex( tokenizer.GetInt(), positions.size() );
								tokenizer.ReadToken();
							}
						}
					}

					face.verts.push_back( vert );
				}

				if ( !parts.empty() )
				{
					parts.back().faces.push_back( face );
				}

		/*		tokenizer.ReadToken();*/
				continue;
			}

			if ( tokenizer.GetKeyword() == (int)EObjKeyword::g )
			{
				parts.push_back( SObjPart() );

				tokenizer.SkipLine();
				tokenizer.ReadToken();
				continue;
			}

			if ( tokenizer.GetKeyword() == (int)EObjKeyword::usemtl )
			{
				if ( !parts.empty() )
				{
					tokenizer.ReadToken();

					if ( tokenizer.GetTokenType() == (int)ETokenType::eIdentifier )
					{
						parts.back().matName = filePath.get_path_base() + tokenizer.GetString() + ".mat";
						tokenizer.ReadToken();
					}

					continue;
				}

				tokenizer.ReadToken();
				continue;
			}

			if ( tokenizer.GetKeyword() == (int)EObjKeyword::mtllib )
			{
				tokenizer.SkipLine();

				tokenizer.ReadToken();
				continue;
			}

			tokenizer.ReadToken();
			continue;
		}

		tokenizer.ReadToken();
		continue;
	}

	for( SObjPart& part : parts )
	{
		CMeshPartHelper* pPart = meshHelper.AddMeshPart();
		
		pPart->SetMaterialName( part.matName );
		
		for( SObjFace& face : part.faces )
		{
			int v0 = AddVertex(face.verts[0], positions, normals, uvs, meshHelper);
			int v1 = AddVertex(face.verts[1], positions, normals, uvs, meshHelper);
			int vlast = v1;

			for (int i = 2; i < (int)face.verts.size(); ++i)
			{
				int vi = AddVertex(face.verts[i], positions, normals, uvs, meshHelper);

				pPart->AddIndex(v0);
				pPart->AddIndex(vlast);
				pPart->AddIndex(vi);

				vlast = vi;
			}
		}
	}



	/*

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

	*/
	

	dataStream.Destroy();

	gVars->pFileSystem->CreateFileDir( meshPath );
	meshHelper.SaveToFile( meshPath );	
	

}

WHITEBOX_END
