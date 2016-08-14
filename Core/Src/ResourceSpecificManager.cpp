#include "ResourceSpecificManager.h"

WHITEBOX_BEGIN

CResourceDescriptor* CResourceSpecificManager::AddResource( const String& name, const String& path, const String& extension, size_t size, CResourceType* pResourceType, CResourceManager* pResourceManager )
{
	CResourceDescriptor* pDescriptor = new CResourceDescriptor( name, path, extension, size, false, pResourceType, pResourceManager );

	m_resources[ path ] = pDescriptor;
	return pDescriptor;
}

CResourceDescriptor* CResourceSpecificManager::GetResourceDescriptor( const String& path )
{
	TResourceDescriptorMap::FindRes itResDesc = m_resources.FindElement( path );
	if ( itResDesc == nullptr )
	{
		return nullptr;
	}
	
	CResourceDescriptor* pDescriptor = *itResDesc;
	return pDescriptor;
}

WHITEBOX_END
