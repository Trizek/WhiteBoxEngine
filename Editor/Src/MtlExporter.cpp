#include "MtlExporter.h"

WHITEBOX_BEGIN

void CMtlExporter::Export( const String& assetFolder, const String& resourceFolder, const String& filePath )
{
	String completeFilePath = assetFolder + filePath;
	
	TFileHandle file = gSystem->pFileSystem->OpenFile( completeFilePath.c_str(), true, false );
	
	char buffer[256];
	while( ReadWord( file, buffer ) )
	{
		if ( strcmp( buffer, "newmtl" ) == 0 )
		{
			if ( ReadWord( file, buffer ) )
			{
				String materialName = buffer;
				CMaterialHelper matHelper;

				if ( ReadWord( file, buffer ) )
				{					
					if ( strcmp( buffer, "map_Kd" ) == 0 )
					{
						if ( ReadWord( file, buffer ) )
						{
							matHelper.m_textureLayers[ 0 ].m_textureName = resourceFolder + filePath.get_path_base() + buffer;
						}
					}	
				}
				
				gSystem->pFileSystem->CreateFileDir( resourceFolder + filePath.get_path_base() + materialName + ".mat" );
				matHelper.SaveToFile( resourceFolder + filePath.get_path_base() + materialName + ".mat" );
			}
		}
	}
	
	gSystem->pFileSystem->CloseFile( file );
}

WHITEBOX_END
