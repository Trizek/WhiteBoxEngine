#include "ResourceSpecificManager.h"

WHITEBOX_BEGIN

CResourceDescriptor* CResourceSpecificManager::AddResource( const String& name, const String& path, const String& extension, size_t size, CResourceType* pResourceType, CResourceManager* pResourceManager )
{
<<<<<<< HEAD
	CResourceDescriptor* pDescriptor = new CResourceDescriptor( name, path, extension, size, false, pResourceType, pResourceManager );
=======
	CResourceDescriptor* pDescriptor = new CResourceDescriptor(name, path, extension, size, pResourceType, pResourceManager);
>>>>>>> master
	m_resources[ path ] = pDescriptor;
	return pDescriptor;
}

<<<<<<< HEAD
// CResourceDescriptor* CResourceSpecificManager::AddProceduralResource( const String& name, const String& extension, IResource* pResource, CResourceType* pResourceType, CResourceManager* pResourceManager )
// {
// 	String path = String("__PROCEDURAL__") + name;
// 	CResourceDescriptor* pDescriptor = new CResourceDescriptor(name, path, extension, 0, true, pResourceType, pResourceManager);
// 	pDescriptor->SetResource( pResource );
// 	m_resources[ path ] = pDescriptor;
// 	return pDescriptor;
// }

=======
>>>>>>> master
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
