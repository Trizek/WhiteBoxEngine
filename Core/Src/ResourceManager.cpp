#include "ResourceManager.h"

#include "DataStream.h"
#include "FileSystem.h"
#include "System.h"
#include "ScriptSerializer.h"

#include "Render/MeshSerializer.h"
#include "Render/MaterialSerializer.h"
#include "Render/TextureSerializer.h"

WHITEBOX_BEGIN

CResourceManager::CResourceManager()
{
	AddResourceType< CMesh, CMeshSerializer >( "msh" );
	AddResourceType< CMaterial, CMaterialSerializer >( "mat" );
	AddResourceType< CTexture, CTextureSerializer >( "jpg" );
	AddResourceExtension< CTexture >( "tga" );
	AddResourceExtension< CTexture >( "dds" );
}

void CResourceManager::AddResourceType( const String& name, const String& extension, IResourceSerializer* pSerializer )
{
	CResourceType* pResourceType = new CResourceType( name, m_resourceTypes.size(), pSerializer, extension );
	m_resourceTypes.push_back( pResourceType );
	m_resourceTypesByName[ name ] = pResourceType;
	m_resourceTypesByExtension[ extension ] = pResourceType;
}

void CResourceManager::AddResourceExtension( const String& typeName, const String& extension )
{
	CResourceType* pResourceType = m_resourceTypesByName[ typeName ];
	pResourceType->AddExtension( extension );
	m_resourceTypesByExtension[ extension ] = pResourceType;
}

void CResourceManager::AddLoadResourceQuery( CResourceDescriptor& descriptor )
{
	m_resourceLoadQueries.push_back( &descriptor );
}

void CResourceManager::LoadQueriedResources()
{
	for( size_t iRes = 0 ; iRes < m_resourceLoadQueries.size() ; ++iRes )
	{
		// Load the resource
		CResourceDescriptor* pDescriptor = m_resourceLoadQueries[ iRes ];
		printf( "Loading resource %s of type %s\n...", pDescriptor->GetName().c_str(), pDescriptor->GetResourceType()->GetName().c_str() );
		
		CDataStream dataStream = CDataStream::GetStreamFromFile( pDescriptor->GetPath(), pDescriptor->GetSize() );
		
		IResource* pResource = pDescriptor->GetResourceType()->GetSerializer()->Load( dataStream, *pDescriptor );
		if ( pResource == NULL )
		{
			printf( "FAILED loading resource %s\n", pDescriptor->GetName().c_str() );
		}
		else
		{
			printf( "Succeeded loading resource %s\n", pDescriptor->GetName().c_str() );
		}
		
		pDescriptor->SetResource( pResource );
		
		dataStream.Destroy();
	}
	
	m_resourceLoadQueries.clear();
}

void CResourceManager::AddResource( const String& path, size_t size /*=0*/ )
{
	size_t pointIndex = path.find( '.' );
	if ( pointIndex == String::npos || pointIndex == 0 )
	{
		return;
	}
	
	String extension = path.substr( pointIndex + 1 );
	TResourceTypeMap::iterator resTypeIt = m_resourceTypesByExtension.find( extension );
	if ( resTypeIt == m_resourceTypesByExtension.end() )
	{
		return;
	}
	CResourceType* pResourceType = resTypeIt->second;
	
	size_t nameStartIndex = pointIndex-1;
	while( nameStartIndex > 0 && (path.c_str()[ nameStartIndex-1 ] != '/') )
	{
		--nameStartIndex;
	}
	CString name = path.substr( nameStartIndex, pointIndex - nameStartIndex );
	
	if ( size == 0 )
	{
		TFileHandle file = gSystem->pFileSystem->OpenFile( path.c_str(), true, false );
		size = gSystem->pFileSystem->GetFileSize( file );
		gSystem->pFileSystem->CloseFile( file );	
	}
	
	printf( "Adding resource %s of extension %s and type %s and size %d, located at %s\n", name.c_str(), extension.c_str(), pResourceType->GetName().c_str(), (int)size, path.c_str() );
	
	pResourceType->GetResourceSpecificManager()->AddResource( name, path, extension, size, pResourceType, this );	
}

void CResourceManager::ParseResources( const String& resourceFolder )
{
	String resourceFolderFormat = resourceFolder;
	if ( !resourceFolderFormat.empty() && (resourceFolderFormat.c_str()[ resourceFolderFormat.length()-1 ] != '/') )
	{
		resourceFolderFormat = resourceFolderFormat + "/";
	}
	CDataStream dataStream = CDataStream::GetStreamFromFile( resourceFolderFormat + "Resources.list" );

	CScriptStreamReader scriptStreamReader;
	SScriptNodePtr pScriptNode = scriptStreamReader.Parse( dataStream );
	
	CScriptNodeReader scriptNodeReader( pScriptNode );
	
	if ( scriptNodeReader.BeginGroup( "Resources" ) )
	{
		while ( scriptNodeReader.BeginGroup( "Resource" ) )
		{		
			String path;
			int size = 0;
			
			scriptNodeReader.Value( "path", path );
			scriptNodeReader.Value( "size", size );
			
			AddResource( path, size );
		
			scriptNodeReader.EndGroup();
		}
	
		scriptNodeReader.EndGroup();
	}
}

WHITEBOX_END
