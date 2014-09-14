#include "AssetManager.h"

#include "FileSystem.h"
#include "ScriptSerializer.h"

#include "FbxExporter.h"
#include "ObjExporter.h"
#include "MtlExporter.h"
#include "CopyExporter.h"

WHITEBOX_BEGIN

CAssetManager::CAssetManager()
{
	AddExporter( "fbx", new CFbxExporter() );

	AddExporter( "obj", new CObjExporter() );
	AddExporter( "mtl", new CMtlExporter() );
	
	AddExporter( "jpg", new CCopyExporter() );
	AddExporter( "tga", new CCopyExporter() );
	AddExporter( "dds", new CCopyExporter() );
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
	CAddToResourceListFileBrowser( ISerializer& serializer )
		: m_serializer(serializer){}

	virtual void OnBrowseFile( const String& filePath )
	{
		if ( m_serializer.BeginGroup( "Resource" ) )
		{
			String mutableFilePath = filePath;
			m_serializer.Value( "path", mutableFilePath );
			
			TFileHandle file = gSystem->pFileSystem->OpenFile( filePath.c_str(), true, false );
			int fileSize = (int)gSystem->pFileSystem->GetFileSize( file );
			gSystem->pFileSystem->CloseFile( file );
			
			m_serializer.Value( "size", fileSize );
		}
		m_serializer.EndGroup();
	}
	
private:
	ISerializer& m_serializer;
};

void	CAssetManager::GenerateResourceListFile( const String& resourceFolder )
{
	CScriptNodeWriter scriptNodeWriter;
	
	if ( scriptNodeWriter.BeginGroup( "Resources" ) )
	{
		CAddToResourceListFileBrowser fileBrowser( scriptNodeWriter );
		gSystem->pFileSystem->BrowseDirectory( resourceFolder, fileBrowser );
	
		scriptNodeWriter.EndGroup();	
	}

	CScriptFileWriter scriptFileWriter( (resourceFolder + "Resources.list").c_str() );
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
	gSystem->pFileSystem->BrowseDirectory( assetFolderFormat, assetBrowser );
	
	GenerateResourceListFile( resourceFolderFormat );
}

WHITEBOX_END
