#include "ResourceSpecificManager.h"

WHITEBOX_BEGIN

CResourceDescriptor* CResourceSpecificManager::AddResource( const String& name, const String& path, const String& extension, size_t size, CResourceType* pResourceType, CResourceManager* pResourceManager )
{
	CResourceDescriptor* pDescriptor = new CResourceDescriptor(name, path, extension, size, pResourceType, pResourceManager);
	m_resources[ path ] = pDescriptor;
	return pDescriptor;
}

CResourceDescriptor* CResourceSpecificManager::GetResourceDescriptor( const String& path )
{
	TResourceDescriptorMap::iterator itResDesc = m_resources.find( path );
	if ( itResDesc == m_resources.end() )
	{
		return nullptr;
	}
	
	CResourceDescriptor* pDescriptor = itResDesc->second;
	return pDescriptor;
}

WHITEBOX_END
