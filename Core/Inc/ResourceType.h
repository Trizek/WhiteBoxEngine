#ifndef __RESOURCETYPE_H__
#define __RESOURCETYPE_H__

#include "ResourceSerializer.h"
#include "ResourceSpecificManager.h"

WHITEBOX_BEGIN

class CResourceType
{
public:
	CResourceType()
		: m_pResourceSerializer(NULL)
	{
		m_pResourceSpecificManager = new CResourceSpecificManager();
	}
		
	CResourceType( const String& name, size_t index, IResourceSerializer* pSerializer, const String& extension )
		: m_name(name)
		, m_index(index)
		, m_pResourceSerializer(pSerializer)
	{
		m_pResourceSpecificManager = new CResourceSpecificManager();
		AddExtension( extension );
	}
		
	const String&	GetName() const
	{
		return m_name;
	}
	
	void			SetName( const String& name )
	{
		m_name = name;
	}
	
	size_t			GetIndex() const
	{
		return m_index;
	}
	
	void			SetIndex( size_t index )
	{
		m_index = index;
	}
	
	IResourceSerializer*	GetSerializer() const
	{
		return m_pResourceSerializer;
	}
	
	void			SetSerializer( IResourceSerializer* pSerializer )
	{
		m_pResourceSerializer = pSerializer;
	}
	
	void	AddExtension( const String& extension )
	{
		m_extensions.push_back( extension );
	}
	
	size_t	GetExtensionsCount() const
	{
		return m_extensions.size();
	}
	
	const String& GetExtension( size_t index ) const
	{
		return m_extensions[ index ];
	}
	
	CResourceSpecificManager*	GetResourceSpecificManager()
	{
		return m_pResourceSpecificManager;
	}

private:
	String						m_name;
	size_t						m_index; // index in ResourceManager
	IResourceSerializer*		m_pResourceSerializer;
	std::vector< String >		m_extensions;
	CResourceSpecificManager*	m_pResourceSpecificManager;
};

WHITEBOX_END

#endif