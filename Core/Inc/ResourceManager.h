#ifndef __RESOURCEMANAGER_H__
#define __RESOURCEMANAGER_H__

#include "ResourceType.h"
#include "ResourceDescriptor.h"
#include "ResourcePointer.h"

WHITEBOX_BEGIN

class CResourceSpecificManager;
class CResourceManager
{
public:
	CResourceManager();

	void				AddResourceType( const String& name, const String& extension, IResourceSerializer* pSerializer );
	void				AddResourceExtension( const String& typeName, const String& extension );
	
	template< class TResourceType, class TResourceSerializerType >
	void				AddResourceType( const String& extension )
	{
		AddResourceType( TResourceType::GetStaticName(), extension, new TResourceSerializerType() );
	}
	
	template< class TResourceType >
	void				AddResourceExtension( const String& extension )
	{
		AddResourceExtension( TResourceType::GetStaticName(), extension );
	}	
	
	void				AddLoadResourceQuery( CResourceDescriptor& descriptor );
	void				LoadQueriedResources(); // long !!!
	
	void				AddResource( const String& path, size_t size = 0 );
	void				ParseResources( const String& resourceFolder );
	
	template< class TResourceType >
	CResourcePointer< TResourceType >	GetResource( const String& path )
	{
		TResourceTypeMap::iterator itResType = m_resourceTypesByName.find( TResourceType::GetStaticName() );
		if ( itResType == m_resourceTypesByName.end() )
		{
			printf( "Resource type %s not found\n", TResourceType::GetStaticName().c_str() );
			return CResourcePointer< TResourceType >( NULL );
		}
		CResourceType* pResourceType = itResType->second;
		CResourceDescriptor* pDescriptor = pResourceType->GetResourceSpecificManager()->GetResourceDescriptor( path );
		if ( pDescriptor == NULL )
		{
			printf( "Resource %s not found\n", path.c_str() );
		}
		
		return CResourcePointer< TResourceType >( pDescriptor );
	}
	

private:
	std::vector< CResourceType* >		m_resourceTypes;
	
	typedef Map< String, CResourceType* > TResourceTypeMap;
	TResourceTypeMap					m_resourceTypesByName;
	TResourceTypeMap					m_resourceTypesByExtension;
	
	std::vector< CResourceDescriptor* >	m_resourceLoadQueries;	
};

WHITEBOX_END

#endif
