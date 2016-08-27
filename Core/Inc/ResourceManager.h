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
	void				AddUnloadResourceQuery( CResourceDescriptor& descriptor );
	void				UpdateResourceLoading(); // long !!!
	
	CResourceDescriptor*	AddResource( const String& path, size_t size = 0 );
	void					ParseResources( const String& resourceFolder );

	
	CResourceDescriptor*				GetResourceDescriptor( const String& path );

	template< class TResourceType >
	CResourcePointer< TResourceType >	GetResource( const String& path )
	{
		return CResourcePointer< TResourceType >( GetResourceDescriptor( path ) );
	}

	static	void ComputeResourceDependencies( const String& resourceRootDir, const String& path, std::vector< String >& dependencies );

private:

	void								GetRecursiveDependencies( CResourceDescriptor* pDescriptor, std::vector< CResourceDescriptor* >& dependencies );

	String								m_resourceRootDir;

	std::vector< CResourceType* >		m_resourceTypes;
	
	typedef Map< String, CResourceType* > TResourceTypeMap;
	TResourceTypeMap					m_resourceTypesByName;
	TResourceTypeMap					m_resourceTypesByExtension;
	
	std::vector< CResourceDescriptor* >	m_resourceLoadQueries;
	std::vector< CResourceDescriptor* >	m_resourceUnloadQueries;

	bool								m_bCheckForDependencyMode;
};

WHITEBOX_END

#endif
