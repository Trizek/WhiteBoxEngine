#include "MtlExporter.h"

WHITEBOX_BEGIN

void CMtlExporter::Export( const String& assetFolder, const String& resourceFolder, const String& filePath )
{
	String completeFilePath = assetFolder + filePath;
	
	TFileHandle file = gVars->pFileSystem->OpenFile( completeFilePath.c_str(), true, false );
	
	char buffer[256];

	String materialName;
	String textureName;
	CMaterialHelper matHelper;
	bool bMat = false;
	bool bTex = false;

#ifdef EXPORT_TO_PNG
	const char* extension = ".png";
#else
	const char* extension = ".dds";
#endif

	while( ReadWord( file, buffer ) )
	{
		if ( strcmp( buffer, "newmtl" ) == 0 )
		{
			if ( bMat )
			{
				if (bTex)
				{
					textureName.ConvertToUnixPath();
					textureName = textureName.get_path_base() + textureName.get_path_name() + extension;
					matHelper.m_textureLayers[0].m_textureName = filePath.get_path_base() + textureName;
				}

				gVars->pFileSystem->CreateFileDir(resourceFolder + filePath.get_path_base() + materialName + ".mat");
				matHelper.SaveToFile(resourceFolder + filePath.get_path_base() + materialName + ".mat");
				matHelper = CMaterialHelper();

				bMat = false;
				bTex = false;
			}

			if ( ReadWord( file, buffer ) )
			{
				materialName = buffer;
				bMat = true;
			}
		}
		else if ( strcmp( buffer, "map_Kd" ) == 0 )
		{
			if ( ReadWord( file, buffer ) )
			{
				textureName = ( *buffer == '/' || *buffer == '\\' )? buffer + 1 : buffer;
				bTex = true;
			}
		}
	}

	if ( bMat )
	{
		if ( bTex )
		{
			textureName.ConvertToUnixPath();
			textureName = textureName.get_path_base() + textureName.get_path_name() + ".dds";
			matHelper.m_textureLayers[0].m_textureName = filePath.get_path_base() + textureName;
		}

		gVars->pFileSystem->CreateFileDir(resourceFolder + filePath.get_path_base() + materialName + ".mat");
		matHelper.SaveToFile(resourceFolder + filePath.get_path_base() + materialName + ".mat");

		bTex = false;
	}
	
	gVars->pFileSystem->CloseFile( file );
}

WHITEBOX_END
