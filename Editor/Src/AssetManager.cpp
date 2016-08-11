#include "AssetManager.h"

#include "FileSystem.h"
#include "ScriptSerializer.h"

/*#include "FbxExporter.h"*/
#include "ObjExporter.h"
#include "MtlExporter.h"
#include "CopyExporter.h"
#include "ResourceManager.h"
#include "TextureExporter.h"

WHITEBOX_BEGIN

CAssetManager::CAssetManager()
{
/*	AddExporter( "fbx", new CFbxExporter() );*/

	AddExporter( "obj", new CObjExporter() );
	AddExporter( "mtl", new CMtlExporter() );
	
	AddExporter( "dds", new CCopyExporter() );
	AddExporter( "jpg", new CTextureExporter() );
	AddExporter( "tga", new CTextureExporter() );
	AddExporter( "bmp", new CTextureExporter() );

	AddExporter( "ps", new CCopyExporter() );
	AddExporter( "vs", new CCopyExporter() );
	AddExporter( "program", new CCopyExporter() );
	AddExporter( "msh", new CCopyExporter() );
	AddExporter( "ttf", new CCopyExporter() );

}

void	CAssetManager::AddExporter( const String& extension, IAssetExporter* pExporter )
{
	m_assetExporters[ extension ] = pExporter;
}

IAssetExporter*	CAssetManager::GetExporter( const String& extension )
{
	TAssetExporterMap::iterator itExporter = m_assetExporters.find( extension );
	if ( itExporter == m_assetExporters.end() )
	{
		return NULL;
	}
	
	return itExporter->second;
}

class CAssetBrowser : public IFileBrowser 
{
public:
	CAssetBrowser( CAssetManager& assetManager, const String& assetFolder, const String& resourceFolder )
		: m_assetManager(assetManager)
		, m_assetFolder(assetFolder)
		, m_resourceFolder(resourceFolder)
	{}

	virtual void OnBrowseFile( const String& filePath )
	{
		IAssetExporter* pExporter = m_assetManager.GetExporter( filePath.get_path_extension() );
		if ( pExporter == NULL )
		{
			printf( "Ignoring file %s...\n", filePath.c_str() );
			return;
		}
		
		String localFilePath( filePath.c_str() + m_assetFolder.length() );
		printf( "Exporting %s...\n", filePath.c_str() );
		
		pExporter->Export( m_assetFolder, m_resourceFolder, localFilePath );
	}
	
private:
	CAssetManager& m_assetManager;
	String	m_assetFolder;
	String	m_resourceFolder;
};

class CAddToResourceListFileBrowser : public IFileBrowser 
{
public:
	CAddToResourceListFileBrowser( ISerializer& serializer, const String& resourceFolder )
		: m_serializer(serializer)
		, m_resourceFolder(resourceFolder){}

	virtual void OnBrowseFile( const String& filePath )
	{
		if ( m_serializer.BeginGroup( "Resource" ) )
		{
			String mutableFilePath = filePath;
			m_serializer.Value( "path", CString( mutableFilePath.c_str() + m_resourceFolder.size() ) );
			
			TFileHandle file = gVars->pFileSystem->OpenFile( filePath.c_str(), true, false );
			int fileSize = (int)gVars->pFileSystem->GetFileSize( file );
			gVars->pFileSystem->CloseFile( file );

			std::vector< String > dependencies;
			CResourceManager::ComputeResourceDependencies( filePath, dependencies );
			
			for( String& dependency : dependencies )
			{
				m_serializer.BeginGroup( "Dependency" );
				m_serializer.Value( "path", dependency );
				m_serializer.EndGroup();
			}
			
			m_serializer.Value( "size", fileSize );
		}
		m_serializer.EndGroup();
	}
	
private:
	ISerializer& m_serializer;
	String	m_resourceFolder;
};

void	CAssetManager::GenerateResourceListFile( const String& resourceFolder )
{
	CScriptNodeWriter scriptNodeWriter;
	
	if ( scriptNodeWriter.BeginGroup( "Resources" ) )
	{
		CAddToResourceListFileBrowser fileBrowser( scriptNodeWriter, resourceFolder );
		gVars->pFileSystem->BrowseDirectory( resourceFolder, fileBrowser );
	
		scriptNodeWriter.EndGroup();	
	}

	CScriptFileWriter scriptFileWriter( CString("Resources.list").c_str() );
	scriptFileWriter.WriteNode( scriptNodeWriter.GetRootNode() );	
}

void	CAssetManager::Export( const String& assetFolder, const String& resourceFolder )
{
	String assetFolderFormat = assetFolder;
	if ( !assetFolderFormat.empty() && (assetFolderFormat.c_str()[ assetFolderFormat.length()-1 ] != '/') )
	{
		assetFolderFormat = assetFolderFormat + "/";
	}
	
	String resourceFolderFormat = resourceFolder;
	if ( !resourceFolderFormat.empty() && (resourceFolderFormat.c_str()[ resourceFolderFormat.length()-1 ] != '/') )
	{
		resourceFolderFormat = resourceFolderFormat + "/";
	}
	
	CAssetBrowser assetBrowser( *this, assetFolderFormat, resourceFolderFormat );
	gVars->pFileSystem->BrowseDirectory( assetFolderFormat, assetBrowser );
	
	GenerateResourceListFile( resourceFolderFormat );
}

WHITEBOX_END
