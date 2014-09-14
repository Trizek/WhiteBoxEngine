#include "ResourceSpecificManager.h"

WHITEBOX_BEGIN

void CResourceSpecificManager::AddResource( const String& name, const String& path, const String& extension, size_t size, CResourceType* pResourceType, CResourceManager* pResourceManager )
{
	m_resources[ path ] = new CResourceDescriptor( name, path, extension, size, pResourceType, pResourceManager );
}

CResourceDescriptor* CResourceSpecificManager::GetResourceDescriptor( const String& path )
{
	TResourceDescriptorMap::iterator itResDesc = m_resources.find( path );
	if ( itResDesc == m_resources.end() )
	{
		return NULL;
	}
	
	CResourceDescriptor* pDescriptor = itResDesc->second;
	return pDescriptor;
}

WHITEBOX_END
