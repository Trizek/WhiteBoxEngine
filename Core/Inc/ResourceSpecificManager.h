#ifndef __RESOURCESPECIFICMANAGER_H__
#define __RESOURCESPECIFICMANAGER_H__

#include "ResourceDescriptor.h"


WHITEBOX_BEGIN

class CResourceType;
class CResourceManager;
class CResourceSpecificManager
{
public:
	CResourceDescriptor*	AddResource( const String& name, const String& path, const String& extension, size_t size, CResourceType* pResourceType, CResourceManager* pResourceManager );
	CResourceDescriptor*	GetResourceDescriptor( const String& path );

private:
	typedef Map< String, CResourceDescriptor* > TResourceDescriptorMap;
	TResourceDescriptorMap	m_resources;
};

WHITEBOX_END

#endif